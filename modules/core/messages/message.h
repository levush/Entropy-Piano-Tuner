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

//======================================================================
//                      Class for a single message
//======================================================================

#ifndef MESSAGE_H
#define MESSAGE_H

#include "prerequisites.h"

/////////////////////////////////////////////////////////////////////////
/// \brief The Message class
///
/// An instance of this class represents a single message. Messages can
/// be of different type, which are listed here in the enum below.
/// For example, Message::MSG_CLEAR_RECORDING would be such a message type.
/// The constructor Message(Message::MSG_CLEAR_RECORDING) would turn this
/// into a message object. The message type of a message object cannot
/// be changed during its lifetime.
///
/// See also: MessageHandler for sending and MessageListener
/// for receiving messages.
/////////////////////////////////////////////////////////////////////////

class EPT_EXTERN Message
{
public:
    /// Available message types:
    enum MessageTypes
    {
        // Bare messages without appended data
        MSG_CLEAR_RECORDING,                    ///< called when the recording was cleared
        MSG_RECORDING_STARTED,                  ///< keystroke recognized and recording started
        MSG_RECORDING_ENDED,                    ///< recording ended
        MSG_FINAL_KEY,                          ///< final key information after recording
        MSG_OPTIONS_CHANGED,                    ///< settings in the system options changed

        // Complex messages carrying data in associated message classes:
        MSG_CALCULATION_PROGRESS,               ///< Message that progress of any kind was made by the calculator
        MSG_CHANGE_TUNING_CURVE,                ///< Message that the tuning curve has been adapted
        MSG_FINAL_KEY_RECOGNIZED,               ///< sent by KeyRecognizer if final FFT is ready
        MSG_KEY_DATA_CHANGED,                   ///< data of a key changed
        MSG_KEY_SELECTION_CHANGED,              ///< Message that a key has been selected
        MSG_MIDI_EVENT,                         ///< new event from MIDI keyboard received
        MSG_MODE_CHANGED,                       ///< Message that the operation mode has changed
        MSG_NEW_FFT_CALCULATED,                 ///< sent by SignalAnalyzer if FFT is ready
        MSG_PRELIMINARY_KEY,                    ///< number of the key recognized during recording
        MSG_PROJECT_FILE,                       ///< Message that a change was made with the current project file
        MSG_RECORDER_ENERGY_CHANGED,            ///< recording input level has changed
        MSG_STROBOSCOPE_EVENT,                  ///< stroboscope message
        MSG_TUNING_DEVIATION,                   ///< tuning deviation curve has been updated
        MSG_SIGNAL_ANALYSIS,                    ///< Analysis of the signal state changed (start, end)
    };

public:

    /// \brief Message constructor.
    ///
    /// This constructor is declared as an explicit one so that it cannot
    /// be called errorneously with integer arguments.
    /// \param type : Type of the message to be created (see enumeration above).
    explicit Message(MessageTypes type);

    /// \brief Virtual destructor without function, can be overwritten in derived classes.
    virtual ~Message();

    /// \brief Get the message type of the message object.
    /// \return Type ofthe message
    MessageTypes getType() const {return mType;}

private:
    /// \brief Local variable holding the message type.
    ///
    /// After initialization during construction this cannot be changed.
    const MessageTypes mType;
};


typedef std::shared_ptr<Message> MessagePtr;    ///< Global type of a shared message pointer
template class EPT_EXTERN std::shared_ptr<Message>;

#endif // MESSAGE_H
