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

#ifndef DRAWERBASE_H
#define DRAWERBASE_H

#include <ctime>
#include <chrono>

#include "prerequisites.h"
#include "../adapters/graphicsviewadapter.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract base class for drawing 2d graphics
///
/// A drawer is an object that draws 2d graphics by calling the
/// methods of a GraphicsViewAdapter.
/// A call of redraw() redraws the complete graphics.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN DrawerBase
{
public:
    DrawerBase(GraphicsViewAdapter *graphics, double intervall = 1.0/24.0);
    ~DrawerBase();

    void redraw(bool force = false);
    bool requestRedraw(bool force = false);

protected:
    virtual void draw() = 0;                    ///< Abstract function : draw the content
    virtual void clear() {mGraphics->clear();}  ///< Clear the whole view

    GraphicsViewAdapter *mGraphics;             ///< Pointer to the graphics view adapter

private:
    using system_time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
    system_time_point *mTimeLastDrawn;          ///< Timeposition when last drawn
    double mRedrawIntervalInSecs;               ///< Update time
};

#endif // DRAWERBASE_H
