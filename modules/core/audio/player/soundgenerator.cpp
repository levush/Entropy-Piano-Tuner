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
//                 Sound generator driving the synthesizer
//=============================================================================

#include "soundgenerator.h"

#include <algorithm>

#include "../../system/log.h"
#include "../../system/eptexception.h"
#include "../../system/simplethreadhandler.h"
#include "../../messages/messagechangetuningcurve.h"
#include "../../messages/messagekeyselectionchanged.h"
#include "../../messages/messagerecorderenergychanged.h"
#include "../../messages/messagepreliminarykey.h"
#include "../../messages/messagehandler.h"
#include "../../messages/messagemidievent.h"
#include "../../messages/messagemodechanged.h"
#include "../../messages/messageprojectfile.h"
#include "../../messages/messagefinalkey.h"
#include "../../piano/piano.h"
#include "../../piano/key.h"
#include "../../math/mathtools.h"
#include "../../settings.h"

//-----------------------------------------------------------------------------
//			                    Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, initializes the member variables.
/// \param audioadapter : Pointer to the audio output implementation
///////////////////////////////////////////////////////////////////////////////

SoundGenerator::SoundGenerator (AudioInterface *audioInterface) :
    mPiano(nullptr),
    mOperationMode(OperationMode::MODE_IDLE),
    mNumberOfKeys(0),
    mKeyNumberOfA4(0),
    mSelectedKey(-1),
    mResonatingKey(-1),
    mResonatingVolume(0)
{
    audioInterface->setDevice(&mSynthesizer);
}

//-----------------------------------------------------------------------------
//	  init function
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Init function
///
/// This will simply start the precalculation of all keys
///////////////////////////////////////////////////////////////////////////////
///
void SoundGenerator::init()
{
    preCalculateSoundOfAllKeys();
}


//-----------------------------------------------------------------------------
//	  Message listener, handling all messages related to sound generation
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
    // REFERENCE SOUND IN TUNING MODE
    // When the key selection changes, the reference sound is stopped.
    // In the tuning mode a new reference sound is started.
    // This is important when the selection changes during tuning.
    case Message::MSG_KEY_SELECTION_CHANGED:
        {
            // keep track of selected key
            auto message(std::static_pointer_cast<MessageKeySelectionChanged>(m));
            mSelectedKey = message->getKeyNumber();
            stopResonatingReferenceSound();
            if (mOperationMode == MODE_TUNING)
                playResonatingReferenceSound(mSelectedKey);
        }
        break;
    // START REFERENCE SOUND IN TUNING MODE AT THE BEGINNING OF RECORDING
    case Message::MSG_RECORDING_STARTED:
        {
            if (mOperationMode==MODE_TUNING)
            {
                if (mSelectedKey>=0) playResonatingReferenceSound(mSelectedKey);
                else stopResonatingReferenceSound();
            }
        }
        break;
    // CHANGE REFERENCE SOUND IN TUNING MODE WHEN KEY SELECTION CHANGES
    case Message::MSG_PRELIMINARY_KEY:
        {
            if (mOperationMode==MODE_TUNING and mSelectedKey<0)
            {
                auto message(std::static_pointer_cast<MessagePreliminaryKey>(m));
                int recognizedkey = message->getKeyNumber();
                if (mResonatingKey != recognizedkey and recognizedkey >= 0)
                    playResonatingReferenceSound(recognizedkey);
            }
        }
        break;
    // STOP REFERENCE SOUND AT THE END OF RECORDING
    case Message::MSG_RECORDING_ENDED:
        {
            // if reference tone with constant volume stop it here.
            if (Settings::getSingleton().getSoundGeneratorMode() == SGM_REFERENCE_TONE
                    or mOperationMode!=MODE_TUNING)
            stopResonatingReferenceSound();
        }
        break;
    // REFERENCE SOUND VOLUME ADJUSTMENT IN TUNING MODE
    case Message::MSG_RECORDER_ENERGY_CHANGED:
        {
            if (mOperationMode==MODE_TUNING and mResonatingKey>=0)
            {
                auto message(std::static_pointer_cast<MessageRecorderEnergyChanged>(m));
                if (message->getLevelType()==MessageRecorderEnergyChanged::LevelType::LEVEL_INPUT)
                    if (Settings::getSingleton().isSoundGeneratorVolumeDynamic())
                        changeVolumeOfResonatingReferenceSound(message->getLevel());
            }
        }
        break;
    // MODE CHANGES STOP THE REFERENCE SOUND AND TRIGGER RECALCULATION
    case Message::MSG_MODE_CHANGED:
        {
            // Keep track of mode changes
            auto message(std::static_pointer_cast<MessageModeChanged>(m));
            mOperationMode = message->getMode();
            stopResonatingReferenceSound();
        }
        break;
    // IF A NEW FILE IS OPENED OR IF PIANO SETTINGS ARE CHANGED
    // ALL SOUNDS HAVE TO BE CALCULATED ONCE AGAIN IN THE CORRESPONDING MODE
    case Message::MSG_PROJECT_FILE:
        {
            // Get a pointer to the piano and various of its properties.
            auto mpf(std::static_pointer_cast<MessageProjectFile>(m));
            mPiano = &(mpf->getPiano());
            mNumberOfKeys = mPiano->getKeyboard().getNumberOfKeys();
            mSynthesizer.setNumberOfKeys(mNumberOfKeys);
            mKeyNumberOfA4 = mPiano->getKeyboard().getKeyNumberOfA4();
            stopResonatingReferenceSound();
            preCalculateSoundOfAllKeys();
        }
        break;
    // HANDLE MIDI KEYPRESSES AND RELATED EVENTS
    case Message::MSG_MIDI_EVENT:
        {
            auto message(std::static_pointer_cast<MessageMidiEvent>(m));
            MidiAdapter::Data data = message->getData();
            switch (data.event)
            {
                case MidiAdapter::MIDI_KEY_PRESS:
                {
                    if (data.byte2 == 0) {
                        // 0 volume will be handled as release
                        int key = data.byte1-69+mKeyNumberOfA4;
                        mSynthesizer.releaseSound(key);
                    } else {
                        handleMidiKeypress(data); // see following function
                    }
                    break;
                }
                case MidiAdapter::MIDI_KEY_RELEASE:
                {
                    int key = data.byte1-69+mKeyNumberOfA4;
                    mSynthesizer.releaseSound(key);
                    break;
                }
            case MidiAdapter::MIDI_CONTROL_CHANGE:
                {
//                    // Funny feature that allows you to switch
//                    // between the operating modes by MIDI pedal
//                    // If it is not a pedal break (damper=67):
//                    if (data.byte1 < 64 or data.byte1 > 67) break;
//                    // If pedal pressed go to recording mode.
//                    if (data.byte2 > 0)
//                        MessageHandler::send<MessageModeChanged>(MODE_RECORDING);
//                    else // else go back to calculation mode
//                        MessageHandler::send<MessageModeChanged>(MODE_CALCULATION);
                }
                default:
                    break;
            }
        }
        break;
    // RECALCULATE AND PLAY ECHO SOUND IN THE RECORDING MODE
    case Message::MSG_FINAL_KEY:
        {
            // echo synthesized sound of the recorded key in recording mode.
            if (mOperationMode==MODE_RECORDING)
            {
                auto message(std::static_pointer_cast<MessageFinalKey>(m));
                int keynumber = message->getKeyNumber();
                auto spectrum = message->getFinalKey()->getPeaks();
                // replay only if the selected key was recognized:
                if (keynumber == mSelectedKey and spectrum.size() > 0)
                {
                    preCalculateSoundOfKey (keynumber,spectrum);
                    mSynthesizer.playSound(keynumber,1,0.2,Envelope(5,5,0,30,false),true,false);
                }
            }
        }
        break;
    // RECALCULTE WAVEFORM DURING CALCULATION MODE WHEN FREUQUENCY CHANGES
    case Message::MSG_CHANGE_TUNING_CURVE:
        {
            if (mOperationMode==MODE_CALCULATION)
            {
                auto message(std::static_pointer_cast<MessageChangeTuningCurve>(m));
                preCalculateSoundOfKey(message->getKeyNumber());
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
/// operation mode. In the tuning mode the MIDI keybaord plays simple ET sine
/// waves. In the recording mode it echos the recorded key as a confirmation
/// of successful recording. In the calculating mode it can be used to test the
/// computed tuning curve by playing the MIDI keyboard.
/// \param data : Data structure delivered by the MIDI event.
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::handleMidiKeypress (MidiAdapter::Data &data)
{
    int key = data.byte1-69+mKeyNumberOfA4; // extract key number starting with 0
    if (key<0 or key>=mNumberOfKeys) return;
    double volume = pow(static_cast<double>(data.byte2) / 128, 2); // keystroke volume
    // The following formula mimics the decay time of a piano string
    const double decay = (key <= 12 ? 1.0/6 : 1.0/210*pow(key,1.43));
    const double release = (key - mKeyNumberOfA4 >= 22 ? decay : 30);
    Envelope envelope (40,decay,0,release,true);

    switch(mOperationMode)
    {
    case MODE_IDLE:
    {
        // In this mode play sine waves with respect to the selected concert pitch
        double frequency = mPiano->getEqualTempFrequency(key,0,mPiano->getConcertPitch());
        mSynthesizer.playSound(key,frequency,volume,Envelope(40,5,0.6,10));
    }
    break;
    case MODE_RECORDING:
    {
        // In this mode select key and play the original sound in the original pitch
        MessageHandler::send<MessageKeySelectionChanged>(key, &(mPiano->getKey(key)));
        mSynthesizer.playSound(key,1,0.1*volume,envelope);
    }
    break;
    case MODE_CALCULATION:
    {
        // In this mode select key and play the original sound in the original pitch
        MessageHandler::send<MessageKeySelectionChanged>(key, &(mPiano->getKey(key)));
        double recorded = mPiano->getKey(key).getRecordedFrequency();
        if (recorded > 0)
        {
            double frequencyshift = mPiano->getKey(key).getComputedFrequency() *
                                    mPiano->getConcertPitch()  / 440.0 / recorded;
            mSynthesizer.playSound(key,frequencyshift,0.1*volume,envelope,true);
        }
    }
    break;
    case MODE_TUNING:
    {
        // In this mode play the computed sound in selected concert pitch
        double recorded = mPiano->getKey(key).getRecordedFrequency();
        if (recorded > 0)
        {
            double frequencyshift = mPiano->getKey(key).getComputedFrequency() *
                                    mPiano->getConcertPitch()  / 440.0 / recorded;
            mSynthesizer.playSound(key,frequencyshift,0.1*volume,envelope);
        }
    }
    break;
    default:
    break;
    }
}


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
    if (mSynthesizer.isPlaying(keynumber)) return;
    auto &key = mPiano->getKey(keynumber);
    double frequency = key.getComputedFrequency()*mPiano->getConcertPitch()/440.0;
    if (frequency>0)
    {
        mResonatingKey=keynumber;
        mResonatingVolume = 1;
        switch (Settings::getSingleton().getSoundGeneratorMode())
        {
        case SGM_REFERENCE_TONE:
        {
            playResonatingSineWave(keynumber,frequency, 0.5);
        }
        break;
        case SGM_SYNTHESIZE_KEY:
        {
            const double volume = 0.2;
            Envelope env(30,50,mResonatingVolume,10,false);
            double frequencyshift = frequency /
                    mPiano->getKey(keynumber).getRecordedFrequency();
            mSynthesizer.playSound(keynumber,frequencyshift,volume,env,false,false);
        }
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
    if (mResonatingKey>=0)
    {
        mSynthesizer.releaseSound(mResonatingKey);
        mResonatingKey = -1;
        mResonatingVolume = 0;
    }
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
    if (mResonatingKey < 0 or mResonatingKey >= mNumberOfKeys) return;
    if (not mSynthesizer.isPlaying(mResonatingKey)) { mResonatingKey=-1; return; }
    double truncatedlevel = std::min(0.8,level);
    double volume = pow(truncatedlevel,2.0);
    if (volume > mResonatingVolume) mResonatingVolume = volume;
    else mResonatingVolume *= 0.87;
    mSynthesizer.ModifySustainLevel(mResonatingKey,mResonatingVolume);
}


//-----------------------------------------------------------------------------
//   Play a permanent sine wave as a resonating background sound for tuning
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Play a permanent sine wave as a resonating background sound for tuning.
/// \param keynumber : Number of the key
/// \param frequency : Frequency of the sine wave to be played.
/// \param volume : Volume of the tone
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::playResonatingSineWave (int keynumber, double frequency, double volume)
{

    auto &key = mPiano->getKey(keynumber);
    auto &peaks = key.getPeaks();
    if (peaks.size()==0 or frequency==0 or volume==0) return;
    if (keynumber < mKeyNumberOfA4 and peaks.size()<4) return;
    auto it = peaks.begin();
    double factor = frequency / it->first;
    if (keynumber < mKeyNumberOfA4-24) it++;
    if (keynumber < mKeyNumberOfA4-30) it++;
    if (keynumber < mKeyNumberOfA4-36) it++;
    double frequ = it->first*factor;
    mSynthesizer.playSound(keynumber,frequ,0.5,Envelope(30,5,1,30),false,false);
}


//-----------------------------------------------------------------------------
//			     Calculate the sound of a given key in advance
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Calculate the sound of a given key in advance, using the spectrum
/// of the given key.
/// \param keynumber : Number of the key
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::preCalculateSoundOfKey (const int keynumber)
{
    mSynthesizer.preCalculateWaveform(keynumber, mPiano->getKey(keynumber).getPeaks());
}


//-----------------------------------------------------------------------------
//			     Calculate the sound of a given key in advance
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Calculate the sound of a given key in advance, using an explicit
/// spectrum passed as a parameter.
/// \param keynumber : Number of the key
/// \param spectrum : The explicit spectrum
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::preCalculateSoundOfKey (const int keynumber, Synthesizer::Spectrum &spectrum)
{
    mSynthesizer.preCalculateWaveform(keynumber,spectrum);
}


//-----------------------------------------------------------------------------
//			     Calculate the sound of all keys in advance
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Calculate the sound of all keys in advance
///////////////////////////////////////////////////////////////////////////////

void SoundGenerator::preCalculateSoundOfAllKeys ()
{
    for (int keynumber = 0; keynumber < mNumberOfKeys; keynumber++)
        preCalculateSoundOfKey(keynumber);
}
