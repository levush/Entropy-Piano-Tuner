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

#ifndef DONOTSHOWAGAINMESSAGEBOX_H
#define DONOTSHOWAGAINMESSAGEBOX_H

#include <QMessageBox>

#include "prerequisites.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief A MessageBox with a QCheckBox to remember the decision of the user.
///
/// It will read and write its status to the SettingsForQt and wont show if
/// the user checked the QCheckBox.
/// The checked results can be resetted in the options::PageEnvironmentGeneral.
///////////////////////////////////////////////////////////////////////////////
class DoNotShowAgainMessageBox : public QMessageBox
{
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief The type and unique id for the message box
    ///////////////////////////////////////////////////////////////////////////////
    enum Type {
        RESET_PITCHES = 0,              ///< Reset pitch markers
        NOT_ALL_KEYS_RECORDED,          ///< Not all keys have been recorded
        TUNING_CURVE_NOT_CALCULATED,    ///< The tuning curve has not been calculated
        TUNING_CURVE_NEEDS_UPDATE,      ///< The tuning curve has to be recalculated
        MODE_CHANGE_SAVE,               ///< Switching the mode from recording and there are changes
        FORCE_RECORDING_INFORMATION,    ///< Information that the user can force a recording

        COUNT                           ///< Number of message boxes
    };

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Private constructor.
    /// \param type The type of the message box
    /// \param text The text to be displayed or an empty string to use the default text
    /// \param parent The parent widget
    ///
    /// Call show(Type, QWidget*) to create a message box.
    ///////////////////////////////////////////////////////////////////////////////
    DoNotShowAgainMessageBox(Type type, const QString &text, QWidget *parent);

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///////////////////////////////////////////////////////////////////////////////
    ~DoNotShowAgainMessageBox();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Show a message box if the user didnt check the QCheckBox.
    /// \param type : The type of the message box
    /// \param text : Text of the box. Leave it empty to use the default text
    /// \param parent : The parent widget
    /// \return QMessageBox::Accepted if the message was accepted
    ///
    /// The function will first check if the QCheckBox was checked in
    /// SettingsForQt and return the result.
    /// Elsewise it will show the message box.
    ///////////////////////////////////////////////////////////////////////////////
    static int show(Type type, const QString text, QWidget *parent = nullptr);
    static int show(Type type, QWidget *parent) {return show(type, QString(), parent);}

    ///
    /// \brief Action that was stored
    /// \param Dialog ID
    /// \return -1 if no action stored (show again), result of dialog elsewise
    ///
    static int doNotShowAgainAction(Type type);
    static bool doNotShowAgain(Type type);

private:
    /// The type of the message box.
    Type mType;
};

#endif // DONOTSHOWAGAINMESSAGEBOX_H
