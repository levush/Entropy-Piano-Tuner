#ifndef PCMWRITERINTERFACE
#define PCMWRITERINTERFACE

#include "audiobase.h"

///
/// \brief Abstract class to write data into the AudioPlayerAdapter
///
class PCMWriterInterface {
public:
    ///
    /// \brief Init the interface with given channesl and sample rates
    /// \param sampleRate The sample rate
    /// \param channels The number of channels
    ///
    virtual void init(const int sampleRate, const int channels) {mSampleRate = sampleRate; mChannels = channels;}

    ///
    /// \brief Exit the interface
    ///
    virtual void exit() {}

    ///
    /// \brief Generate new samples and write them into the outputPacket
    /// \param outputPacket The output packet (write there)
    /// \returns true on success, false if the player should pause
    ///
    virtual bool generateAudioSignal(AudioBase::PacketType &outputPacket) = 0;

protected:
    int mSampleRate = 0;
    int mChannels = 0;
};

#endif // PCMWRITERINTERFACE

