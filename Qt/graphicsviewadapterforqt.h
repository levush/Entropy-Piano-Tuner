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

#ifndef GRAPHICSVIEWADAPTERFORQT_H
#define GRAPHICSVIEWADAPTERFORQT_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "../core/adapters/graphicsviewadapter.h"

class DrawerBase;

///////////////////////////////////////////////////////////////////////////////
/// \brief Implementation of the GraphicsViewAdapter in Qt using QGraphicsView.
///
/// This class handles the drawing of lines, charts and filled rectangles and
/// will create GraphicsItemForQt as GraphicsItem.
/// It also handles the conversion from relative to absolute coordinates.
///
/// The mSceneRect is used for converting relative to absolute coordinates.
/// It will always be fully displayed in the QGraphicsView.
///////////////////////////////////////////////////////////////////////////////
class GraphicsViewAdapterForQt : public QGraphicsView, public GraphicsViewAdapter
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor.
    /// \param parent : The parent widget
    /// \param drawer : The drawer that draws into this graphics view
    /// \param sceneRect : The scene rectange for QGraphicsView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    GraphicsViewAdapterForQt(QWidget *parent, DrawerBase *drawer, QRectF sceneRect);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///////////////////////////////////////////////////////////////////////////////
    ~GraphicsViewAdapterForQt();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Setter function for the scene rect.
    /// \param rect : The new scene rect of QGraphicsView
    ///
    /// Changing this will also force the drawer to redraw.
    ///////////////////////////////////////////////////////////////////////////////
    void setSceneRect(const QRectF &rect);

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented show event.
    /// \param event : The event
    ///
    /// This function will fit the mSceneRect into the view of the QGraphicsViewer.
    ///////////////////////////////////////////////////////////////////////////////
    void showEvent(QShowEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented resize event.
    /// \param event : The event
    ///
    /// This function will fit the mSceneRect into the view of the QGraphicsViewer.
    ///////////////////////////////////////////////////////////////////////////////
    void resizeEvent(QResizeEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Clears the scene.
    ///
    /// It will clear the QGraphicsScene and call GraphicsViewAdapter::clear()
    ///////////////////////////////////////////////////////////////////////////////
    virtual void clear() override;

    // reimplemented functions for drawing
    virtual GraphicsItem* drawLine(double x1, double y1, double x2, double y2, PenType pen) override;
    virtual GraphicsItem* drawChart(const std::vector<Point> &points, PenType pen) override;
    virtual GraphicsItem* drawFilledRect(double x, double y, double w, double h, PenType pen, FillTypes fill) override;
    virtual GraphicsItem* drawColorBar (double x, double y, double w, double h) override;

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Get a QPen for the given value of PenTypes.
    /// \param penType : The pen type
    /// \param cosmetic : A cosmetic pen does not scale with the QGraphicsView
    /// \return The created QPen
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QPen getPen(PenType penType, bool cosmetic = true) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Create a QBrush for the given value of FillTypes.
    /// \param fill : The fill type
    /// \return The created QBrush
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QBrush getFill(FillTypes fill) const;

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Converting from relative to absolute coordinates.
    /// \param p : A QPointF in relative coordinates
    /// \return The QPointF in absolute coordinates
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QPointF convertRelToAbs(const QPointF &p) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Converting from relative to absolute coordinates.
    /// \param s : A QSizeF in relative coordinates
    /// \return The QSizeF in absolute coordinates
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QSizeF convertRelToAbs(const QSizeF &s) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Converting of a line from relative to absolute coordinates.
    /// \param x1 : The starting point x-coord of the line
    /// \param y1 : The starting point y-coord of the line
    /// \param x2 : The ending point x-coord of the line
    /// \param y2 : The ending point y-coord of the line
    /// \return QLineF in absolute coordinates
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QLineF convertRelToAbsLine(qreal x1, qreal y1, qreal x2, qreal y2) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Converting from absolute to relative coordinates.
    /// \param p : A QPointF in absolute coordinates
    /// \return The QPointF in relative coordinates
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QPointF convertAbsToRel(const QPointF &p) const;

private:
    /// The drawer of this GraphicsViewAdapterForQt
    DrawerBase *mDrawer;

    /// The QGraphicsScene
    QGraphicsScene mScene;

    /// The scene rect
    QRectF mSceneRect;
};

#endif // GRAPHICSVIEWADAPTERFORQT_H
