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

//=============================================================================
//                      Core initialisation adapter
//=============================================================================

#ifndef COREINITIALISATIONADAPTER_H
#define COREINITIALISATIONADAPTER_H

#include "prerequisites.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Core initialization adapter (singleton class).
///
/// This adapter is part of the interface between the core and the GUI.
/// The purpose of this adapter and its derived implementation in the GUI is
/// to show a message box during the initialization process of the main
/// application. Moreover, it informs about the progress of the initialization
/// process in regular intervals by showing an advancing progress bar.
///
/// The class is reimplemented in Qt as
/// QtCoreInitialisation (see initializediagol.h). It is instantiated in
/// tunerapplication.cpp and passed via pointer to init(...) of the core.
/// The core itself calls create(), performs the initialization and calls
/// the function updateProgress between all the steps. Finally the core calls
/// destroy().
///
/// \see QtCoreInitialisation
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN CoreInitialisationAdapter
{
private:
    static std::unique_ptr<CoreInitialisationAdapter> mSingleton;

public:
    CoreInitialisationAdapter();

    virtual void create()=0;       ///< Create the initialization dialog box
    virtual void destroy()=0;      ///< Destroy the initialization dialog box

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Show the current progress as a progress bar
    ///        in the initialization message box.
    /// \param percentage : Percentage of progress between 0 and 100
    ///////////////////////////////////////////////////////////////////////////////
    virtual void updateProgress (int percentage) = 0;
};

#endif // COREINITIALISATIONADAPTER_H
