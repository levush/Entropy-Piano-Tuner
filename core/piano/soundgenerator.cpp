/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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
//                 Sound generator driving the synthesizer
//=============================================================================

#include "soundgenerator.h"

#include <algorithm>

#include "../system/eptexception.h"
#include "../system/log.h"
#include "../system/simplethreadhandler.h"
#include "../messages/messagekeyselectionchanged.h"
#include "../messages/messagerecorderenergychanged.h"
#include "../messages/messagepreliminarykey.h"
#include "../messages/messagehandler.h"
#include "../messages/messagemidievent.h"
#include "../messages/messagemodechanged.h"
#include "../messages/messageprojectfile.h"
#include "../messages/messagefinalkey.h"
#include "../piano/piano.h"
#include "../piano/key.h"
#include "../math/mathtools.h"
#include "../settings.h"

//-----------------------------------------------------------------------------
//			                    Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, intiailizes the member variables
/// \param audioadapter : Pointer to the audio output implementation
///////////////////////////////////////////////////////////////////////////////

SoundGenerator::SoundGenerator (AudioPlayerAdapter *audioadapter) :
    mSynthesizer (audioadapter),
    mPiano(nullptr),
    mOperationMode(OperationMode::MODE_IDLE),
    mConcertPitch(0),
    mNumberOfKeys(0),
    mKeyNumberOfA4(0),
    mSelectedKey(-1),
    mResonatingKey(-1),
    mResonatingVolume(0)
{}


//-----------------------------------------------------------------------------
//            Play a resonating reference sound in the tuning mode
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Play a resonating reference sound in the tuning mode
///
/// To support accurate tuning, the EPT generates an imitation of the
/// recorded piano sound in the computed tune in the background. This allows
/// the user to tune the strings according to the sound in the earphone.
///
/// The following function starts such a "resonating reference sound".
/// To avoid interference with the MIDI playback system, which is also active
/// in the tuning mode, the synthesizer id's of the reference sound are
/// internally shifted by mNumberOfKeys.
///
/// \param keynumber : Number of the key.
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::playResonatingReferenceSound (int keynumber)
{
    if (mResonatingKey >= 0 and keynumber != mResonatingKey)
        stopResonatingReferenceSound();
    if (keynumber < 0 or keynumber >= mNumberOfKeys) return;
    if (mSynthesizer.isPlaying(mNumberOfKeys+keynumber)) return;
    auto &key = mPiano->getKey(keynumber);
    double frequ = key.getComputedFrequency()*mPiano->getConcertPitch()/440.0;
    if (frequ>0)
    {
        mResonatingKey=keynumber;
        mResonatingVolume = 0.2;
        switch (Settings::getSingleton().getSoundGeneratorMode())
        {
        case SGM_REFERENCE_TONE:
            playReferenceTone(key,keynumber,frequ, 0.5);
            break;
        case SGM_SYNTHESIZE_KEY:
            playOriginalSoundOfKey(mNumberOfKeys+keynumber,
                                   0, 50, 50, mResonatingVolume, 20);
            break;
        default:
            break;
        }
    }

}


//-----------------------------------------------------------------------------
//                    Stop the resonating reference sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stop the resonating reference sound
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::stopResonatingReferenceSound ()
{
//    if (mSynthesizer.isPlaying(mNumberOfKeys+mResonatingKey))
//        mSynthesizer.releaseSound(mNumberOfKeys+mResonatingKey);
//    mResonatingKey = -1;
//    mResonatingVolume = 0;
}


//-----------------------------------------------------------------------------
//			   Change the volume of the resonating reference sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Change the volume of the resonating reference sound
///
/// In order to produce an efficient interference, the volume of the
/// resonating reference sound is adjusted automatically to the actual
/// volume of the piano. To this end the 'level' sent to the VU meter is
/// mapped back to a volume and the sustain level of the reference sound
/// in the synthesizer is changed accordingly.
/// \param level : Level between 0 and 1
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::changeVolumeOfResonatingReferenceSound (double level)
{
    if (not mSynthesizer.isPlaying(mNumberOfKeys+mResonatingKey))
        { mResonatingKey=-1; return; }
    double truncatedlevel = std::min(0.8,level);
    double volume = 0.2*pow(truncatedlevel,2.0);
    if (volume > mResonatingVolume) mResonatingVolume = volume;
    else mResonatingVolume *= 0.87;
    mSynthesizer.ModifySustainLevel(mNumberOfKeys+mResonatingKey,mResonatingVolume);
}


//-----------------------------------------------------------------------------
//			               Message listener
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Sound generator message listener and dispatcher.
///
/// The sound generator is exclusively driven by messages. The present
/// function listens to the messages and takes action accordingly.
/// \param m : Pointer to the message.
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::handleMessage(MessagePtr m)
{
    EptAssert(m, "Message has to exist!");

    switch (m->getType())
    {
    case Message::MSG_KEY_SELECTION_CHANGED:
        {
            // keep track of selected key
            auto message(std::static_pointer_cast<MessageKeySelectionChanged>(m));
            mSelectedKey = message->getKeyNumber();
            stopResonatingReferenceSound();
            if (mOperationMode == MODE_TUNING)
            {
                playResonatingReferenceSound (mSelectedKey);
            }
        }
    break;
    case Message::MSG_RECORDING_STARTED:
        {
            if (mOperationMode==MODE_TUNING)
            {
                if (mSelectedKey>=0) playResonatingReferenceSound(mSelectedKey);
                else stopResonatingReferenceSound();
            }
        }
        break;
    case Message::MSG_RECORDING_ENDED:
        {
            if (mOperationMode == MODE_TUNING)
            {
                stopResonatingReferenceSound();
            }
        }
        break;
    case Message::MSG_RECORDER_ENERGY_CHANGED:
        {
            if (mOperationMode==MODE_TUNING and mResonatingKey>=0)
            {
                auto message(std::static_pointer_cast<MessageRecorderEnergyChanged>(m));
                if (message->getLevelType()==MessageRecorderEnergyChanged::LevelType::LEVEL_INPUT)
                {
                    if (Settings::getSingleton().isSoundGeneratorVolumeDynamic())
                    {
                        changeVolumeOfResonatingReferenceSound(message->getLevel());
                    }
                    else
                    {
                        changeVolumeOfResonatingReferenceSound(1);
                    }
                }
            }
        }
    break;
    case Message::MSG_PRELIMINARY_KEY:
        if (mOperationMode==MODE_TUNING and mSelectedKey<0)
        {
            auto message(std::static_pointer_cast<MessagePreliminaryKey>(m));
            int recognizedkey = message->getKeyNumber();
            if (mResonatingKey != recognizedkey)
            {
                playResonatingReferenceSound(recognizedkey);
            }
        }
        break;
    case Message::MSG_MODE_CHANGED:
        {
            // Keep track of mode changes
            auto message(std::static_pointer_cast<MessageModeChanged>(m));
            mOperationMode = message->getMode();
            stopResonatingReferenceSound();
            updateAllWaveforms();
        }
        break;
    case Message::MSG_PROJECT_FILE:
        {
            // Get a pointer to the piano and various of its properties.
            auto mpf(std::static_pointer_cast<MessageProjectFile>(m));
            mPiano = &(mpf->getPiano());
            mConcertPitch = mPiano->getConcertPitch();
            mNumberOfKeys = mPiano->getKeyboard().getNumberOfKeys();
            mKeyNumberOfA4 = mPiano->getKeyboard().getKeyNumberOfA4();
            updateAllWaveforms();
        }
        break;
    case Message::MSG_MIDI_EVENT:
        {
            // In case of MIDI Keyboard events:
            auto message(std::static_pointer_cast<MessageMidiEvent>(m));
            MidiAdapter::Data data = message->getData();
            switch (data.event)
            {
                case MidiAdapter::MIDI_KEY_PRESS:
                {
                    handleMidiKeypress(data); // see following function
                    break;
                }
                case MidiAdapter::MIDI_KEY_RELEASE:
                {
                    int key = data.byte1-69+mKeyNumberOfA4;
                    mSynthesizer.releaseSound(key);
                    mSynthesizer.releaseSound(key+100);
                    break;
                }
            case MidiAdapter::MIDI_CONTROL_CHANGE:
                {
                    // private setting to allow MIDI switch
                    // between the operating modes
                    // If it is not a pedal break (damper=67):
                    if (data.byte1 < 64 or data.byte1 > 67) break;
                    // If pedal pressed go to recording mode.
                    if (data.byte2 > 0)
                        MessageHandler::send<MessageModeChanged>(MODE_RECORDING);
                    else
                        MessageHandler::send<MessageModeChanged>(MODE_CALCULATION);
                }
                default:
                    break;
            }
        }
        break;
    case Message::MSG_FINAL_KEY:
        {
            // echo sound texture of the recorded key in recording mode.
            if (mOperationMode==MODE_RECORDING)
            {
                auto message(std::static_pointer_cast<MessageFinalKey>(m));
                int id = message->getKeyNumber();
                // replay only if the selected key was recognized:
                if (id == mSelectedKey)
                {
                    auto key = message->getFinalKey();
                    double frequency = key->getRecordedFrequency();
                    updateWaveform(id,0);
                    playOriginalSoundOfKey(id,frequency,0.5,5,5);   // echo sound
                }
            }
        }
        break;
    default:
        break;
    }
}


//-----------------------------------------------------------------------------
//			               Handle MIDI keypress
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Handle MIDI keypress.
///
/// This function takes action on MIDI keypress events depending on the actual
/// operation mode. In the tuning mode the MIDI keybaord plays simple sine
/// waves. In the recording mode it echos the recorded key as a confirmation
/// of success. In the calculating mode it can be used to test the computed
/// tuning curve by playing the MIDI keyboard. In the tuning mode it can be
/// be used as a reference oscillator for electromagnetic driving.
/// \param data : Data structure delivered by the MIDI event.
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::handleMidiKeypress (MidiAdapter::Data &data)
{
    int key = data.byte1-69+mKeyNumberOfA4;
    if (key<0 or key>=mNumberOfKeys) return;
    double volume = pow(static_cast<double>(data.byte2) / 256, 2);

    switch(mOperationMode)
    {
    case MODE_IDLE:
    {
        // In this mode play sine waves with respect to the selected concert pitch
        double frequency = mPiano->getEqualTempFrequency(key,0,mPiano->getConcertPitch());
        playSineWave(key,frequency,0.7*volume);
    }
    break;
    case MODE_RECORDING:
    {
        // In this mode play the original sound in the original pitch
        MessageHandler::send<MessageKeySelectionChanged>(key, &mPiano->getKey(key));
        playOriginalSoundOfKey(key,0.3*volume);
    }
    break;
    case MODE_TUNING:
    case MODE_CALCULATION:
    {
        // In these modes play the computed sound in selected concert pitch
        playOriginalSoundOfKey(key+100,0.3*volume);
    }
    break;
    default:
    break;
    }
}



//-----------------------------------------------------------------------------
//			               Stereo location of a key
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute a stereo location depending on the key number.
///
/// \param keynumber : Number of the key
/// \return : Stereo location between 0 (left) and 1 (right)
///////////////////////////////////////////////////////////////////////////////

double SoundGenerator::getStereo (int keynumber)
{
    if (keynumber >=0 and keynumber < mNumberOfKeys)
        return static_cast<double>(keynumber+1) / (mNumberOfKeys+2);
    else return 0.5;
}


//-----------------------------------------------------------------------------
//			               Play a simple sine wave
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Play a simple sine wave.
///
/// \param key : The number of the key (used as ID for the synthesizer).
/// \param frequency : Frequency of the sine wave to be played.
/// \param volume : Volume of the tone
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::playSineWave(int keynumber, double frequency, double volume)
{
    EptAssert (keynumber >=0 and keynumber < mNumberOfKeys,"range of key");
    mSynthesizer.play(keynumber,frequency,volume,getStereo(keynumber),90,5,0.7,10);
}


//-----------------------------------------------------------------------------
//			               Play a permanent sine wave
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Play a permanent sine wave.
///
/// \param id : identification tag of the sound
/// \param frequency : Frequency of the sine wave to be played.
/// \param volume : Volume of the tone
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::playReferenceTone (const Key &key, int keynumber, double frequency, double volume)
{
    auto &peaks = key.getPeaks();
    if (peaks.size()==0 or frequency==0 or volume==0) return;
    if (keynumber < mKeyNumberOfA4 and peaks.size()<4) return;
    auto it = peaks.begin();
    double factor = frequency / it->first;
    if (keynumber < mKeyNumberOfA4-24) it++;
    if (keynumber < mKeyNumberOfA4-30) it++;
    if (keynumber < mKeyNumberOfA4-36) it++;
    //double f = it->first/2*factor; // half frequency
    double f = it->first*factor;
    int id = mNumberOfKeys + keynumber;

    if(f){}; if (id){};
//    mSynthesizer.createSound(id,1,0.5,30,5,1,30);
//    mSynthesizer.addFourierComponent(id,f,0.5);
//    mSynthesizer.playSound(id);
}


//-----------------------------------------------------------------------------
//			        Play sould of a key according to spectrum
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Play a sound of a recorded key according to its recorded spectrum.
///
/// This function generates a sound that mimics the recorded sound of a given
/// key. To this end it uses the list of peaks which is stored with each key.
/// Each peak contributes with an individual sine wave.
/// \param key : Reference to the key to be played (access to list of peaks).
/// \param id : Identification tag, usually number of the key.
/// \param volume : Volume of the key.
/// \param attack : Attack rate at which the sound is initiated.
/// \param decay : Decay rate at which the sound disappears.
/// \param sustain : Sustain level.
/// \param release : Release rate.
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::playOriginalSoundOfKey (const int id, const double volume,
                                             const double attack, const double decay,
                                             const double sustain, const double release)
{
    mSynthesizer.play(id,0,volume, getStereo(id), attack, decay, sustain, release);
}




void SoundGenerator::updateWaveform (const int keynumber, const double waitingtime)
{
    auto getRuntime = [] (double keynumber) { return 5.0 * pow(2.0,-keynumber/12.0); };
    const Key &key = mPiano->getKey(keynumber);
    mSynthesizer.registerSound(keynumber,    key.getRecordedFrequency(),key.getPeaks(),
                               getStereo(keynumber), getRuntime(keynumber), waitingtime);
    mSynthesizer.registerSound(keynumber+100,
                               key.getComputedFrequency()/440.0*mPiano->getConcertPitch(),
                               key.getPeaks(),
                               getStereo(keynumber), getRuntime(keynumber), waitingtime);
}



void SoundGenerator::updateAllWaveforms(const double waitingtime)
{
    for (int i=0; i<mNumberOfKeys; i++) updateWaveform(i,waitingtime);
}
