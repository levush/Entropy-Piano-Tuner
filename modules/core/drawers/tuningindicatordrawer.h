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
//                            Tuning indicator
//=============================================================================

#ifndef TUNINGINDICATORDRAWER_H
#define TUNINGINDICATORDRAWER_H

#include "prerequisites.h"
#include "drawerbase.h"
#include "../messages/messagelistener.h"
#include "../analyzers/signalanalyzer.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Drawer for the tuning indicator
///
/// This class draws the content of the tuning indicator in the tuning mode.
/// It is completely driven by messages. There are two operation modes
/// depending on the settings: (a) the spectral mode, where a zoomed part of
/// the spectrum is displayed which has to be brought to the center, and
/// (b) the stroboscopic mode, where a stroboscopic interference pattern is
/// shown in rainbow colors.
/// \see Stroboscope
/// \see TuningIndicatorView
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN TuningIndicatorDrawer : public DrawerBase, public MessageListener
{
public:
    TuningIndicatorDrawer(GraphicsViewAdapter *graphics);
    ~TuningIndicatorDrawer() {}

    void toggleSpectralAndStroboscopeMode();

protected:
    virtual void draw() override final;
    virtual void clear() override final;
    virtual void handleMessage(MessagePtr m) override;

private:
    const Piano *mPiano;            ///< Pointer to the piano
    int mNumberOfKeys;              ///< Total number of keys
    int mSelectedKey;               ///< Number of selected key, -1 if none
    int mRecognizedKey;             ///< Number of recognized key, -1 if none
    OperationMode mOperationMode;   ///< Current operation mode of the EPT

    FFTDataPointer mFFTData;        ///< Pointer to the Fourier transform data
    FrequencyDetectionResult mFrequencyDetectionResult; ///< Copy of frequency detection result

    using ComplexVector = std::vector<std::complex<double>>;
    ComplexVector mDataVector;      ///< Data vector holding complex phases for stroboscope
};

#endif // TUNINGINDICATORDRAWER_H
