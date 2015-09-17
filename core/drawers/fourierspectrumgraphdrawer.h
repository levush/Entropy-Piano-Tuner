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

///////////////////////////////////////////////////////////////////////////////
/// \brief The FourierSpectrumGraphDrawer class
///
/// This class is drawing the spectrum (the red line) and if available the
/// peak markers on top of it.
///////////////////////////////////////////////////////////////////////////////

class FourierSpectrumGraphDrawer : public DrawerBase, public MessageListener
{
public:
    enum RoleType
    {
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
    double mConcertPitch;                   ///< Target freuqency of A4
    int mKeyNumberOfA4;                     ///< Index of A4 key
    int mNumberOfKeys;                      ///< Total number of keys
    int mSamplingRate;                      ///< copy of sample rate
    OperationMode mCurrentOperationMode;    ///< Current mode of operation
    std::shared_ptr<FFTPolygon> mPolygon;   ///< Shared pointer to spectral polygon
    std::shared_ptr<Key> mKey;              ///< Shared pointer to selected key, holding the peaks
};

#endif // FOURIERSPECTRUMGRAPHDRAWER_H
