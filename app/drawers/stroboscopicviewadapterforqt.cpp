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
//            Graphics implementation of the stroboscope for Qt
//=============================================================================

#include "stroboscopicviewadapterforqt.h"

#include <QGraphicsPixmapItem>

#include "../core/math/mathtools.h"
#include "../core/system/eptexception.h"

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor with no stroboscope in the beginning.
/// \param parent : The parent widget
/// \param drawer : The drawer that draws into this graphics view
/// \param sceneRect : The scene rectange for QGraphicsView
///
/// This function calls the constructor of the base class and resets the
/// pointer to the stroboscope, meaning that the stroboscope is not yet active.
///////////////////////////////////////////////////////////////////////////////

StroboscopicViewAdapterForQt::StroboscopicViewAdapterForQt(QWidget *parent,
                                                           DrawerBase *drawer,
                                                           QRectF sceneRect) :
    GraphicsViewAdapterForQt(parent,drawer,sceneRect),
    mStroboscopeItem(nullptr)
{
}


//-----------------------------------------------------------------------------
//                                Destructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Destructor, deleting the stroboscope if existent
///////////////////////////////////////////////////////////////////////////////

StroboscopicViewAdapterForQt::~StroboscopicViewAdapterForQt()
{
    if (mStroboscopeItem)
    {
        delete mStroboscopeItem;
        mStroboscopeItem = nullptr;
    }
}


//-----------------------------------------------------------------------------
//                                Clear view
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Clear view of stroboscope and spectral indicator
///////////////////////////////////////////////////////////////////////////////

void StroboscopicViewAdapterForQt::clear()
{
    GraphicsViewAdapterForQt::clear();
    // calling clear of the base class already deletes mStroboscopeItem
    // Therefore, we do not have a delete here.
    mStroboscopeItem = nullptr;
}


//-----------------------------------------------------------------------------
//                           Draw the stroboscope
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief StroboscopicViewAdapterForQt::drawStroboscope
///
/// This function depends on a vector of complex numbers.
/// It draws rainbow-like strips with an
/// intensity proportional to the absolute values of the complex numbers
/// and a horizontal shift proportional to the phase of the complex numbers.
/// This function is time-critical and uses direct memory access.
/// \param data : Vector of complex numbers holding the stroboscopic information
///////////////////////////////////////////////////////////////////////////////

void StroboscopicViewAdapterForQt::drawStroboscope (const ComplexVector &data)
{
    // if no stroboscope exists we clear the scene and create it
    if (not mStroboscopeItem)
    {
        clear();
        mStroboscopeItem = new QGraphicsPixmapItem;
        mScene.addItem(mStroboscopeItem);
    }

    // get the pixel size of the scene. This is not identical with the
    // actual pixel size shown on the screen (it is typically 200x200)
    qreal W = mSceneRect.width();
    qreal H = mSceneRect.height();

    // Create an image of the same size
    QImage image (W, H, QImage::Format_RGB32);

    // loop over all vector entries (partials)
    const int N = static_cast<int>(data.size());
    if (N>0)
    {
        // First derive the graphical information from the complex numbers
        std::vector<double> phase(N),saturation(N), value(N);
        for (int n=0; n<N; n++)
        {
            phase[n]      = std::arg(data[n])/MathTools::TWO_PI + 0.5;
            saturation[n] = pow(std::min(1.0,25.0/N*std::abs(data[n])*(n+1)),0.3);
            value[n]      = saturation[n];
        }

        // Then for each partial generate a sequence of rgb values.
        // These values are stored in a NxW matrix organized as a vector:
        std::vector<uint32_t> rgbColors(N*W);
        QColor color;
        const int xMargin = W/100;

        // For keys with 4 partials and more the first partial covers
        // the phase 2pi over the whole display. For very high keys
        // the wave number is increased to 4pi, 6pi etc
        const double wavenumber = std::max(1,5-N);

        // Before drawing the colors are computed
        for (int n=0; n<N; n++) for (int x=0; x<W; x++)
        {
            // The margins are black
            if (x<xMargin or x>=W-xMargin) rgbColors[W*n+x]=0;
            else // otherwise they are set to a phase-shifted rainbow
            {
                double xc = (wavenumber*x/W*(n+1)+phase[n]);
                double hue = xc - floor(xc);
                // This function generates a rainbow-like color scale:
                color.setHsvF(hue,saturation[n],value[n],1);
                rgbColors[W*n+x] = static_cast<uint32_t>(color.rgb());
            }
        }

        // Depending on the number of the partials N, these color vectors
        // are now copied into several strips of a certain stripWidth.
        // Since this is time-critical this is done via memcpy.
        // Black margins are added using memset.
        int yMargin = H/20;
        int stripWidth = (H-2*yMargin)/N;
        int actualMargin = (H-N*stripWidth)/2;
        int upperBound = N*stripWidth+actualMargin;

        // Main drawing loop
        for (int y=0; y<H; ++y)
        {
            auto line = image.scanLine(H-1-y);
            if (y>=actualMargin and y<upperBound)
            {
                int n = (y-actualMargin)/stripWidth;
                memcpy(line,&rgbColors[W*n],W*4);
            }
            else memset(line,0,W*4);
        }
    }
    else image.fill(0); // if N=0 return black rectangle

    // Convert the image to a pixmap:
    QPixmap pixmap(W,H);
    pixmap.convertFromImage(image);

    // Copy the pixmap to the stroboscope item.
    // This method is much faster than adding a new object
    // to the scene over and over again.
    if (mStroboscopeItem) mStroboscopeItem->setPixmap(pixmap);
}
