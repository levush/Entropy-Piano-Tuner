#include "audiorecorder.h"

#include "stroboscope.h"
#include "../audiointerface.h"
#include "../../messages/messagerecorderenergychanged.h"
#include "../../messages/messagemodechanged.h"
#include "../../messages/messagehandler.h"
#include "../../math/mathtools.h"
#include "../../system/log.h"

//-----------------------------------------------------------------------------
//                            Various constants
//-----------------------------------------------------------------------------

/// Capacity of the local circular audio buffer in seconds
const int    AudioRecorder::BUFFER_SIZE_IN_SECONDS = 2;

/// Update interval in milliseconds, defining the packet size.
const int    AudioRecorder::UPDATE_IN_MILLISECONDS = 50;

/// Attack rate at which the sliding level goes up (1=instantly).
const double AudioRecorder::ATTACKRATE = 0.97;

/// Decay rate at which the sliding level goes down.
const double AudioRecorder::DECAYRATE  = 0.7;

/// Level below which retriggering (restart) is allowed.
const double AudioRecorder::LEVEL_RETRIGGER = 0.3;

/// Level above which the recorder starts to operate.
const double AudioRecorder::LEVEL_TRIGGER = 0.48;

/// Level above which the input mGain is automatically reduced.
const double  AudioRecorder::LEVEL_CUTOFF = 0.9;

/// dB shift for off mark (high value = shorter recording)
const double AudioRecorder::DB_OFF = 2;


//-----------------------------------------------------------------------------
//                             Constructor
//-----------------------------------------------------------------------------

AudioRecorder::AudioRecorder() :
      mMuted(false),            // input device is not muted
      mGain(1),                 // Gain factor amplifying the PCM signal
      mCounter(0),              // Counter counting incoming PCM values
      mCounterThreshold(0),     // Counter threshold for a single packet
      mPacketM1(0),             // First intensity moment of a single packet
      mPacketM2(0),             // Second intensity moment of a single packet
      mSlidingLevel(0),         // Sliding VU level of the signal
      mStopLevel(0.1),          // Level at which recording stops
      mRecording(false),        // Flag for recording
      mRestartable(true),       // Flag if recorder is restartable (retrigger)
      mWaiting(false),          // Wait for analysis to be completed
      mStandby(false),          // Flag for standby mode
      mPacketCounter(0),        // Counter for the number of packages
      mIntensityHistogram(),    // Histogram of intensities for level control
      mCurrentPacket(0),        // Local audio buffer
      mStroboscope(this)
{
}


void AudioRecorder::open(AudioInterface *audioInterface) {
    PCMDevice::open(audioInterface);
    mCurrentPacket.resize(getSampleRate() * BUFFER_SIZE_IN_SECONDS);
    mCounterThreshold =   getSampleRate() * UPDATE_IN_MILLISECONDS / 1000;
}

int64_t AudioRecorder::write(const char *data, int64_t max_bytes) {
    // convert data from DataType to PCMDataType
    PacketType packet(max_bytes / sizeof(DataType));
    const DataType *d = reinterpret_cast<const DataType *>(data);
    PCMDataType *p = packet.data();
    const PCMDataType *const pend = packet.data() + packet.size();

    while (p != pend) {
        *p = *d;
        *p /= std::numeric_limits<DataType>::max();
        ++p;
        ++d;
    }

    pushRawData(packet);

    return packet.size() * sizeof(DataType);
}

//---------------------------------------------l--------------------------------
//                        Reset input level control
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Reset input level control
///
/// This function resets the automatic input level control. It is usually
/// called when the reset button below the VU meter is pressed. The function
/// sets the input gain of the impemented input device to 1 by calling the
/// corresponding virtualmfunction. It also sets the local gain to 1 and
/// clears the intensity histogram.
///////////////////////////////////////////////////////////////////////////////

void AudioRecorder::resetInputLevelControl()
{
    if (mAudioInterface) {
        mAudioInterface->setGain(1);
    }

    mGain = 1;
    mIntensityHistogram.clear();
    mPacketCounter = 0;
}


//-----------------------------------------------------------------------------
//                           Mutes the input device
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set and reset the muting flag.
///
/// This will mute the input signal with the effect of sending 0 as input signal
/// and sending the value 0 to the VU meter.
/// \param muted : Activate or deactivate the muting
///////////////////////////////////////////////////////////////////////////////

void AudioRecorder::setMuted (bool muted)
{
    mMuted = muted;
}

//-----------------------------------------------------------------------------
//                      Read all data from the buffer
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Read all data from the internal buffer
///
/// Reads all data from the internal buffer into the packet vector and clears
/// the internal buffer. This function is called by the SignalAnalyzer.
/// \param packet : Reference to the packet where the new data is stored.
///////////////////////////////////////////////////////////////////////////////

void AudioRecorder::readAll(PacketType &packet)
{
    std::lock_guard<std::mutex> lock(mCurrentPacketMutex);   // lock the function
    packet = mCurrentPacket.getOrderedData();               // copy the content
    mCurrentPacket.clear();                                 // clear audio buffer
}


//-----------------------------------------------------------------------------
//          Convert signal intensity to a VU level and vice versa
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Convert an intensity (variance) of the signal to a VU level
/// \param intensity : Variance of the PCM signal
/// \return Level to be displayed by the VU meter (not clipped to [0,1])
///////////////////////////////////////////////////////////////////////////////

double AudioRecorder::convertIntensityToLevel (double intensity)
{ return pow(intensity*mGain*mGain,0.25); }


///////////////////////////////////////////////////////////////////////////////
/// \brief Convert a VU level to the corresponding intensity.
/// This function is the inverse of convertIntensityToLevel.
/// \param level : Level used to display in a VU meter
/// \return Corresponding intensity
///////////////////////////////////////////////////////////////////////////////

double AudioRecorder::convertLevelToIntensity (double level)
{ return pow(level,4.0)/mGain/mGain; }


//-----------------------------------------------------------------------------
//                Push raw PCM data from the implementation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief The implementation calls this function when new data is available.
///
/// This function is called by the implementation when newly read data is
/// ready to be pushed to the local buffer in the adapter. The implementation
/// has to convert the actual PCM data format to floating point values
/// in range [-1,1]. Then it has to call the present function.
///
/// The raw signal is multiplied by a gain factor mGain which is adjusted
/// dynamically during the recording process.
///
/// \param data : Vector containing the raw pcm data to be copied
///////////////////////////////////////////////////////////////////////////////

void AudioRecorder::pushRawData(const PacketType &data)
{
    if (data.size()==0) return;

    // Forward packet to the stroboscope
    mStroboscope.pushRawData (data);

    std::lock_guard<std::mutex> lock(mCurrentPacketMutex);

    // loop over all PCM values of the incoming data vector
    for (auto &pcmvalue : data)
    {
        mCurrentPacket.push_back(pcmvalue);
        mPacketM1 += pcmvalue;
        mPacketM2 += pcmvalue*pcmvalue;

        if (++mCounter > mCounterThreshold) // if packet is complete
        {
            // Compute the intensity (variance) of the packet and reset counter
            double intensity = (mPacketM2-mPacketM1*mPacketM1/mCounter)/mCounter;
            mPacketM1 = mPacketM2 = mCounter = 0;

            // Convert intensity to the corresponding VU level
            double level = MathTools::restrictToInterval(convertIntensityToLevel(intensity),0,1);

            // Compute a sliding level with fast attack and slow deay
            if (level > mSlidingLevel)
                  mSlidingLevel += (level-mSlidingLevel)*ATTACKRATE;
            else  mSlidingLevel -= (mSlidingLevel-level)*DECAYRATE;

            // If muted then the shown level is zero
            double shownLevel = (mMuted ? 0 : mSlidingLevel);

            // Send shown (muted) level to the GUI VU meter
            MessageHandler::send<MessageRecorderEnergyChanged>(MessageRecorderEnergyChanged::LevelType::LEVEL_INPUT, shownLevel);

            // Switch recording process on and off according to the shown (muted) level
            controlRecordingState (shownLevel);

            // Control the input level shown at the VU meter automaticall
            if (not mMuted) automaticControl (intensity,level);
        }
    }
}


//-----------------------------------------------------------------------------
//           Automatic noise estimation and threshold control
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Automatic noise estimation and threshold adjustment
///
/// This function constructs a histogram of the intensities of all incoming
/// packages. Typically this histogram has a pronounced peak at the left
/// edge, representing the background noise of the microphone in periods
/// of silence. The function locates the edges of the spectrum and adjusts
/// the noise threshold as well as the gain factor.
///
/// \param intensity : Intensity (variance) of the incoming packet.
/// \param level : Actual level (shown on the VU meter) of the packet.
///////////////////////////////////////////////////////////////////////////////

void AudioRecorder::automaticControl (double intensity, double level)
{
    if (intensity == 0) return;

    // Limit the maximal raw pcm intensity.
    // A sine wave with maximal amplitude +/- 1 has the integrated intensity 1/2.
    // Therefore, we reduce the hardware input level whenever this value
    // is surpassed. This down-regulation is irreversible.
    if (intensity > 0.45) mAudioInterface->setGain(mAudioInterface->getGain() * 0.9);

    // The level (passed as parameter) is a power law funtion of the intensity
    // scaled with the parameter mGain. The parameter mGain controls the overall
    // input amplification. It can move in both directions.

    // if level exceeds predefined cutoff turn gain down
    if (level > LEVEL_CUTOFF) mGain *= 0.9;

    // compute intensity histogram in decibel
    double dB = 4.34294 * log(intensity);
    mIntensityHistogram[MathTools::roundToInteger(dB)]++;

    // after 10 packets if the histogram has enough entries
    if ((++mPacketCounter)%10==0 and mIntensityHistogram.size()>5)
    {
        // determine left and right boundary in a soft way
        double norm=0, MIN=0, MAX=0;
        for (auto &e : mIntensityHistogram)
        {
            norm += e.second;
            MIN += pow(fabs(static_cast<double>(e.first)),10) * e.second;
            MAX += exp(e.first) * e.second;
        }
        if (norm==0) return;
        double dBoff = -pow(MIN/norm,1.0/10) + DB_OFF;
        double dBmax = log(MAX/norm);

        // slowly adjust the gain according to the upper edge
        double gain_target = exp(-dBmax/4.34294/2.0);
        mGain += 0.02*(gain_target-mGain);

        // slowly adjust the noise cutoff according to the lower edge
        double newStopLevel = convertIntensityToLevel(exp (dBoff/4.34294));
        if (newStopLevel < LEVEL_RETRIGGER  and fabs(newStopLevel-mStopLevel)>0.01)
        {
            // notify that stop level changed (reposition off level in mainwindow)
            mStopLevel += 0.1*(newStopLevel-mStopLevel);
            MessageHandler::send<MessageRecorderEnergyChanged>(
                        MessageRecorderEnergyChanged::LevelType::LEVEL_OFF, mStopLevel);
        }

        // After 10 packages decrease the whole histogram by a factor.
        // This ensures that new data gradually overwrites older data.
        if (mPacketCounter >= 10) for (auto &e: mIntensityHistogram) e.second /= 1.018;
    }
}


//-----------------------------------------------------------------------------
//              Control the beginning and the end of recording
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Control the beginning and the end of recording.
///
/// This function detects the beginning and the end of the recording period.
/// To this end it counts how many packages have a larger energy than the
/// recording trigger treshold. If there are several of them in a row, the
/// function sends a message that the recording started to all other modules.
///
/// The recording process can also be restarted while it is still running
/// provided that the energy falls below the mEnergyRt-threshold.
/// \param E : energy of the actual package
///////////////////////////////////////////////////////////////////////////////

void AudioRecorder::controlRecordingState (double level)
{
    // check for recording end
    // ------------------------------------------------------------------------

    if (mRecording and level < mStopLevel)
    {
        mRecording   = false;
        mRestartable = true;
        MessageHandler::send(Message::MSG_RECORDING_ENDED);
        LogI("Recording stopped");
    }

    // check for recording start (only if not in standby mode)
    // ------------------------------------------------------------------------

    if (mStandby or mWaiting) return;

    // If the level falls below LEVEL_RETRIGGER, the status of
    // the adapter is declared as restartable:
    if (not mRestartable and mSlidingLevel < LEVEL_RETRIGGER) mRestartable = true;

    // If the adapter is restartable or crosses the trigger recording starts
    if (level>LEVEL_TRIGGER and mRestartable)
    {
        if (mRecording) LogI("Recording retriggered")
        else            LogI("Recording started");
        mRecording   = true;
        mRestartable = false;
        mWaiting = true;
        MessageHandler::send(Message::MSG_RECORDING_STARTED);
    }
}


//-----------------------------------------------------------------------------
//               Cut away silence at the beginning of the buffer
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Cut trailing silence
///
/// The local buffer of the adapter has a length of a few seconds. When the
/// recording starts, the buffer will contain a period of silence before
/// the key was hit. This function removes this part of the vector.
/// \param packet : The packet with the audio PCM data (call by reference)
///////////////////////////////////////////////////////////////////////////////

void AudioRecorder::cutSilence (PacketType &packet)
{
    // determine the maximum amplitude in the packet
    double maxamplitude = 0;
    for (auto &y : packet) if (fabs(y)>maxamplitude) maxamplitude = fabs(y);
    double trigger = std::min(0.2,maxamplitude*maxamplitude/100);

    int w = getSampleRate() / 40;           // section width of 0.025 sec
    int sections = static_cast<int>(packet.size()) / w;         // number of sections
    if (sections < 2) return;                 // required: at least two sections
    size_t entries_to_delete = 0;             // number of sections to be deleted
    for (int sec = 0; sec < sections; ++sec)  // scan sectors
    {
        double energy = 0;
        for (int i = sec*w; i < (sec+1)*w; ++i) energy += packet[i] * packet[i];
        if (energy / w < trigger) entries_to_delete += w;
        else break;
    }
    // Remove the first silent sectors:
    EptAssert(entries_to_delete <= packet.size(),"inconsistent numbers of elements");
    if (entries_to_delete > 0)
        packet.erase(packet.begin(), packet.begin() + entries_to_delete);
}
