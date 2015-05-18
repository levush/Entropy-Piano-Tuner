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

#ifndef FOURIERSPECTRUMGRAPHDRAWER_H
#define FOURIERSPECTRUMGRAPHDRAWER_H

#include "../system/prerequisites.h"
#include "drawerbase.h"
#include "../piano/keyboard.h"
#include "../messages/messagelistener.h"
#include "../analyzers/signalanalyzer.h"

class FourierSpectrumGraphDrawer : public DrawerBase, public MessageListener
{
public:
    enum RoleType {
        ROLE_GLOBAL   = 1,
        ROLE_CHART    = 2,
        ROLE_PEAK     = 4
    };

public:
    FourierSpectrumGraphDrawer(GraphicsViewAdapter *graphics);
    ~FourierSpectrumGraphDrawer() {}

protected:
    virtual void draw() override final;
    virtual void reset() override final;
    virtual void handleMessage(MessagePtr m) override;

    void updateSpectrum();

private:
    double mConcertPitch;
    int mKeyNumberOfA4;
    int mNumberOfKeys;
    int mSamplingRate;
    OperationMode mCurrentOperationMode;
    std::shared_ptr<FFTPolygon> mPolygon;
    std::shared_ptr<Key> mKey;
};

#endif // FOURIERSPECTRUMGRAPHDRAWER_H
