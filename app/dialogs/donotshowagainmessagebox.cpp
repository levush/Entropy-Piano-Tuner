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

#include "donotshowagainmessagebox.h"

#include <QCheckBox>

#include "core/system/eptexception.h"

#include "implementations/settingsforqt.h"

DoNotShowAgainMessageBox::DoNotShowAgainMessageBox(Type type, const QString &text, QWidget *parent) :
    QMessageBox(parent),
    mType(type)
{
    setCheckBox(new QCheckBox(tr("Do not show again.")));

    // generate title and buttons
    switch (type) {
    case RESET_PITCHES:
        setWindowTitle(tr("Clear pitch markers"));
        setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        break;
    case NOT_ALL_KEYS_RECORDED:
        setWindowTitle(tr("Not all keys recorded"));
        setStandardButtons(QMessageBox::Ok);
        break;
    case TUNING_CURVE_NOT_CALCULATED:
        setWindowTitle(tr("Tuning curve not calculated"));
        setStandardButtons(QMessageBox::Ok);
        break;
    case TUNING_CURVE_NEEDS_UPDATE:
        setWindowTitle(tr("Tuning curve must be recalculated"));
        setStandardButtons(QMessageBox::Ok);
        break;
    case MODE_CHANGE_SAVE:
        setWindowTitle(tr("Save changes"));
        setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        setIcon(QMessageBox::Question);
        break;
    case FORCE_RECORDING_INFORMATION:
        setWindowTitle(tr("Information"));
        setStandardButtons(QMessageBox::Ok);
        setIcon(QMessageBox::Information);
        break;
    default:
        setWindowTitle(tr("Question"));
        setStandardButtons(QMessageBox::Ok);
        setIcon(QMessageBox::Question);
        break;
    }

    if (text.isEmpty()) {
        switch (type) {
        case RESET_PITCHES:
            setText(tr("Do you really want to clear all pitch markers? This can not be undone!"));
            break;
        case NOT_ALL_KEYS_RECORDED:
            setText(tr("Not all keys have been recorded. Switch the mode and record them."));
            break;
        case TUNING_CURVE_NOT_CALCULATED:
            setText(tr("The tuning curve has not been calculated. Switch the mode and calculate it."));
             break;
        case TUNING_CURVE_NEEDS_UPDATE:
            setText(tr("There are missing frequencies in the calculated tuning curve. Recalculate to fix this."));
            break;
        case MODE_CHANGE_SAVE:
            setText(tr("Do you want to save your current changes? You can save at any time using the tool button or the action from the menu."));
            break;
        case FORCE_RECORDING_INFORMATION:
            setText(tr("If the recording of a key fails you can force its recording by clicking the desired key twice. A forced key will be highlighted as a dark red color."));
            break;
        default:
            EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "DoNotShowAgainMessageBoxTypeNotImplemented");
        }
    } else {
        setText(text);
    }
}

DoNotShowAgainMessageBox::~DoNotShowAgainMessageBox()
{

}


int DoNotShowAgainMessageBox::show(Type type, const QString text, QWidget *parent) {
    int storedResult = doNotShowAgainAction(type);
    if (storedResult >= 0) {
        // do not show again, accept
        return storedResult;
    }

    DoNotShowAgainMessageBox b(type, text, parent);

    int r(b.exec());

    // store value, but deny if the user canceled the dialog
    bool denyStoring = r == QMessageBox::Rejected || r == QMessageBox::Cancel;
    if (b.checkBox()->isChecked() && !denyStoring) {
        // store state
        SettingsForQt::getSingleton().setDoNotShowAgainMessageBox(type, true, r);
    }

    return r;
}

int DoNotShowAgainMessageBox::doNotShowAgainAction(Type type) {
    return SettingsForQt::getSingleton().doNotShowAgainMessageBox(type);
}

bool DoNotShowAgainMessageBox::doNotShowAgain(Type type) {
    return SettingsForQt::getSingleton().doNotShowAgainMessageBox(type) >= 0;
}
