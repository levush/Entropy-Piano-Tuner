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
//                           Stroboscope-Drawer
//=============================================================================

#ifndef STROBOSCOPEDRAWER_H
#define STROBOSCOPEDRAWER_H

#include <complex>

#include "drawerbase.h"
#include "../messages/messagelistener.h"


class StroboscopeDrawer  : public DrawerBase, public MessageListener
{
public:
    StroboscopeDrawer(GraphicsViewAdapter *graphics);
    ~StroboscopeDrawer() {}

protected:
    virtual void draw() override final;
    virtual void reset() override final;
    virtual void handleMessage(MessagePtr m) override;

private:
    using ComplexVector = std::vector<std::complex<double>>;
    ComplexVector mDataVector;
    int convertHsvToRgb (double h, double s, double v=1);

};

#endif // STROBOSCOPEDRAWER_H
