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

#include "guidescreenplay.h"
#include "guidemessagebox.h"
#include "mainwindow.h"
#include "settingsforqt.h"
#include "../core/messages/messagemodechanged.h"

GuideScreenplay::GuideScreenplay(MainWindow *mainWindow)
    : mMainWindow(mainWindow),
      mNextExpectedGuideBoxType(STATUS_WAITING_FOR_INIT) {


}

GuideScreenplay::~GuideScreenplay()
{

}

void GuideScreenplay::start()
{
    mNextExpectedGuideBoxType = STATUS_IDLE;  // application started, guide in default mode
    if (SettingsForQt::getSingleton().getApplicationRuns() == 1) {
        // first run
        if (GuideMessageBox::showMessageBox(GuideMessageBox::MB_WELCOME, mMainWindow, true) == QMessageBox::No) {
            // disable the guide
            SettingsForQt::getSingleton().enableGuide(false);
            mMainWindow->setGuideActionEnabled(false);
        }
        // always show help message box
        GuideMessageBox::showMessageBox(GuideMessageBox::MB_WHERE_TO_FIND_HELP, mMainWindow, true);
    }
    else
    {
        // not the first run
        if (SettingsForQt::getSingleton().isGuideEnabled()) {
            // show welcome back message if guide is enbled
            if (GuideMessageBox::showMessageBox(GuideMessageBox::MB_WELCOME_BACK, mMainWindow, true) == QMessageBox::No) {
                // disable the guide
                SettingsForQt::getSingleton().enableGuide(false);
                mMainWindow->setGuideActionEnabled(false);
            }
            // always show help message box
            GuideMessageBox::showMessageBox(GuideMessageBox::MB_WHERE_TO_FIND_HELP, mMainWindow, true);
        }
    }

    if (SettingsForQt::getSingleton().isGuideEnabled()) {
        guideActivated();
    }
}

void GuideScreenplay::guideActivated() {
    if (SettingsForQt::getSingleton().isGuideEnabled() == false) {
        return;
    }

    if (mNextExpectedGuideBoxType == STATUS_IDLE) {
        // we are at the very beginning
        if (GuideMessageBox::showMessageBox(GuideMessageBox::MB_EDIT_PIANO_INFORMATION, mMainWindow) == QMessageBox::Yes) {
            mMainWindow->onEditPianoDataSheet();
            GuideMessageBox::showMessageBox(GuideMessageBox::MB_REEDIT_PIANO_INFORMATION, mMainWindow);
        }

        GuideMessageBox::showMessageBox(GuideMessageBox::MB_SWITCH_TO_RECORD_MODE, mMainWindow);
        mNextExpectedGuideBoxType = GuideMessageBox::MB_START_RECORD_MODE;
    }
}

void GuideScreenplay::handleMessage(MessagePtr m) {
    switch (m->getType()) {
    case Message::MSG_MODE_CHANGED: {
        auto mmc(std::static_pointer_cast<MessageModeChanged>(m));
        switch (mmc->getMode()) {
        case MODE_RECORDING:
            if (mNextExpectedGuideBoxType == GuideMessageBox::MB_START_RECORD_MODE) {
                mNextExpectedGuideBoxType = STATUS_IDLE;
            }
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}
