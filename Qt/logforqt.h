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

#ifndef LOGFORQT_H
#define LOGFORQT_H

#include "../core/system/log.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Implementation for the log in Qt.
///
/// The implementation will use qDebug, qWarning and qCritical for message
/// output.
///////////////////////////////////////////////////////////////////////////////
class LogForQt : public Log
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Empty default constructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    LogForQt();


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Empty defaul destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~LogForQt();

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Writes the log message to qDebug.
    /// \param l : The message to write
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void impl_verbose(const char *l) override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Writes the log message to qDebug.
    /// \param l : The message to write
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void impl_debug(const char *l) override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Writes the log message to qDebug.
    /// \param l : The message to write
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void impl_information(const char *l) override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Writes the log message to qWarning.
    /// \param l : The message to write
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void impl_warning(const char *l) override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Writes the log message to qCritical.
    /// \param l : The message to write
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void impl_error(const char *l) override final;
};

#endif // LOGFORQT_H
