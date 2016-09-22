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
//           FFTW3 implementation for fast Fourier transformations
//=============================================================================

#ifndef FFT_IMPLEMENTATION_H
#define FFT_IMPLEMENTATION_H

#include "fftadapter.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Thread-safe implementation of fftw3
///
/// <b>USAGE:</b> FFT's can be carried out simply by calling the
/// function calculateFFT with the corresponding arguments. If a given
/// instance has to carry out a larger number of FFTs using the same vector
/// sizes it is meaningful to optimize the calculation by initially calling the
/// function optimize with the input vector as the argument.
///
/// <b>DETAILS:</b>
/// A Fourier transformation using fftw3 requires two steps. First a
/// so-called 'plan' has to be computed which optimizes the
/// FFT algorithm. Then in a second step the actual Fourier transform is
/// carried out. A plan may be kept for several calculations as long
/// as the location of the vectors and their sizes are not changed.
/// By keeping a plan the computation time is drastically reduced.
///
/// As of now, the second step (the FFT) is thread-safe while the first one is not.
/// This means that only one thread is allowed to create a plan at a given
/// time. To this end this implementation class protects all accesses
/// to planmaking by a static mutex. In addition, it keeps local input
/// and output vectors with a constant location.
///
/// The class provides two types of FFTs, namely, real to complex
/// and comples to real. The second one is the inverse of the first one.
///
/// Since memory allocation should be carried out with the inbuilt
/// allocation function of FFTW3, the implementation copies the vectors
/// into local member vectors by memcpy.
///////////////////////////////////////////////////////////////////////////////

#include <fftw3.h>
#include <mutex>

#include "prerequisites.h"


class EPT_EXTERN FFT_Implementation : public FFTAdapter
{
public:

    FFT_Implementation();
    ~FFT_Implementation();

    void calculateFFT  (const FFTRealVector &in, FFTComplexVector &out);
    void calculateFFT  (const FFTComplexVector &in, FFTRealVector &out);

    void optimize (FFTRealVector &in);
    void optimize (FFTComplexVector &in);


private:

    // R and C mean: real and complex
    // CR means: complex to real
    // RC means: real to complex

    double       *mRvec1;               ///< Local copy of incoming real data
    double       *mRvec2;               ///< Local copy of outgoing real data
    fftw_complex *mCvec1;               ///< Local copy of incoming complex data
    fftw_complex *mCvec2;               ///< Local copy of outgoing complex data
    size_t       mNRC;                  ///< Size of the FFT real -> complex
    size_t       mNCR;                  ///< Size of the FFT complex -> real

    fftw_plan mPlanRC;                  ///< Plan for FFT real -> complex
    fftw_plan mPlanCR;                  ///< Plan for FFT complex -> real
    static std::mutex mPlanMutex;       ///< Static mutex protecting planmaking

    void updatePlan (const FFTRealVector &in, unsigned flags);
    void updatePlan (const FFTComplexVector &in, unsigned flags);
};

#endif // FFT_IMPLEMENTATION_H
