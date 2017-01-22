#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include "prerequisites.h"
#include "../pcmdevice.h"
#include "../circularbuffer.h"
#include "stroboscope.h"
//#include "../../messages/messagelistener.h"

#include <vector>
#include <map>
#include <mutex>

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract adapter class for recording audio signals
///
/// The class has an internal circlar buffer which holds the incoming
/// audio data for a maximum of a few seconds. The user can retrieve
/// the data in form of vector (packet) by calling readAll(&packet).
///
/// This class has to be implemented by the actual sound device
/// implementation (AudioRecorderForQt). The implementation has to
/// call pushRawData.
///
/// The adapter incorporates an autonomous fully automatic level control.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN AudioRecorder : public PCMDevice
{
public:
    /// Floating point data type for a single PCM Value. The PCM values are
    /// assumed to be in [-1,1].
    typedef double PCMDataType;

    /// Type definition of a PCM packet (vector of PCM values).
    typedef std::vector<PCMDataType> PacketType;

    // Static constants, explained in the source file:

    static const int    BUFFER_SIZE_IN_SECONDS;     // size of circular buffer
    static const int    UPDATE_IN_MILLISECONDS;     // elementary packet size
    static const double ATTACKRATE;                 // for sliding level
    static const double DECAYRATE;                  // for sliding level
    static const double LEVEL_RETRIGGER;            // level for retriggering
    static const double LEVEL_TRIGGER;              // level where rec. starts
    static const double LEVEL_CUTOFF;               // highest allowed level
    static const double DB_OFF;                     // dB shift for off mark

public:
    AudioRecorder();                 ///< Constructor
    virtual ~AudioRecorder() {}                 ///< Empty destructor

    virtual void open(AudioInterface *audioInterface) override final;

    void readAll(PacketType &packet);       // Read all buffered data
    void cutSilence (PacketType &packet);   // Cut off trailing silence

    void resetInputLevelControl();          // Reset level control
    double getStopLevel() const { return mStopLevel; }

    Stroboscope *getStroboscope() {return &mStroboscope;}
    void setStandby (bool flag) { mStandby = flag; }
    void setWaitingFlag (bool flag) { mWaiting = flag; }
private:

    virtual int64_t read(char *, int64_t) override final {return 0;}
    virtual int64_t write(const char *data, int64_t max_bytes) override final;


    void setMuted(bool muted);              // Mute the input device





    // The implementation calls the following functions:
    void pushRawData (const PacketType &data);

private:
    bool   mMuted;              ///< Is the input device muted
    double mGain;               ///< Recording amplification factor
    int    mCounter;            ///< Counts counting incoming PCM values
    int    mCounterThreshold;   ///< Counter threshold for updating energy
    double mPacketM1;           ///< First intensity moment of a single packet
    double mPacketM2;           ///< Second intensity moment of a single packet
    double mSlidingLevel;       ///< Sliding VU level of the signal
    double mStopLevel;          ///< Level at which recording stops
    bool   mRecording;          ///< Flag true if recording is on
    bool   mRestartable;        ///< Flag true if start/retriggering possible
    bool   mWaiting;            ///< Wait for the data analysis to be completed
    bool   mStandby;            ///< Standby flag
    int    mPacketCounter;      ///< Counter for the number of packages

    std::map <int,double> mIntensityHistogram;      ///< Histogram of intensities

    CircularBuffer<PCMDataType> mCurrentPacket;  ///< Local audio buffer
    mutable std::mutex mCurrentPacketMutex;         ///< Buffer access mutexbo

    Stroboscope mStroboscope;      ///< Instance of stroboscope

    double convertIntensityToLevel (double intensity);          // map for VU meter
    double convertLevelToIntensity (double level);              // inverse map VU meter
    void   controlRecordingState(double level);                 // switch recording on/off
    void   automaticControl (double intensity, double level);   // automatic input level control
};


#endif // AUDIORECORDER_H
