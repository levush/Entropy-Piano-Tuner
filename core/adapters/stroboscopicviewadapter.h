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

#ifndef STROBOSCOPICVIEWADAPTER_H
#define STROBOSCOPICVIEWADAPTER_H

#include <vector>
#include <complex>

//////////////////////////////////////////////////////////////////////////////
/// \brief Stroboscopic view adapter class.
///
/// This class contains only a single virtual function to draw the stroboscope.
///
//////////////////////////////////////////////////////////////////////////////

class StroboscopicViewAdapter
{
public:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Abstract function: Draw stroboscope
    ////
    /// A stroboscope is a tuning indicator which displays interference
    /// patterns of the incoming PCM data with respect to the target
    /// frequency. In the EPT the stroboscopic display is simplified in that
    /// it only gets the amplitudes and complex phases of the first
    /// few partials. Nevertheless the data should be displayed immediately.
    /// Therefore, the whole drawing is done in the implementation of this
    /// adapter.
    /// \see GraphicsViewAdapterForQt
    /// \see GraphicsItem
    /// \param data : Vector of complex numbers, encoding the amplitudes
    /// and the phase shifts of the partials.
    /// \return Pointer to the created GraphicsItem.
    ///////////////////////////////////////////////////////////////////////////

    using ComplexVector = std::vector<std::complex<double>>;
    virtual void drawStroboscope (const ComplexVector &data) = 0;

};

#endif // STROBOSCOPICVIEWADAPTER_H
