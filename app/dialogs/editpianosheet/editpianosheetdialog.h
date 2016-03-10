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

#ifndef EDITPIANOSHEETDIALOG_H
#define EDITPIANOSHEETDIALOG_H

#include <QDialog>

#include "prerequisites.h"

class Piano;

namespace Ui {
class EditPianoSheetDialog;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Dialog to edit the piano data sheet stored in a Piano.
///
///////////////////////////////////////////////////////////////////////////////
class EditPianoSheetDialog : public QDialog
{
    Q_OBJECT

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor.
    /// \param piano : The piano
    /// \param parent : The parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit EditPianoSheetDialog(const Piano &piano, QWidget *parent = 0);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Default destructor.
    ///////////////////////////////////////////////////////////////////////////////
    ~EditPianoSheetDialog();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Applys the current values in the dialog to the given piano.
    /// \param piano The piano whose data will be set
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void applyData(Piano *piano) const;

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented keyPressEvent(QKeyEvent*)
    /// \param The QKeyEvent
    ///
    /// Pressing return does not close the dialog anymore when editing a field.
    ///////////////////////////////////////////////////////////////////////////////
    void keyPressEvent(QKeyEvent *event);

    bool eventFilter(QObject *o, QEvent *e);

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot to set the tuning time to the current time.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onSetTuningTimeToNow();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief The current number of keys changed.
    /// \param k : The new number of keys.
    ///
    /// This will adjust the maximal selectible index of the key number of A.
    ///////////////////////////////////////////////////////////////////////////////
    void onNumberOfKeysChanged(int k);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot to set the concert pitch to its default value.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onDefaultConcertPitch();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot to set the number of keys to its default value.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onDefaultNumberOfKeys();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot to set the key number of A to its default value.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onDefaultKeyNumberOfA();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot to set the keys on bass bridge to its default value.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onDefaultKeysOnBassBridge();

private:
    /// The Ui of the dialog.
    Ui::EditPianoSheetDialog *ui;
};

#endif // EDITPIANOSHEETDIALOG_H
