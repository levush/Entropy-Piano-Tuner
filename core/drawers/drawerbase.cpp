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
//                           Drawer base class
//=============================================================================

#include "drawerbase.h"

//-----------------------------------------------------------------------------
//			                     Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor of a drawer
///
/// \param graphics : Pointer to the GraphicsViewAdapter
/// \param intervall : maximal update interval in seconds
///////////////////////////////////////////////////////////////////////////////

DrawerBase::DrawerBase (GraphicsViewAdapter *graphics, double intervall)
    : mGraphics(graphics),
      mTimeLastDrawn(new system_time_point(std::chrono::high_resolution_clock::now())),
      mRedrawIntervalInSecs(intervall)
{
}

DrawerBase::~DrawerBase ()
{
    delete mTimeLastDrawn;
}

//-----------------------------------------------------------------------------
//			                        Redraw
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function to completely redraw the scene.
///
/// The function first clears the scene and then calls the abstract
/// draw() method.
/// \param force : true if redrawing is forced.
///////////////////////////////////////////////////////////////////////////////

void DrawerBase::redraw (bool force)
{
    if (requestRedraw(force))
    {
        mGraphics->clear();
        draw();
    }
}


//-----------------------------------------------------------------------------
//          Check for redrawing. If true, set redraw timer to zero
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Check whether the content has to be redrawn
///
/// If force is set to false the function returns true on redraw timeout.
/// If force is set to true the function will always return true.
/// If returning true the function will automatically reset the timer.
/// \param force : Force redrawing
/// \return true for redrawing, false otherwise
///////////////////////////////////////////////////////////////////////////////

bool DrawerBase::requestRedraw (bool force)
{
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::high_resolution_clock::now() - *mTimeLastDrawn);

    if (force or elapsed.count() >= mRedrawIntervalInSecs * 1000)
    {
        *mTimeLastDrawn = std::chrono::high_resolution_clock::now();
        return true;
    }
    return false;
}

