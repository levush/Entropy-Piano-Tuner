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

#include "guidemessagebox.h"
#include "settingsforqt.h"

GuideMessageBox::GuideMessageBox(Types type, QWidget *parent)
    : QMessageBox(parent) {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setModal(true);


    setIconPixmap(QPixmap(":/media/images/mode_idle.png"));

    switch (type) {
    case MB_WELCOME:
        setText(tr("Welcome to the entropy piano tuner"));
        setInformativeText(tr("This guide will instruct you though the program to help you tuning your piano. Do you want to enable this guide?"));
        setStandardButtons(Yes | No);
        setDefaultButton(Yes);
        break;
    case MB_WELCOME_BACK:
        setText(tr("Welcome back to the entropy piano tuner"));
        setInformativeText(tr("The guide is currently activated. Do you want to keep it enabled?"));
        setStandardButtons(Yes | No);
        setDefaultButton(Yes);
        break;
    case MB_WHERE_TO_FIND_HELP:
        setText(tr("If you are stuck, you can always get further help from the help menu."));
        setInformativeText(tr("Press Ok to start tuning your piano."));
        setStandardButtons(Ok);
        break;
    case MB_EDIT_PIANO_INFORMATION:
        setText(tr("Before the actual tuning you should provide some information about your piano in order to easier recover your work."));
        setInformativeText(tr("Do you want to open an dialog to edit the piano data sheet now?"));
        setStandardButtons(Yes | No);
        setDefaultButton(Yes);
        break;
    case MB_REEDIT_PIANO_INFORMATION:
        setText(tr("If you want to make changes in the piano data sheet at a later point in time you can call this dialog by the menu."));
        setStandardButtons(Ok);
        break;
    case MB_SWITCH_TO_RECORD_MODE:
        setText(tr("Now it is time to record all of your piano keys' tones."));
        setInformativeText(tr("Switch to the recording mode by pressing the corresponding button in the left tool bar."));
        setStandardButtons(Ok);
        break;
    case MB_START_RECORD_MODE:
        setText(tr("Before recording any sound you have to calibrate initially your input device."));
        setInformativeText(tr("Do you want to open the automatic calibration?"));
        setStandardButtons(Yes | No);
        setDefaultButton(Yes);
        break;
    }
}

GuideMessageBox::~GuideMessageBox()
{

}


int GuideMessageBox::showMessageBox(Types type, QWidget *parent, bool force) {
    // only show message boxes when guide is enabled in settings
    if (SettingsForQt::getSingleton().isGuideEnabled() || force) {
        GuideMessageBox mb(type, parent);
        return mb.exec();
    } else {
        return -1;
    }
}
