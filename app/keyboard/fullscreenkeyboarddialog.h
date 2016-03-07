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

#ifndef FULLSCREENKEYBOARDDIALOG_H
#define FULLSCREENKEYBOARDDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>

#include "prerequisites.h"

class KeyboardGraphicsView;

///////////////////////////////////////////////////////////////////////////////
/// \brief Dialog that displays a KeyboardGraphicsView in fullscreen.
///
/// On small devices it is impossible to select a key on the standard
/// KeyboardGraphicsView at the bottom. Instead a press on the
/// KeyboardGraphicsView will open this dialog and allow to select a key in
/// fullscreen mode.
///////////////////////////////////////////////////////////////////////////////
class FullScreenKeyboardDialog : public QDialog
{
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor.
    /// \param The KeyboardGraphicsView that opened this dialog.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    FullScreenKeyboardDialog(KeyboardGraphicsView *parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///////////////////////////////////////////////////////////////////////////////
    ~FullScreenKeyboardDialog();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for the displayed KeyboardGraphicsView.
    /// \return mKeyboardView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    KeyboardGraphicsView *getKeyboardView() {return mKeyboardView;}

private slots:
    void reject() override final;
    void accept() override final;

private:
    /// Pointer to the displayed KeyboardGraphicsView in fullscreen.
    KeyboardGraphicsView *mKeyboardView;

    KeyboardGraphicsView *mParentKeyboardView;

    /// button box
    QDialogButtonBox *mDialogButtons;
};

#endif // FULLSCREENKEYBOARDDIALOG_H
