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
//                           Recording manager
//=============================================================================

#include "recordingmanager.h"

#include "../../messages/messageprojectfile.h"
#include "../../messages/messagekeyselectionchanged.h"
#include "../../messages/messagemodechanged.h"

RecordingManager::RecordingManager  (AudioRecorderAdapter *audioRecorder)
 : mAudioRecorder (audioRecorder),
   mPiano(nullptr),
   mOperationMode(MODE_IDLE),
   mSelectedKey(nullptr),
   mNumberOfKeys(88),
   mNumberOfSelectedKey(-1)
{
}


//-----------------------------------------------------------------------------
//               Message handler  (for standby and stroboscope)
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Manage standby mode
/// \param m Message from the message handler
///////////////////////////////////////////////////////////////////////////////

void RecordingManager::handleMessage(MessagePtr m)
{
    switch (m->getType())
    {
        case Message::MSG_PROJECT_FILE:
        {
            auto mpf(std::static_pointer_cast<MessageProjectFile>(m));
            mPiano = &mpf->getPiano();
            mNumberOfKeys = mPiano->getKeyboard().getNumberOfKeys();
            updateStroboscopicFrequencies();
            break;
        }
        case Message::MSG_SIGNAL_ANALYSIS_ENDED:
        {
            mAudioRecorder->setWaitingFlag (false);
            break;
        }
        case Message::MSG_MODE_CHANGED:
        {
            auto mmc(std::static_pointer_cast<MessageModeChanged>(m));
            mOperationMode = mmc->getMode();
            switch (mOperationMode)
            {
                case MODE_TUNING:
                    mAudioRecorder->getStroboscope()->start();
                    mAudioRecorder->setStandby(false);
                break;
                case MODE_RECORDING:
                    mAudioRecorder->getStroboscope()->stop();
                    mAudioRecorder->setStandby(false);
                break;
                default:
                    mAudioRecorder->getStroboscope()->stop();
                    mAudioRecorder->setStandby(true);
                break;
            }
            break;
        }
        case Message::MSG_KEY_SELECTION_CHANGED:
        {
            auto message(std::static_pointer_cast<MessageKeySelectionChanged>(m));
            mSelectedKey = message->getKey();
            mNumberOfSelectedKey = message->getKeyNumber();
            updateStroboscopicFrequencies();
            break;
        }
        case Message::MSG_RECORDING_STARTED:
        {
            if (mOperationMode == MODE_TUNING) mAudioRecorder->getStroboscope()->setFramesPerSecond(FPS_FAST);
            break;
        }
        case Message::MSG_RECORDING_ENDED:
        {
            if (mOperationMode == MODE_TUNING) mAudioRecorder->getStroboscope()->setFramesPerSecond(FPS_SLOW);
            break;
        }
        default:
            break;
    }
}


void RecordingManager::updateStroboscopicFrequencies()
{
    if (mSelectedKey)
    {
        std::vector<double> ftab;
        const double fc = mSelectedKey->getComputedFrequency();
        if (fc > 0)
        {
            const Key::PeakListType peaks = mSelectedKey->getPeaks();
            if (peaks.size()>0)
            {
                const double f1 = peaks.begin()->first;
                int N = 0;

                if (f1>0) for (auto &e : peaks)
                {
                    if (++N > 1 + (mNumberOfKeys-mNumberOfSelectedKey)/8) break;
                    ftab.push_back(e.first/f1*mPiano->getConcertPitch()/440.0*fc);
                }
            }
            else // if no peaks are available
            {
                const double B = mPiano->getExpectedInharmonicity (fc);
                for (int n=1; n<=3; ++n) ftab.push_back(n*fc*sqrt((1+B*n*n)/1+B));
            }
        }
        mAudioRecorder->getStroboscope()->setFrequencies(ftab);
    }
}
