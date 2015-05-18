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

#ifndef GUIDEMESSAGEBOX_H
#define GUIDEMESSAGEBOX_H

#include <QMessageBox>

class GuideMessageBox : QMessageBox
{
    Q_OBJECT
public:
    enum Types {
        MB_WELCOME,
        MB_WELCOME_BACK,
        MB_WHERE_TO_FIND_HELP,
        MB_EDIT_PIANO_INFORMATION,
        MB_REEDIT_PIANO_INFORMATION,
        MB_SWITCH_TO_RECORD_MODE,
        MB_START_RECORD_MODE,
    };

private:
    /// \brief private constructor
    /// use the static showMessageBox method to create a message box
    GuideMessageBox(Types type, QWidget *parent);

public:
    ~GuideMessageBox();


    static int showMessageBox(Types type, QWidget *parent, bool force = false);
};

#endif // GUIDEMESSAGEBOX_H
