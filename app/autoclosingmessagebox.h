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

#ifndef AUTOCLOSINGMESSAGEBOX_H
#define AUTOCLOSINGMESSAGEBOX_H

#include <QMessageBox>

#include "prerequisites.h"

#include "core/messages/messagelistener.h"


///////////////////////////////////////////////////////////////////////////////
/// \brief a dialog that will automatically close on a event (message).
///
/// Not that you have to create this message box with new.
/// It will delete itself on closing.
///////////////////////////////////////////////////////////////////////////////
class AutoClosingMessageBox : public QMessageBox, public MessageListener
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reasons for the dialog to close automatically.
    ///////////////////////////////////////////////////////////////////////////////
    enum CloseReason {
        MSG_RECORDING_STARTED,              ///< MSG_RECORDING_STARTED was received
    };

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor.
    /// \param parent : The parent widget
    /// \param closeReason : A reason to close the dialog automatically
    /// \param icon : An default icon to be displayed in the message box
    /// \param title : The title that shall be displayed
    /// \param text : An informative text to be displayed
    /// \param buttons : A standard button to be added
    ///////////////////////////////////////////////////////////////////////////////
    AutoClosingMessageBox(QWidget *parent,
                          CloseReason closeReason,
                          Icon icon,
                          const QString &title,
                          const QString &text,
                          StandardButton buttons = StandardButton::Ok);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///////////////////////////////////////////////////////////////////////////////
    ~AutoClosingMessageBox();

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Message handling.
    /// \param m : The message to handle
    ///////////////////////////////////////////////////////////////////////////////
    virtual void handleMessage(MessagePtr m) override final;


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplementation of the close event.
    ///
    /// The function will delete this.
    /// \param e : The event
    ///////////////////////////////////////////////////////////////////////////////
    virtual void closeEvent(QCloseEvent *e) override final;

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Executing of the dialog is forbidden.
    ///
    /// Do not call this function. The dialog has to be created with new to be
    /// global. It will delete itself upon closing.
    /// \return -1
    ///////////////////////////////////////////////////////////////////////////////
    virtual int exec() override final;

private:
    /// Reason to close the dialog automatically.
    const CloseReason mCloseReason;
};

#endif // AUTOCLOSINGMESSAGEBOX_H
