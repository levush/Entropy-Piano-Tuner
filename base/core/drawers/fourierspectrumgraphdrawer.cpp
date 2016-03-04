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
//            Drawer for drawing the spectrum with the peak markers
//=============================================================================

#include "fourierspectrumgraphdrawer.h"

#include "../messages/messagenewfftcalculated.h"
#include "../messages/messagefinalkey.h"
#include "../messages/messageprojectfile.h"
#include "../messages/messagemodechanged.h"
#include "../math/mathtools.h"

const double FourierSpectrumGraphDrawer::updateInterval = 1.0;

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor of a FourierSpectrumGraphDrawer
/// \param graphics : Pointer to the graphics adapter
///////////////////////////////////////////////////////////////////////////////

FourierSpectrumGraphDrawer::FourierSpectrumGraphDrawer(GraphicsViewAdapter *graphics)
    : DrawerBase(graphics, updateInterval),
      mConcertPitch(0),
      mKeyNumberOfA4(0),
      mNumberOfKeys(-1),
      mSamplingRate(0),
      mCurrentOperationMode(MODE_IDLE)
{
}


//-----------------------------------------------------------------------------
//                           Draw the spectrum
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Draw the spectrum.
///
/// This function first draws the background grid and then calls the function
/// updateSpectrum() which does the actual drawing of the curve.
///////////////////////////////////////////////////////////////////////////////

void FourierSpectrumGraphDrawer::draw()
{
    // draw gray vertical background grid
    for (int i = 0; i <= mNumberOfKeys; i++)
    {
        double x = static_cast<double>(i)/mNumberOfKeys;
        mGraphics->drawLine (x, 0, x, 1, GraphicsViewAdapter::PEN_THIN_DARK_GRAY);
    }

    // update (draw) the spectrum
    updateSpectrum();
}


//-----------------------------------------------------------------------------
//                           Update the spectrum
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function for updating the red curve showing the spectrum.
///
/// This function actually draws the spectrum. It puts graphic items which
/// are defined by different roles (global,chart,peak). First the old elements
/// are removed (one chart, many peaks). The data used for plotting is held by
/// the shared pointers mKey and mPolygon. Then the peaks are placed and finally
/// the red line is plotted as a polygon.
////////////////////////////////////////////////////////////////////////////////

void FourierSpectrumGraphDrawer::updateSpectrum()
{
    // delete old peaks and chart
    // ==========================================================================

    GraphicsItem *item = mGraphics->getGraphicItemByRole(ROLE_CHART);
    if (item)
    {
        delete item;
        item = nullptr;
    }

    GraphicItemsList peakItems = mGraphics->getGraphicItemsByRole(ROLE_PEAK);
    for (GraphicsItem *peakItem : peakItems)
    {
        delete peakItem;
    }

    // draw new peaks and chart
    // ==========================================================================


    // check if data is available, otherwise quit
    if (not mPolygon) return;

    // lambda function mapping the frequency to an x-coodinate in [0,1]:
    const double a = (mKeyNumberOfA4+0.5)/mNumberOfKeys;
    const double b = 12.0 / mNumberOfKeys / MathTools::LOG2;
    auto xposition = [a,b,this] (double f) { return a + b*log(f/mConcertPitch); };


    // MARK PEAKS AS THIN BLUE DOTS IN THE BACKGROUND (mKey needed)

    const double exponent = 0.3; // magnify low power spectral lines nonlinearly

    if (mCurrentOperationMode == MODE_RECORDING) if (mKey)
    {
        Key::PeakListType peaks = mKey->getPeaks();
        for (auto p : peaks)
        {
            double x = xposition(p.first);

            // Search for the corresponding peaks in the polygon:
            auto pos1 = mPolygon->begin();
            while (pos1!=mPolygon->end() and pos1->first < p.first*0.995) pos1++;
            auto pos2 = pos1;
            while (pos2!=mPolygon->end() and pos2->first < p.first*1.005) pos2++;
            auto comp = [] (const std::pair<double,double> &a, const std::pair<double,double> &b)
                { return a.second < b.second; };
            auto maxelem = std::max_element(pos1,pos2, comp);
            if (maxelem != mPolygon->end())
            {
                double y = 1-0.95*pow(maxelem->second, exponent);
                const double dx=0.003;
                const double dy=0.02;
                item = mGraphics->drawFilledRect(x-dx/2,y-dy/2,dx,dy,
                                                 GraphicsViewAdapter::PEN_THIN_TRANSPARENT,
                                                 GraphicsViewAdapter::FILL_BLUE);
                if (item) item->setItemRole(ROLE_PEAK);
            }
        }
    }

    // DRAW SPECTRUM

    std::vector<GraphicsViewAdapter::Point> points;
    EptAssert(mConcertPitch > 0,"concert pitch should be positive");
    EptAssert(mNumberOfKeys > 0,"invalid number of keys");

    for (auto &p : *mPolygon)
    {
        double x=xposition(p.first);
        if (x>=0 and x<=1) points.push_back({x, 1-0.95*pow(p.second,exponent)});
    }
    item = mGraphics->drawChart(points, GraphicsViewAdapter::PEN_THIN_RED);
    if (item) item->setItemRole(ROLE_CHART);
}


//-----------------------------------------------------------------------------
//                           Clear the spectrum
//-----------------------------------------------------------------------------

/// \brief Clear the spectrum

void FourierSpectrumGraphDrawer::clear()
{ DrawerBase::clear(); }


//-----------------------------------------------------------------------------
//                             Message handler
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Message dispatcher for FourierSpectrumGraphDrawer.
/// \param m : Message pointer
///////////////////////////////////////////////////////////////////////////////

void FourierSpectrumGraphDrawer::handleMessage(MessagePtr m)
{
    switch (m->getType())
    {
    case Message::MSG_PROJECT_FILE:
    {
        // Update local parameter
        auto mpf(std::static_pointer_cast<MessageProjectFile>(m));
        mConcertPitch = mpf->getPiano().getConcertPitch();
        mNumberOfKeys = mpf->getPiano().getKeyboard().getNumberOfKeys();
        mKeyNumberOfA4 = mpf->getPiano().getKeyboard().getKeyNumberOfA4();
        mPolygon.reset();
        redraw(true);
        break;
    }
    case Message::MSG_MODE_CHANGED:
    {
        // Update operation mode
        auto mmc(std::static_pointer_cast<MessageModeChanged>(m));
        mCurrentOperationMode = mmc->getMode();
        break;
    }
    case Message::MSG_NEW_FFT_CALCULATED:
    {
        // Update mPolygon
        auto mnfc(std::static_pointer_cast<MessageNewFFTCalculated>(m));
        if (mnfc->hasError())
        {
            mPolygon.reset();
        }
        else
        {
            mPolygon = mnfc->getPolygon();
            mSamplingRate = mnfc->getData()->samplingRate;
        }
        // Reset mKey
        mKey.reset();
        // Plot curve
        if (requestRedraw(mnfc->isFinal())) updateSpectrum();
        break;
    }
    case Message::MSG_CLEAR_RECORDING:
        // Clear everything, cancel spectrum
        mPolygon.reset();
        mKey.reset();
        redraw(true);
        break;
    case Message::MSG_CALCULATION_PROGRESS:
        redraw();
        break;
    case Message::MSG_FINAL_KEY:
    {
        // Update mKey
        auto mnfc(std::static_pointer_cast<MessageFinalKey>(m));
        mKey = mnfc->getFinalKey();
        if (requestRedraw(true)) updateSpectrum();
        break;
    }
    default:
        break;
    }
}

