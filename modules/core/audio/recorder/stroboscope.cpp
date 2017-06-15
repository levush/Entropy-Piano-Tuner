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
//                               Stroboscope
//=============================================================================

#include "stroboscope.h"
#include "audiorecorder.h"
#include "../../messages/messagehandler.h"
#include "../../messages/messagestroboscope.h"
#include "../../math/mathtools.h"
#include "core/settings.h"

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stroboscope::Stroboscope
/// \param recorder
///////////////////////////////////////////////////////////////////////////////

Stroboscope::Stroboscope(AudioRecorder *recorder) :
    mRecorder(recorder),
    mActive(false),
    mSamplesPerFrame(22050),
    mSampleCounter(0),
    mMaxAmplitude(1E-21)
{
}


//-----------------------------------------------------------------------------
//   push raw PCM data to the stroboscope (called by AudioRecorderAdapter)
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stroboscope::pushRawData
/// \param data
///////////////////////////////////////////////////////////////////////////////

void Stroboscope::pushRawData (const PacketType &data)
{
    // data packet size is typically 1100, can be 0.
    if (mActive) if (Settings::getSingleton().isStroboscopeActive())
    {
        std::lock_guard<std::mutex> lock (mMutex);
        for (auto &pcm : data)
        {
            if (fabs(pcm)>mMaxAmplitude) mMaxAmplitude=fabs(pcm);
            if (mMaxAmplitude < 1E-20) continue;

            int N = static_cast<int>(mComplexPhase.size());
            for (int i=0; i<N; ++i)
            {
                mComplexPhase[i] *= mComplexIncrement[i];
                mMeanComplexPhase[i] += mComplexPhase[i] * pcm / mMaxAmplitude;
            }
            if (mSampleCounter-- <= 0)
            {
                for (auto &c : mComplexPhase) c /= std::abs(c);
                ComplexVector normalizedPhases (mMeanComplexPhase);
                for (auto &c : normalizedPhases) c /= 0.5*mSamplesPerFrame/(1-FRAME_DAMPING);
                MessageHandler::sendUnique<MessageStroboscope>(normalizedPhases);

                for (auto &c : mMeanComplexPhase) c *= FRAME_DAMPING;
                mMaxAmplitude *= AMPLITUDE_DAMPING;
                mSampleCounter = mSamplesPerFrame;
            }
        }
    }
}


//-----------------------------------------------------------------------------
//                          Set frames per second
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stroboscope::setFramesPerSecond
/// \param fps
///////////////////////////////////////////////////////////////////////////////

void Stroboscope::setFramesPerSecond (double fps)
{
    mSamplesPerFrame = mRecorder->getSampleRate() / fps;
}


//-----------------------------------------------------------------------------
//              Set frequencies of the partials to be analyzed
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stroboscope::setFrequencies
/// \param frequencies
///////////////////////////////////////////////////////////////////////////////

void Stroboscope::setFrequencies(const std::vector<double> &frequencies)
{
    std::lock_guard<std::mutex> lock (mMutex);
    mComplexPhase.resize(frequencies.size());
    mComplexPhase.assign(frequencies.size(),1);
    mMeanComplexPhase.resize(frequencies.size());
    mMeanComplexPhase.assign(frequencies.size(),0);
    mComplexIncrement.clear();
    for (auto &f : frequencies)
        mComplexIncrement.push_back(std::exp(Complex(0,MathTools::TWO_PI) *
                                    (f/mRecorder->getSampleRate())));
}

