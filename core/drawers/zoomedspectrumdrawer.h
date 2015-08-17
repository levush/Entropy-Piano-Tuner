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
//                          Draw zoomed spectrum
//=============================================================================

#ifndef ZOOMEDSPECTRUMDRAWER_H
#define ZOOMEDSPECTRUMDRAWER_H

#include "drawerbase.h"
#include "../messages/messagelistener.h"
#include "../analyzers/signalanalyzer.h"
#include "../piano/piano.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief The ZoomedSpectrumDrawer class
///
/// This class draws the zoomed spectrum in the tuning mode
///////////////////////////////////////////////////////////////////////////////

class ZoomedSpectrumDrawer : public DrawerBase, public MessageListener
{
public:
    ZoomedSpectrumDrawer(GraphicsViewAdapter *graphics);
    ~ZoomedSpectrumDrawer() {}

protected:
    virtual void draw() override final;
    virtual void reset() override final;
    virtual void handleMessage(MessagePtr m) override;

private:
    const Piano *mPiano;
    int mNumberOfKeys;
    int mSelectedKey;
    int mRecognizedKey;
    OperationMode mOperationMode;

    FFTDataPointer mFFTData;
    FrequencyDetectionResult mFrequencyDetectionResult;

};

#endif // ZOOMEDSPECTRUMDRAWER_H
