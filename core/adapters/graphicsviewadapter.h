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

//============================================================================
//               Adapter for viewing graphical elements
//============================================================================

#ifndef GRAPHICSVIEWADAPTER_H
#define GRAPHICSVIEWADAPTER_H

#include "prerequisites.h"
#include "../drawers/graphicsitem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract base class for implementations rendering graphics.
///
/// The GraphicsViewAdapter provides pure virtual functions to be implemented
/// in the actual GUI. Drawers derived from DrawerBase will use these
/// methods to draw.
///
/// At the core level, all coordinates are relative, that is, they vary
/// between 0 and 1 and refer to the respective panel in which the object
/// is drawn.
///
/// The GraphicsViewAdapter holds a list of GraphicsItem,
/// which allows individual elements of the panel to be redrawn separately.
/// Moreover, it provides a system of flags to mark certain classes of items
/// such as lines, tuning markers etc., and to access them selectively.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN GraphicsViewAdapter
{
public:
    using RoleType = GraphicsItem::RoleType;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Structure holding the coordinates of a single point.
    ///
    /// This structure describes the coordinates of a simple point in 2D.
    /// The coordinates are defined relative to the respective panel
    /// in which the object is drawn and are supposed to lie in the
    /// range between 0 and 1.
    ///////////////////////////////////////////////////////////////////////////

    struct Point
    {
        double x;               ///< Real-valued x coordinate between 0 and 1
        double y;               ///< Real-valued y coordinate between 0 and 1
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Available pen types for drawing.
    ///////////////////////////////////////////////////////////////////////////

    enum PenType
    {
        PEN_THIN_BLACK,
        PEN_MEDIUM_BLACK,
        PEN_THIN_VERY_LIGHT_GRAY,
        PEN_THIN_LIGHT_GRAY,
        PEN_THIN_DARK_GRAY,
        PEN_MEDIUM_LIGHT_GRAY,
        PEN_MEDIUM_DARK_GRAY,
        PEN_MEDIUM_GRAY,
        PEN_THIN_CYAN,
        PEN_MEDIUM_CYAN,
        PEN_MEDIUM_ORANGE,
        PEN_MEDIUM_MAGENTA,
        PEN_THIN_MAGENTA,
        PEN_THIN_RED,
        PEN_MEDIUM_RED,
        PEN_THIN_BLUE,
        PEN_THIN_TRANSPARENT,
        PEN_MEDIUM_DARK_GREEN,
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Available filling colors (e.g. to fill rectangles)
    ///////////////////////////////////////////////////////////////////////////

    enum FillTypes
    {
        FILL_TRANSPARENT,
        FILL_LIGHT_GRAY,
        FILL_RED,
        FILL_ORANGE,
        FILL_GREEN,
        FILL_LIGHT_GREEN,
        FILL_BLUE,
    };

public:

    GraphicsViewAdapter() {}            ///< Empty constructor
    ~GraphicsViewAdapter() {clear();}   ///< Destructor clearing the view

    virtual void clear();               // Clear graphics panel

    ///@{ \name Functions for handling graphics items:

    GraphicItemsList        &getGraphicItems();
    const GraphicItemsList  &getGraphicItems() const;
    GraphicsItem            *getGraphicItem (int keyIndex, RoleType role);
    GraphicsItem            *getGraphicItemByRole (RoleType role);
    GraphicItemsList        getGraphicItems (int keyIndex);
    GraphicItemsList        getGraphicItemsByRole (RoleType role);
    GraphicItemsList        getGraphicItems (int keyIndex, RoleType role);
    ///@}

    ///@{ \name Functions for drawing:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Abstract function: Draw a line.
    /// \param x1 : Starting point x coordinate between 0 and 1
    /// \param y1 : Starting point y coordinate between 0 and 1
    /// \param x2 : Ending point x coordinate between 0 and 1
    /// \param y2 : Ending point y coordinate between 0 and 1
    /// \param pen : The pen to be used
    /// \returns The newly created graphics item or nullptr if out of range.
    ///////////////////////////////////////////////////////////////////////////

    virtual GraphicsItem* drawLine(double x1, double y1,
                          double x2, double y2,
                          PenType pen = PEN_THIN_BLACK) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Abstract function: Draw a chart (polygon).
    ///
    /// This function draws a chart (polygon) of points connected by
    /// straight lines. The points are passed to the function as a vector.
    /// The function is abstract and has to be overridden in the implementation.
    /// \param points : Vector of points.
    /// \param pen : The pen used to draw the polygon.
    /// \returns The newly created graphics item or nullptr if out of range.
    ///////////////////////////////////////////////////////////////////////////

    virtual GraphicsItem* drawChart(const std::vector<Point> &points,
                           PenType pen = PEN_THIN_BLACK) = 0;


    //////////////////////////////////////////////////////////////////////////
    /// \brief Abstract function: Draw a filled rectangle
    ///
    /// This function draws a filled rectangle. It is a pure virtual function
    /// which has to be overridden in the respective implementation.
    /// \param x : Left x coordinate
    /// \param y : Upper y coordinate
    /// \param w : Width of the rectangle
    /// \param h : Height of the rectangle
    /// \param pen : The pen used to draw the border
    /// \param fill : The filling color used to fill the rectangle.
    /// \returns The newly created graphics item or nullptr if out of range.
    ///////////////////////////////////////////////////////////////////////////

    virtual GraphicsItem* drawFilledRect(double x, double y,
                                double w, double h,
                                PenType pen = PEN_THIN_BLACK,
                                FillTypes fill = FILL_RED) = 0;


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
    /// \see StroboscopicViewAdapterForQt
    /// \see GraphicsItem
    /// \param data : Vector of complex numbers, encoding the amplitudes
    /// and the phase shifts of the partials.
    /// \return Pointer to the created GraphicsItem.
    ///////////////////////////////////////////////////////////////////////////

    using ComplexVector = std::vector<std::complex<double>>;
    virtual void drawStroboscope (const ComplexVector &data) = 0;

    ///@}

private:
    GraphicItemsList mGraphicItems; ///< List of all graphic items in the view
};

#endif // GRAPHICSVIEWADAPTER_H
