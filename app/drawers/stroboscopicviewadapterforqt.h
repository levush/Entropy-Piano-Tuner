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
//            Graphics implementation of the stroboscope for Qt
//=============================================================================

#ifndef STROBOSCOPICVIEWADAPTERFORQT_H
#define STROBOSCOPICVIEWADAPTERFORQT_H

#include "prerequisites.h"

#include "core/adapters/graphicsviewadapter.h"

#include "implementations/graphicsviewadapterforqt.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Stroboscopic view adapter for Qt
///
/// This is the implementation of the stroboscopic view adapter for Qt.
/// The essential function is drawStroboscope, which depends on a vector of
/// complex numbers. The stroboscope shows stripes of rainbow colorscales with an
/// intensity proportional to the absolute values of the complex numbers
/// and a horizontal shift proportional to the phase of the complex numbers.
/// This function is time-critical and uses direct memory access.
///
/// This class is derived from GraphicsViewAdapterForQt. It is therefore a
/// special type of GraphicsView which is able to display a stroboscope.
/// \see GraphicsViewAdapterForQt
///////////////////////////////////////////////////////////////////////////////

class StroboscopicViewAdapterForQt : public GraphicsViewAdapterForQt
{
public:

    StroboscopicViewAdapterForQt (QWidget *parent, DrawerBase *drawer, QRectF sceneRect);
    ~StroboscopicViewAdapterForQt();

    virtual void drawStroboscope (const ComplexVector &data) override final;
    void clear() override final;

private:

    QGraphicsPixmapItem *mStroboscopeItem;      ///< Pointer to the stroboscope

};

#endif // STROBOSCOPICVIEWADAPTERFORQT_H
