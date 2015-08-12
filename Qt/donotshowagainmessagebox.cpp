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

#include "donotshowagainmessagebox.h"
#include <QCheckBox>
#include "settingsforqt.h"
#include "../core/system/eptexception.h"

DoNotShowAgainMessageBox::DoNotShowAgainMessageBox(Type type, const QString &text, QWidget *parent) :
    QMessageBox(parent),
    mType(type)
{
    setCheckBox(new QCheckBox(tr("Do not show again.")));

    // generate title and buttons
    switch (type) {
    case RESET_PITCHES:
        setWindowTitle(tr("Reset recording"));
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
    default:
        setWindowTitle(tr("Question"));
        setStandardButtons(QMessageBox::Ok);
        break;
    }

    if (text.isEmpty()) {
        switch (type) {
        case RESET_PITCHES:
            setText(tr("Do you really want to reset all recorded keys? This can not be made undone!"));
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
    if (doNotShowAgain(type)) {
        // do not show again, accept
        return QMessageBox::Accepted;
    }

    DoNotShowAgainMessageBox b(type, text, parent);

    int r(b.exec());
    if (r == QMessageBox::Rejected || r == QMessageBox::Cancel) {
        return r;
    }
    if (b.checkBox()->isChecked()) {
        // store state
        SettingsForQt::getSingleton().setDoNotShowAgainMessageBox(type, true);
    }
    return QMessageBox::Accepted;
}

bool DoNotShowAgainMessageBox::doNotShowAgain(Type type) {
    return SettingsForQt::getSingleton().doNotShowAgainMessageBox(type);
}
