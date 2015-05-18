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

#include "zoomedspectrumdrawer.h"

#include <cstdint>
#include <iostream>
#include <algorithm>

#include "../messages/messagenewfftcalculated.h"
#include "../messages/messagekeyselectionchanged.h"
#include "../messages/messagepreliminarykey.h"
#include "../messages/messagemodechanged.h"
#include "../messages/messageprojectfile.h"
#include "../messages/messagetuningdeviation.h"
#include "../math/mathtools.h"

ZoomedSpectrumDrawer::ZoomedSpectrumDrawer(GraphicsViewAdapter *graphics) :
    DrawerBase(graphics),
    mPiano(nullptr),
    mNumberOfKeys(0),
    mKeyNumberOfA(0),
    mSelectedKey(-1),
    mRecognizedKey(-1),
    mOperationMode(MODE_COUNT)
{
}

//-----------------------------------------------------------------------------
//                            Message listener
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Message listener
/// \param m : Pointer to the incoming message
///////////////////////////////////////////////////////////////////////////////

void ZoomedSpectrumDrawer::handleMessage(MessagePtr m)
{
    switch (m->getType())
    {
    case Message::MSG_MODE_CHANGED:
    {
        auto mmc(std::static_pointer_cast<MessageModeChanged>(m));
        mOperationMode = mmc->getMode();
        redraw(true);
        break;
    }
    case Message::MSG_NEW_FFT_CALCULATED:
    {
        /*auto mnfc(std::static_pointer_cast<MessageNewFFTCalculated>(m));
        if (mnfc->hasError()) {
            mFFTData.reset();
            mSamplingRate = -1;
        } else {
            mFFTData = mnfc->getData();
            mSamplingRate = mFFTData->samplingRate;
        }
        redraw(true);*/
        break;
    }
    case Message::MSG_PRELIMINARY_KEY:
    {
        if (mOperationMode==MODE_TUNING)
        {
            auto message(std::static_pointer_cast<MessagePreliminaryKey>(m));
            mRecognizedKey = message->getKeyNumber();
            redraw(true);
        }
        else mRecognizedKey=-1;
        break;
    }
    case Message::MSG_KEY_SELECTION_CHANGED:
    {
        auto mksc(std::static_pointer_cast<MessageKeySelectionChanged>(m));
        mSelectedKey = mksc->getKeyNumber();
        redraw(true);
        break;
    }
    case Message::MSG_PROJECT_FILE:
    {
        auto mpf(std::static_pointer_cast<MessageProjectFile>(m));
        mPiano = &mpf->getPiano();
        mNumberOfKeys = mPiano->getKeyboard().getNumberOfKeys();
        mSelectedKey = std::min<int>(mSelectedKey, mNumberOfKeys);
        redraw();
        break;
    }
    case Message::MSG_TUNING_DEVIATION: {
        auto mtd(std::static_pointer_cast<MessageTuningDeviation>(m));
        mFrequencyDetectionResult = mtd->getResult();
        redraw();
        break;
    }
    default:
        break;
    }
}

//-----------------------------------------------------------------------------
//                                  Reset
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Reset: Clear the shared pointer to the FFT
///////////////////////////////////////////////////////////////////////////////

void ZoomedSpectrumDrawer::reset()
{
    mFFTData.reset();
    DrawerBase::reset();
}


//-----------------------------------------------------------------------------
//                             Drawing function
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Main drawing function
///////////////////////////////////////////////////////////////////////////////


void ZoomedSpectrumDrawer::draw()
{
    //------- Draw a horizontal and a vertical line separating the field ------

    const int cents = 10; // Width for the squared tuning marker
    const double dx = 1.0/(2+cents);
    for (double x=dx; x<1; x+=dx)
        mGraphics->drawLine(x, 0.8, x, 1, GraphicsViewAdapter::PEN_THIN_LIGHT_GRAY);
    mGraphics->drawLine(0.5, 0, 0.5, 0.8, GraphicsViewAdapter::PEN_THIN_DARK_GRAY);
    mGraphics->drawLine(0,   0.8, 1, 0.8, GraphicsViewAdapter::PEN_THIN_DARK_GRAY);

    int keynumber = (mSelectedKey >= 0 ? mSelectedKey : mRecognizedKey);
    if (keynumber<0) return;

//    if (not mFFTData) {return;}         // we need data
    if (not mFrequencyDetectionResult) {return;} // we need data
    if (not mPiano) {return;}           // and a piano

    const Key &key = mPiano->getKey(keynumber);
    double f = key.getComputedFrequency() * mPiano->getConcertPitch() / 440.0;

    // -------------------------- Analyze the spectrum -------------------------

//    const Key::PeakListType &peaks = key.getPeaks();

//    // For the low keys we do not watch the ground frequency, instead we montor
//    // the first or the second partial which is much stronger.
//    if (mKeyNumberOfA-keynumber > 24 and peaks.size()>=3)
//    {
//        auto it = peaks.begin();
//        double f1 = it->second;
//        it++;
//        if (mKeyNumberOfA-keynumber > 36) it++;
//        f *= it->second / f1;
//    }

//    auto qtof = [this] (int q) { return (double)q / mFFTData->getTime(); };
//    auto ftoq = [this] (double f) { return f * mFFTData->getTime(); };

//    const double width_in_cents = 20;
//    const double factor = pow(2.0,width_in_cents/1200);
//    int qmin = MathTools::roundToInteger (ftoq(f/factor));
//    int qmax = MathTools::roundToInteger (ftoq(f*factor));

//    // fix bounds
//    qmin = std::max<int>(0, qmin);
//    qmax = std::min<int>(mFFTData->fft.size() - 1, qmax);

//    std::vector<GraphicsViewAdapter::Point> points;
//    double max=0,absolutemax=0;
//    for (double &e : mFFTData->fft) if (e>absolutemax) absolutemax=e;
//    for (int q=qmin; q<=qmax; ++q) if (mFFTData->fft[q]>max) max=mFFTData->fft[q];

//    // do not paint if there is not enough intensity
//    if (max<absolutemax/10000) return;

//    // ------------------- Draw the zoomed spectrum (RED) ---------------------

//    for (int q=qmin; q<=qmax; ++q)
//    {
//        double y=0.8-0.77*pow(mFFTData->fft[q]/max,0.66);
//        double x=((double)q-qmin)/(qmax-qmin);
//        points.push_back({x, y});
//    }
//    mGraphics->drawChart(points, GraphicsViewAdapter::PEN_THIN_RED);

//    // ------------------- Draw superposition (CYAN) -------------------------
//    if (peaks.size()>0)
//    {
//        const double width_in_cents = 30;
//        const double factor = pow(2.0,width_in_cents/1200);
//        int qmin = MathTools::roundToInteger (ftoq(f/factor));
//        int qmax = MathTools::roundToInteger (ftoq(f*factor));
//        int size = qmax-qmin+1;
//        std::ofstream os ("0-WEIGHT.dat");
//        if (size>0)
//        {
//            std::vector<double> superposition(size+1,0);
//            int countpeaks = 0;
//            for (auto &p : peaks)
//            {
//                double fn = p.first;
//                int qn = MathTools::roundToInteger (ftoq(fn));
//                for (int n=0; n<=size; ++n) superposition[n] += std::pow(mFFTData->at(qn+n-size/2),0.5);
//                for (int n=0; n<=size; ++n) os << std::pow(mFFTData->at(qn+n-size/2),0.5) << std::endl;
//                os << "&" << std::endl;
//                if (++countpeaks > 1) break;
//            }
//            std::vector<GraphicsViewAdapter::Point> points;
//            double max=0;
//            for (int n=0; n<=size; ++n) if (superposition[n]>max) max=superposition[n];
//            if (max>0)
//            {
//                for (int n=0; n<=size; ++n)
//                {
//                    double y=0.8-0.77*pow(superposition[n]/max,0.5);
//                    double x=((double)n)/size;
//                    points.push_back({x, y});
//                }
//            }
//            mGraphics->drawChart(points, GraphicsViewAdapter::PEN_MEDIUM_CYAN);
//        }
//    }

    // only show a window of a part of the complete curve
    const int specWindowSize = mFrequencyDetectionResult->tuningDeviationCurve.size() / 4;

    //---------------------- Draw tuning deviation curve -----------------
    if (specWindowSize > 0) {
        double max = *std::max_element(mFrequencyDetectionResult->tuningDeviationCurve.begin(),
                                      mFrequencyDetectionResult->tuningDeviationCurve.end());

        std::vector<GraphicsViewAdapter::Point> points;
        const int centerIndex = mFrequencyDetectionResult->tuningDeviationCurve.size() / 2;
        const int startIndex = std::max<int>(0, centerIndex - specWindowSize / 2 + MathTools::roundToInteger(mFrequencyDetectionResult->positionOfMaximum) - mFrequencyDetectionResult->deviationInCents);
        const int endIndex = std::min<int>(startIndex + specWindowSize,
                                           mFrequencyDetectionResult->tuningDeviationCurve.size());
        for (int i = startIndex; i < endIndex; ++i) {
            double y = mFrequencyDetectionResult->tuningDeviationCurve[i] / max;
            y *= y;
            y = 0.8 - 0.77 * y;
            double x = static_cast<double>(i - startIndex) / specWindowSize;
            points.push_back({x, y});
        }
        mGraphics->drawChart(points, GraphicsViewAdapter::PEN_THIN_RED);
    }


    //----------------------- Draw tuning marker -------------------------

    double markerWidth = 0.1;
    double markerHeight = 0.1;
    double deviation = mFrequencyDetectionResult->deviationInCents;
    double mx = 0.5 - markerWidth / 2 + deviation / specWindowSize;
    double my = 0.85 - markerHeight / 2;


    auto borderline = GraphicsViewAdapter::PEN_THIN_DARK_GRAY;
    auto filling = GraphicsViewAdapter::FILL_RED;
    if (abs(deviation) < 5) filling = GraphicsViewAdapter::FILL_GREEN;
    else if (abs(deviation) < 10) filling = GraphicsViewAdapter::FILL_ORANGE;

    mGraphics->drawFilledRect(mx, my, markerWidth, markerHeight, borderline, filling);

//    if (peaks.size()>0)
//    {
//        const double width = cents/1200.0*MathTools::LOG2;
//        double norm=0, sum=0;
//        double fpeak1 = peaks.begin()->first;
//        for (auto &p : peaks)
//        {
//            double fn = p.first*f/fpeak1;
//            double R = std::min(f/2, fn*width);
//            int qmin = MathTools::roundToInteger (ftoq(fn-R));
//            int qmax = MathTools::roundToInteger (ftoq(fn+R));
//            double max=0; int qex=0;
//            for (int q=qmin; q<=qmax; ++q) if (mFFTData->fft[q]>max)
//            { max = mFFTData->fft[q]; qex=q; }
//            if (qex)
//            {
//                double df = qtof(qex)-fn;
//                sum += df/R;
//                norm++;
//            }
//        }
//        if (norm>0)
//        {
//            double deviation = sum/norm;
//            double x = 0.5+0.45*deviation;
//            double dx=0.1;
//            double dy=0.1;
//            auto borderline = GraphicsViewAdapter::PEN_THIN_DARK_GRAY;
//            auto filling = GraphicsViewAdapter::FILL_RED;
//            if (fabs(deviation)<0.15) filling = GraphicsViewAdapter::FILL_GREEN;
//            else if (fabs(deviation)<0.3) filling = GraphicsViewAdapter::FILL_ORANGE;
//            mGraphics->drawFilledRect(x-dx/2,0.85,dx,dy,borderline,filling);
//        }
//    }
}


