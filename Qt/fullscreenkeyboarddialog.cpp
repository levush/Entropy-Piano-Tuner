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

#include "fullscreenkeyboarddialog.h"
#include <QVBoxLayout>
#include "keyboardgraphicsview.h"

FullScreenKeyboardDialog::FullScreenKeyboardDialog(KeyboardGraphicsView *parent) :
    QDialog(parent) {

    setWindowTitle(tr("Keyboard"));
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    // create a new KeyboardGraphicsView in fullscreen mode
    KeyboardGraphicsView *keyboardView = new KeyboardGraphicsView(this, KeyboardGraphicsView::MODE_FULLSCREEN);
    keyboardView->setKeyboard(parent->getKeyboard());
    layout->addWidget(keyboardView);

    // show the dialog in fullscreen
    showFullScreen();

    // select the current key
    keyboardView->setSelection(parent->getSelectedKeyIndex(), parent->getSelectedKeyState(), parent->getPreliminaryKey());

    QObject::connect(keyboardView, SIGNAL(selectionChanged(int8_t)), this, SLOT(accept()));

    mKeyboardView = keyboardView;
}

FullScreenKeyboardDialog::~FullScreenKeyboardDialog()
{

}
