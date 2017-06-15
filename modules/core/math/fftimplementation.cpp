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
//             FFTW3 implementation for fast Fourier transformations
//=============================================================================

#include "fftimplementation.h"

#include <cstring>
#include <iostream>
#include <typeinfo>

#include "../system/eptexception.h"
#include "../system/log.h"

//-----------------------------------------------------------------------------
//                              static mutex
//-----------------------------------------------------------------------------

std::mutex FFT_Implementation::mPlanMutex;


//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, clears member variables and checks type consistency
///////////////////////////////////////////////////////////////////////////////

FFT_Implementation::FFT_Implementation() :
    mRvec1(nullptr),
    mRvec2(nullptr),
    mCvec1(nullptr),
    mCvec2(nullptr),
    mNRC(0),
    mNCR(0),
    mPlanRC(nullptr),
    mPlanCR(nullptr)
{
    // Check the consistency of types defined in the adapater:
    EptAssert (typeid(FFTRealType)==typeid(double),
               "FFT only implemented for real data type double.");
    EptAssert (typeid(FFTComplexType)==typeid(std::complex<double>),
               "FFT only implemented for complex data type std::complex<double>.");
}


//-----------------------------------------------------------------------------
//                                Destructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Destructor, deletes plans and local vector copies if existing.
///////////////////////////////////////////////////////////////////////////////

FFT_Implementation::~FFT_Implementation()
{
    std::lock_guard<std::mutex> lock(mPlanMutex);
    try
    {
        if (mPlanRC) fftw_destroy_plan(mPlanRC);
        if (mPlanCR) fftw_destroy_plan(mPlanCR);
        if (mRvec1) free(mRvec1);
        if (mCvec2) fftw_free(mCvec2);
        if (mCvec1) fftw_free(mCvec1);
        if (mRvec2) free(mRvec2);
    }
    catch (...) LogE("fftw3_destroy_plan throwed an exception");
}


//-----------------------------------------------------------------------------
//            Optimize forward transformation real -> complex
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Optimize plan for the forward transfomation.
///
/// This function should be called at the beginning if the subsequent
/// code performs many FFTs on vectors with varying content, but with
/// the same location and the same size. Note that this function may be
/// time-consuming, but it accelerates subsequent computations significantly.
///
/// \param in : vector of real numbers to be transformed
///////////////////////////////////////////////////////////////////////////////

void FFT_Implementation::optimize (FFTRealVector &in)
{
    updatePlan(in,FFTW_PATIENT);
}


//-----------------------------------------------------------------------------
//             Optimize backward transformation complex -> real
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Optimize plan for the backward transformation.
///
/// This function should be called at the beginning if the subsequent
/// code performs many FFTs on vectors with varying content, but with
/// the same location and the same size. Note that this function may be
/// time-consuming, but it accelerates subsequent computations significantly.
///
/// \param in : vector of complex numbers to be transformed
///////////////////////////////////////////////////////////////////////////////

void FFT_Implementation::optimize (FFTComplexVector &in)
{
    updatePlan(in,FFTW_MEASURE);
}


//-----------------------------------------------------------------------------
//   Private function: construct a plan for transformations real->complex
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Construct a new plan for the FFT transformation if necessary.
///
/// This function checks whether the plan and the local vectors still
/// exist and whether they have the correct size. If so, the function does
/// nothing which means that the existing plan will be reused. Ohterwise,
/// it deletes the plan and the vectors and constructs them again
/// with the given size.
///
/// \param in : vector of real numbers of size N
/// \param flags : fftw3 internal flags controlling planmaking
///////////////////////////////////////////////////////////////////////////////

void FFT_Implementation::updatePlan (const FFTRealVector &in,
                                     unsigned flags)
{
    // if plan still exists and input vector has the same size do nothing
    if (mPlanRC and mRvec1 and mCvec2 and in.size()==mNRC) return
    EptAssert(in.size()>0,"vector size has to be nonzero");

    std::lock_guard<std::mutex> lock(mPlanMutex);
    try {
        // delete old plan and vectors
        if (mPlanRC) fftw_destroy_plan(mPlanRC);
        if (mRvec1) free(mRvec1);
        if (mCvec2) fftw_free(mCvec2);

        // allocate new vectors and create a new plan
        mNRC   = in.size();
        mRvec1 = (double *) malloc(mNRC*sizeof(double));
        mCvec2 = (fftw_complex*) fftw_malloc((mNRC/2+1)*sizeof(fftw_complex));
        EptAssert(mRvec1, "May not be nullptr");
        EptAssert(mCvec2, "May not be nullptr");
        mPlanRC = fftw_plan_dft_r2c_1d (static_cast<int>(mNRC), mRvec1, mCvec2, flags);
    }
    catch (...) LogE("fftw_pplan_dft_r2c_1d throwed an exception");
}


//-----------------------------------------------------------------------------
//   Private function: construct a plan for transformations complex->real
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Construct a new plan for the FFT transformation if necessary.
///
/// This function checks whether the plan and the local vectors still
/// exist and whether they have the correct size. If so, the function does
/// nothing which means that the existing plan will be reused. Ohterwise,
/// it deletes the plan and the vectors and constructs them again
/// with the given size.
///
/// \param in : vector of complex numbers
/// \param flags : fftw3 internal flags controlling planmaking
///////////////////////////////////////////////////////////////////////////////

void FFT_Implementation::updatePlan (const FFTComplexVector &in,
                                     unsigned flags)
{
    // if plan still exists and input vector has the same size do nothing
    if (mPlanCR and mCvec1 and mRvec2 and in.size()==mNCR/2+1) return;
    EptAssert(in.size()>0,"vector size has to be nonzero");

    std::lock_guard<std::mutex> lock(mPlanMutex);
    try {
        // delete old plan and vectors
        if (mPlanCR) fftw_destroy_plan(mPlanCR);
        if (mCvec1) fftw_free(mCvec1);
        if (mRvec2) free(mRvec2);

        // allocate new vectors and create a new plan
        mNCR   = 2*in.size()-2;
        mCvec1 = (fftw_complex*) fftw_malloc((mNCR/2+1)*sizeof(fftw_complex));
        mRvec2 = (double *) malloc(mNCR*sizeof(double));
        EptAssert(mCvec1, "May not be nullptr");
        EptAssert(mRvec2, "May not be nullptr");
        mPlanCR = fftw_plan_dft_c2r_1d (static_cast<int>(mNCR), mCvec1, mRvec2, flags);
    }
    catch (...) LogE("fftw_pplan_dft_c2r_1d throwed an exception");
}


//-----------------------------------------------------------------------------
//                   Calculate forward FFT real -> complex
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Foward FFT, mapping a real vector with size N to a complex one with
/// size N/2+1.
///
/// \param in : vector of real numbers of size N
/// \param out : vector of complex numbers, will be resized to N/2+1
///////////////////////////////////////////////////////////////////////////////

void FFT_Implementation::calculateFFT  (const FFTRealVector &in, FFTComplexVector &out)
{
    // resize output vector
    if (out.size() != in.size()/2+1) out.resize(in.size()/2+1);

    // check for persistent data
    if (in.size() == 0) {
        LogD("Calling FFT with empty vector. Skipping computation");
        return;
    }

    // Perform the computation
    updatePlan(in,FFTW_ESTIMATE);
    EptAssert (in.size()==mNRC and out.size()==mNRC/2+1,"Vector consistency");
    try {
        std::memcpy(mRvec1,in.data(),mNRC*sizeof(double));
        fftw_execute(mPlanRC);
        std::memcpy(out.data(),mCvec2,(mNRC/2+1)*sizeof(fftw_complex));
    }
    catch (...) LogE("fftw_execute throwed an exception");
}


//-----------------------------------------------------------------------------
//                   Calculate backward FFT complex -> real
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Backward FFT, mapping a complex vector with size M to a
/// complex one with size 2M-1.
///
/// \param in : vector of complex numbers of size M
/// \param out : vector of real numbers, will be resized to size 2*M-1
///////////////////////////////////////////////////////////////////////////////

void FFT_Implementation::calculateFFT  (const FFTComplexVector &in, FFTRealVector &out)
{
    EptAssert (in.size()>=1,"calling FFT with empty vector");
    if (out.size() != 2*in.size()-2) out.resize(2*in.size()-2);
    updatePlan(in,FFTW_ESTIMATE);
    EptAssert (in.size()==mNCR/2+1 and out.size()==mNCR,"Vector consistency");
    try {
        std::memcpy(mCvec1,in.data(),(mNCR/2+1)*sizeof(fftw_complex));
        fftw_execute(mPlanCR);
        std::memcpy(out.data(),mRvec2,mNCR*sizeof(double));
    }
    catch (...) LogE("fftw_execute throwed an exception");
}
