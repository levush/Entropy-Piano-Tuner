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

#ifndef PLATFORMTOOLS_H
#define PLATFORMTOOLS_H

#include <memory>
#include <QString>
#include <QStringList>

#include "prerequisites.h"

#include "core/system/platformtoolscore.h"

class TunerApplication;

///////////////////////////////////////////////////////////////////////////////
/// Namespace that contains platform dependent implementations of certain
/// tools.
///
///////////////////////////////////////////////////////////////////////////////
class PlatformTools : public PlatformToolsCore {
public:
    static PlatformTools *getSingleton() {return dynamic_cast<PlatformTools*>(PlatformToolsCore::getSingleton());}

    /// \brief Initialise native components (e.g. midi on android/iOS)
    virtual void init() {}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Load a startup file from the given arguments.
    /// \param args : The startup arguments
    /// \return Loading success.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool loadStartupFile(const QStringList args);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief open the startup file from the arguments
    /// \param args : The program arguments
    /// \return true on success
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool openFileFromArgs(const QStringList &args);
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to disable the screen saver.
    ///
    /// Working on android, Mac Os X, iOS, Windows.
    ///////////////////////////////////////////////////////////////////////////////
    virtual void disableScreensaver() {}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to enable the screen saver.
    ///
    /// Working on android, Mac Os X, iOS, Windows.
    ///////////////////////////////////////////////////////////////////////////////
    virtual void enableScreensaver() {}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function called from the implementations to open a file.
    /// \param file : Absolute file path to open.
    /// \param cached : Is the file cached?
    ///
    /// This function will use the singleton class TunerApplication to open the
    /// file.
    ///////////////////////////////////////////////////////////////////////////////
    void openFile(const char *file, bool cached);
};

template <typename PT>
class PlatformToolsImplementation : public PlatformTools {
protected:
    static std::unique_ptr<PT> mSingleton;
public:
};

#endif // PLATFORMTOOLS_H
