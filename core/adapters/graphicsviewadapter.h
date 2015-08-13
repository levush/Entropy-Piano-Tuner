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

#ifndef GRAPHICSVIEWADAPTER_H
#define GRAPHICSVIEWADAPTER_H

#include <vector>
#include "../drawers/graphicsitem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract base class for implementations rendering graphics.
///
/// The GraphicsViewAdapter provides pure virtual functions to be implemented
/// in the actual GUI. The class DrawerBase will use these methods to draw.
///
/// On the core level, all coordinates are relative, that is, they vary
/// between 0 and 1 and refer to the respective panel in which the object
/// is drawn.
///
/// The GraphicsViewAdapter cooperations closely with the class GraphicsItem,
/// which allows individual elements of the panel to be redrawn separately.
///////////////////////////////////////////////////////////////////////////////

class GraphicsViewAdapter
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Structure holding the coordinates of a single point.
    ///
    /// This structure describes the coordinates of a simple point in 2D.
    /// The coordinates are relative to the respective panel in which the
    /// in which the object is drawn and are supposed to lie in the
    /// range between 0 and 1.
    ///////////////////////////////////////////////////////////////////////////

    struct Point
    {
        double x;               ///< x coordinate between 0 and 1
        double y;               ///< y coordinate between 0 and 1
    };

    /// Available pen types for drawing.
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
        PEN_THIN_RED,
        PEN_MEDIUM_RED,
        PEN_THIN_BLUE,
        PEN_THIN_TRANSPARENT,
        PEN_MEDIUM_DARK_GREEN,
    };

    /// Available filling colors (e.g. to fill rectangles)
    enum FillTypes {
        FILL_TRANSPARENT,
        FILL_LIGHT_GRAY,
        FILL_RED,
        FILL_ORANGE,
        FILL_GREEN,
        FILL_LIGHT_GREEN,
        FILL_BLUE,
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Empty default constructor
    ///////////////////////////////////////////////////////////////////////////
    GraphicsViewAdapter();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor that will delete all graphic items in the view
    ///////////////////////////////////////////////////////////////////////////
    ~GraphicsViewAdapter();
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clear all elements in the respective graphics panel.
    ///
    /// This function clears the graphics view and deletes all items in
    /// the respective panel.
    ///////////////////////////////////////////////////////////////////////////
    virtual void clear();

    ///@{ \name Functions for handling graphics items:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the list of the graphic items.
    ///
    /// Getter function retrieving the complete list of the drawn graphic items.
    /// \returns mGraphicItems : List of graphics item.
    ///////////////////////////////////////////////////////////////////////////
    GraphicItemsList &getGraphicItems() { return mGraphicItems; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant list of the graphic items
    ///
    /// Getter function retreiving the complete list of the drawn graphic items.
    /// \returns mGraphicItems : Constant list of graphics item.
    ///////////////////////////////////////////////////////////////////////////
    const GraphicItemsList &getGraphicItems() const {return mGraphicItems;}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a single graphic item with the given properties.
    ///
    /// Note that the function will return nullptr if no item matched with the
    /// request. The function will fail if more than one item matches.
    /// \see getGraphicItems(int, RoleType)
    /// \param keyIndex : The index of the graphic item
    /// \param role The " user defined role of the item
    /// \return Pointer to the item or nullptr
    ///////////////////////////////////////////////////////////////////////////
    GraphicsItem *getGraphicItem(int keyIndex, RoleType role);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a single graphic item with a given role.
    ///
    /// Note that the function will return nullptr if no item matches with the
    /// requested role. The function will fail if more than one item matches.
    /// \see getGraphicItemsByRole(RoleType)
    /// \param keyIndex : The index of the graphic item
    /// \param role The " user defined role of the item
    /// \return Pointer to the item or nullptr
    ///////////////////////////////////////////////////////////////////////////
    GraphicsItem *getGraphicItemByRole(RoleType role);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a list of graphic items that match with the keyIndex.
    /// \param keyIndex : The index of the graphic item
    /// \return List of items that may be empty
    ///////////////////////////////////////////////////////////////////////////
    GraphicItemsList getGraphicItems(int keyIndex);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a list of graphic items that match with the given role.
    /// \param role : The role of the graphic item
    /// \return List of items that may be empty
    ///////////////////////////////////////////////////////////////////////////
    GraphicItemsList getGraphicItemsByRole(RoleType role);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a list of graphic items.
    ///
    /// The graphic items have to match with the keyIndex and the user defined
    /// role.
    /// \param key : Index The index of the graphic item
    /// \param role : The user defined role of the item
    /// \return List of items that may be empty
    ///////////////////////////////////////////////////////////////////////////
    GraphicItemsList getGraphicItems(int keyIndex, RoleType role);
    ///@}

    ///@{ \name Functions for drawing:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Draw a line.
    /// \param x1 : Starting point x coordinate between 0 and 1
    /// \param y1 : Starting point y coordinate between 0 and 1
    /// \param x2 : Ending point x coordinate between 0 and 1
    /// \param y2 : Ending point y coordinate between 0 and 1
    /// \param pen : The pen to be used
    /// \returns The newly created graphics item or nullptr if out of range
    ///////////////////////////////////////////////////////////////////////////

    virtual GraphicsItem* drawLine(double x1, double y1,
                          double x2, double y2,
                          PenType pen = PEN_THIN_BLACK) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Abstact function: Draw a chart.
    ///
    /// This function draws a chart, i.e., a polygon of points connected by
    /// straight lines. The points are passed to the function as a vector.
    /// The function is abstract and has to be overridden in the implementation.
    /// \param points : Vector of points.
    /// \param pen : The pen used to draw the polygon.
    /// \returns The newly created graphics item or nullptr if out of range
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
    /// \param fill : The filling used to fill the rectangle
    /// \returns The newly created graphics item or nullptr if out of range
    ///////////////////////////////////////////////////////////////////////////

    virtual GraphicsItem* drawFilledRect(double x, double y,
                                double w, double h,
                                PenType pen = PEN_THIN_BLACK,
                                FillTypes fill = FILL_RED) = 0;
    ///@}


private:
    GraphicItemsList mGraphicItems; ///< List of all graphic items in the view
};

#endif // GRAPHICSVIEWADAPTER_H
