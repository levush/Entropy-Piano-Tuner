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

#ifndef GRAPHICSITEMFORQT_H
#define GRAPHICSITEMFORQT_H

#include <QGraphicsItem>

#include "prerequisites.h"

#include "core/drawers/graphicsitem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Implementation class for the GraphicsItem in Qt
///
/// This implementation class holds and manages a pointer pointing to the
/// corresponding QGraphicsItem
///////////////////////////////////////////////////////////////////////////////

class GraphicsItemForQt : public GraphicsItem
{
public:
    GraphicsItemForQt(GraphicsViewAdapter *graphicsView,
                      QGraphicsItem *item);
    ~GraphicsItemForQt();

    void setItem(QGraphicsItem *item);
    virtual void setPosition(double x, double y) override final;
    virtual void setZOrder(double z) override final;

private:
    QGraphicsItem *mItem;  ///< Pointer pointing to the QGraphicsItem
};

#endif // GRAPHICSITEMFORQT_H
