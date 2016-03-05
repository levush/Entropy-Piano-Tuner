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

#include "autoclosingmessagebox.h"
#include "../core/system/eptexception.h"

AutoClosingMessageBox::AutoClosingMessageBox(QWidget *parent, CloseReason closeReason, Icon icon, const QString &title, const QString &text, StandardButton buttons) :
    QMessageBox(icon, title, text, buttons, parent),
    mCloseReason(closeReason)
{
    setModal(true);
}

AutoClosingMessageBox::~AutoClosingMessageBox()
{

}

void AutoClosingMessageBox::handleMessage(MessagePtr m) {
    switch (m->getType()) {
    case Message::MSG_RECORDING_STARTED:
        if (mCloseReason == MSG_RECORDING_STARTED) {
            accept();
        }
        break;
    default:
        break;
    }
}

void AutoClosingMessageBox::closeEvent(QCloseEvent *e) {
    QMessageBox::closeEvent(e);
    delete this;
}

int AutoClosingMessageBox::exec() {
    EptAssert(false, "This function may not be called!");
    return -1;
}
