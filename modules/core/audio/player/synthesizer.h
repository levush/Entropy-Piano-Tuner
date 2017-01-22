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
//                      A simple sine wave synthesizer
//=============================================================================

#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include "prerequisites.h"

#include "waveformgenerator.h"
#include "../pcmdevice.h"
#include "../../system/simplethreadhandler.h"


//=============================================================================
//                  Structure describing an envelope
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/// \brief Structure describing the envelope (dynamics) of a sound
///
/// The envelope of synthesizer sounds follows the conventional ADSR scheme.
/// A first attack phase and a subsequent decay is followed by a sustain
/// period of constant volume while the key is pressed. Finally, when the
/// key is released, the envelope enters the release phase.
///////////////////////////////////////////////////////////////////////////////

struct Envelope
{
    double attack;      ///< Initial attack rate
    double decay;       ///< Subsequent decay rate
    double sustain;     ///< Sustain level
    double release;     ///< Release rate
    double hammer;      ///< Intensity of hammer noise

    Envelope(double attack=0, double decay=0,
             double sustain=0, double release=0,
             double hammer=0);
};

//=============================================================================
//                          Structure of a tone
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/// \brief Structure of a single tone.
///
/// This structure contains all data elements which characterize a single
/// tone. Each tone carries an identifying keynumber. The tone is characterized
/// mainly by the sound (static properties) and the envelope (dynamics).
///
/// The clock variable counts the number of samples from the beginning of
/// the tone. The clock_timeout limits the maximal duration of a tone.
/// The variables 'stage' indicates the dynamical state of the envelope.
///////////////////////////////////////////////////////////////////////////////

struct Tone
{
    int keynumber;                      ///< Identification tag (negativ=sine)
    double frequency;                   ///< Fundamental frequency
    double leftamplitude;               ///< Left stereo volume
    double rightamplitude;              ///< Right stereo volume
    double phaseshift;                  ///< Stereo phase shift
    Envelope envelope;                  ///< Dynamic properties of the tone

    int_fast64_t clock;                 ///< Running time in sample cycles.
    int stage;                          ///< 1=attack 2=decay 3=sustain 4=release.
    double amplitude;                   ///< current envelope amplitude

    WaveformGenerator::Waveform waveform; ///< Copy of the waveform
};


//=============================================================================
//                          Synthesizer class
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/// \brief Synthesizer class
///
/// This is the synthesizer of the EPT. It runs in an independent thread.
/// The acoustic system of the EPT is driven by messages. The SoundGenerator
/// listens to these messages and tells the Synthesizer which notes have to
/// be played. The Synthesizer in turn calls the WaveformGenerator to
/// pre-calculate PCM data for sound generation. The main task of the
/// synthesizer class is to create a real-time superposition of these
/// pre-calculated PCM waveforms. Moreover, it creates appropriate volume
/// differences and phase shifts between the two stereo channels.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN Synthesizer : public PCMDevice
{
public:

    using Spectrum = std::map<double,double>;   // type of spectrum

    Synthesizer ();

    virtual void open (AudioInterface *audioInterface) override final;
    virtual void close () override final { mWaveformGenerator.exit(); PCMDevice::close(); }

    void setNumberOfKeys (int numberOfKeys);

    void preCalculateWaveform   (const int id, const Spectrum &spectrum);

    void playSound              (const int id,
                                 const double frequency,
                                 const double volume,
                                 const Envelope &env,
                                 const bool waitforcomputation = false,
                                 const bool stereo = true);

    void ModifySustainLevel     (const int id, const double level);

    void releaseSound           (const int id);

    bool isPlaying              (const int id) const;


    WaveformGenerator &getWaveformGenerator() {return mWaveformGenerator;}


    virtual int64_t read(char *data, int64_t max_bytes) override final;
    bool generateAudioSignal(DataType *outputPacket, const int64_t packet_size);
    virtual int64_t write(const char *, int64_t) override final {return 0;}
private:
    using Waveform = WaveformGenerator::Waveform;

    WaveformGenerator mWaveformGenerator;

    int mNumberOfKeys;                      ///< Number of keys, passed in init()
    int mSampleRate;
    int mChannels;

    std::vector<Tone> mPlayingTones;        ///< Chord defined as a collection of tones.
    mutable std::mutex mPlayingMutex;       ///< Mutex to protect access to the chord.

    const int_fast64_t  SineLength = 16384; ///< sine value buffer length.
    const double CutoffVolume = 0.00001;    ///< Fade-out volume cutoff.

    Waveform mSineWave;                     ///< Sine wave vector, computed in init().

    static const std::vector< std::vector<int> > mHammerKnockFFT;
    FFTRealVector mHammerWaveLeft;          ///< Hammer noise, computed in init().
    FFTRealVector mHammerWaveRight;

    int mReverbSize,mReverbCounter;
    int mDelay1,mDelay2,mDelay3;
    std::vector<double> mReverbL,mReverbR;    ///< Reverb
    double mIntensity;

    const Tone* getSoundPointer (const int id) const;
    Tone* getSoundPointer (const int id);
    void updateIntensity();
};

#endif // SYNTHESIZER_H
