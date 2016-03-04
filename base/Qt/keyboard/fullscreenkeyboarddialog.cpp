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

#include "fullscreenkeyboarddialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "core/system/eptexception.h"
#include "keyboardgraphicsview.h"

FullScreenKeyboardDialog::FullScreenKeyboardDialog(KeyboardGraphicsView *parent) :
    QDialog(parent),
    mParentKeyboardView(parent) {

    setWindowTitle(tr("Keyboard"));
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    // create a new KeyboardGraphicsView in fullscreen mode
    KeyboardGraphicsView *keyboardView = new KeyboardGraphicsView(this);
    keyboardView->setKeyboard(parent->getKeyboard());
    layout->addWidget(keyboardView);

    // show the dialog in fullscreen
    showFullScreen();

    // select the current key
    keyboardView->setSelection(parent->getSelectedKeyIndex(), parent->getSelectedKeyState(), parent->getPreliminaryKey());

    QDialogButtonBox *buttons = new QDialogButtonBox;
    mDialogButtons = buttons;
    layout->addWidget(buttons);
    buttons->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    QObject::connect(keyboardView, SIGNAL(selectionChanged(int8_t)), this, SLOT(accept()));
    QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    mKeyboardView = keyboardView;
}

FullScreenKeyboardDialog::~FullScreenKeyboardDialog()
{

}

void FullScreenKeyboardDialog::reject() {
    QDialog::reject();
}

void FullScreenKeyboardDialog::accept() {
    mParentKeyboardView->selectKey(mKeyboardView->getSelectedKeyIndex(),
                                   mKeyboardView->getSelectedKeyState(),
                                   true);

    QDialog::accept();
}
