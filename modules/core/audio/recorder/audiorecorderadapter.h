/*****************************************************************************
 * Copyright 2016 Haye Hinrichsen, Christoph Wick
 *
 * This file is part of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

//=============================================================================
//                          Adapter for audio input
//=============================================================================

#ifndef AUDIORECORDERADAPTER_H
#define AUDIORECORDERADAPTER_H

#include "prerequisites.h"
#include "../audiobase.h"
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

class EPT_EXTERN AudioRecorderAdapter : public AudioBase
{
public:
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
    AudioRecorderAdapter();                 ///< Constructor
    virtual ~AudioRecorderAdapter() {}                 ///< Empty destructor

    void resetInputLevelControl();          // Reset level control
    void setMuted(bool muted);              // Mute the input device

    void readAll(PacketType &packet);       // Read all buffered data
    void cutSilence (PacketType &packet);   // Cut off trailing silence

    double getStopLevel() const { return mStopLevel; }

    void setStandby (bool flag) { mStandby = flag; }
    void setWaitingFlag (bool flag) { mWaiting = flag; }

    Stroboscope *getStroboscope() {return &mStroboscope;}

protected:
    // The implementation calls the following functions:
    void pushRawData (const PacketType &data);
    virtual void setSamplingRate (int rate) override;

    // This class controls the input gain of the implementation
    virtual void setDeviceInputGain(double volume) = 0;
    virtual double getDeviceInputGain() const = 0;



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

#endif // AUDIORECORDERADAPTER_H
