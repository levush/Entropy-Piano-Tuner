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

#ifndef PLATFORMTOOLS_H
#define PLATFORMTOOLS_H

#include <QString>
#include <QStringList>

class TunerApplication;

///////////////////////////////////////////////////////////////////////////////
/// Namespace that contains platform dependent implementations of certain
/// tools.
///
///////////////////////////////////////////////////////////////////////////////
namespace platformtools {

///////////////////////////////////////////////////////////////////////////////
/// \brief Load a startup file from the given arguments.
/// \param args : The startup arguments
/// \return Loading success.
///
///////////////////////////////////////////////////////////////////////////////
bool loadStartupFile(const QStringList args);

///////////////////////////////////////////////////////////////////////////////
/// \brief open the startup file from the arguments
/// \param args : The program arguments
/// \return true on success
///
///////////////////////////////////////////////////////////////////////////////
bool openFileFromArgs(const QStringList &args);

///////////////////////////////////////////////////////////////////////////////
/// \brief Function called from the implementations to open a file.
/// \param file : Absolute file path to open.
/// \param cached : Is the file cached?
///
/// This function will use the singleton class TunerApplication to open the
/// file.
///////////////////////////////////////////////////////////////////////////////
void openFile(const char *file, bool cached);

///////////////////////////////////////////////////////////////////////////////
/// \brief Function to disable the screen saver.
///
/// Working on android, Mac Os X, iOS, Windows.
///////////////////////////////////////////////////////////////////////////////
void disableScreensaver();

///////////////////////////////////////////////////////////////////////////////
/// \brief Function to enable the screen saver.
///
/// Working on android, Mac Os X, iOS, Windows.
///////////////////////////////////////////////////////////////////////////////
void enableScreensaver();


}  // namespace platformtools

#endif // PLATFORMTOOLS_H
