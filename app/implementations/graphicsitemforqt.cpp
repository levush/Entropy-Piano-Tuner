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

//============================================================================
//                     Graphics Item implementation for Qt
//============================================================================

#include "graphicsitemforqt.h"

#include "../core/system/eptexception.h"
#include "graphicsviewadapterforqt.h"


//-----------------------------------------------------------------------------
//                               Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, initializing the pointer mItem
///////////////////////////////////////////////////////////////////////////////
GraphicsItemForQt::GraphicsItemForQt (GraphicsViewAdapter *graphicsView,
                                      QGraphicsItem *item) :
    GraphicsItem(graphicsView),
    mItem(item)
{
    EptAssert(item, "The item must exist");
}


//-----------------------------------------------------------------------------
//                               Destructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Destructor, deletes the item and sets mItem to nullptr.
///////////////////////////////////////////////////////////////////////////////

GraphicsItemForQt::~GraphicsItemForQt()
{
    if (mItem)
    {
        delete mItem;
        mItem = nullptr;
    }
}


//-----------------------------------------------------------------------------
//                  Set the associated QGraphicsItem pointer
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set the associated QGraphicsItem pointer
/// \param item : Pointer to the QGraphicsItem
///////////////////////////////////////////////////////////////////////////////

void GraphicsItemForQt::setItem (QGraphicsItem *item)
{
    mItem = item;
}


//-----------------------------------------------------------------------------
//              Set or change the position of the graphical item
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set or change the position of the graphical item
/// \param x : Horizontal position varying from 0 to 1
/// \param y : Vertical position varying from 0 to 1
///////////////////////////////////////////////////////////////////////////////

void GraphicsItemForQt::setPosition (double x, double y)
{
    QPointF pos(static_cast<GraphicsViewAdapterForQt*>(mGraphicsView)->
                convertRelToAbs(QPointF(x, y)));
    mItem->setPos(pos);
}


//-----------------------------------------------------------------------------
//              Set or change the z-order of the graphical item
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set or change the z-order of the graphical item
/// \param z
///////////////////////////////////////////////////////////////////////////////

void GraphicsItemForQt::setZOrder (double z)
{
    mItem->setZValue(z);
}
