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

//=============================================================================
//                           Drawer base class
//=============================================================================

#include "drawerbase.h"

#include <thread>

#include "../system/prerequisites.h"

//-----------------------------------------------------------------------------
//			                     Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
///
/// \param graphics : Pointer to the GraphicsViewAdapter
/// \param intervall : maximal update interval in seconds
///////////////////////////////////////////////////////////////////////////////

DrawerBase::DrawerBase(GraphicsViewAdapter *graphics, double intervall)
    : mGraphics(graphics),
      mTimeLastDrawn(std::chrono::high_resolution_clock::now()),
      mRedrawIntervalInSecs(intervall)
{
}


//-----------------------------------------------------------------------------
//			                        Redraw
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function to redraw the scene.
///
/// The function firstly clears the scene and then calls the abstract
/// draw() method.
/// \param force : true if redrawing is forced.
///////////////////////////////////////////////////////////////////////////////

void DrawerBase::redraw(bool force)
{
    if (reqestRedraw(force))
    {
        mGraphics->clear();
        draw();
    }
}

//-----------------------------------------------------------------------------
//			                        Redraw
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function to check if one may redraw.
/// \param force : Force redrawing
/// \return true for redrawing, false elsewise
///
/// If force is set to true the function will always return true.
/// If returning true the function will automatically reset the timer.
///////////////////////////////////////////////////////////////////////////////
///
bool DrawerBase::reqestRedraw(bool force)
{
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - mTimeLastDrawn);

    if (force or elapsed.count() >= mRedrawIntervalInSecs * 1000) {
        mTimeLastDrawn = std::chrono::high_resolution_clock::now();
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
//			                        Reset
//-----------------------------------------------------------------------------

///  \brief Reset graphics (clear)

void DrawerBase::reset()
{
    mGraphics->clear();
}
