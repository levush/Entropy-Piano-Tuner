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
   mPiano(nullptr)
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
            switch (mmc->getMode())
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
            const Key* key = message->getKey();
            if (key)
            {
                const Key::PeakListType peaks = key->getPeaks();
                if (peaks.size()>0)
                {
                    double f1 = peaks.begin()->first;
                    int N = 0;
                    std::vector<double> ftab;

                    if (f1>0) for (auto &e : peaks)
                    {
                        if (++N > 7) break;
                        ftab.push_back(e.first/f1*mPiano->getConcertPitch()/440.0*key->getComputedFrequency());
                    }
                    mAudioRecorder->getStroboscope()->setFrequencies(ftab);
                }
            }
            break;
        }
        default:
            break;
    }
}
