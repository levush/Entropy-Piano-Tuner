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

#include "graphicsviewadapterforqt.h"

#include "graphicsitemforqt.h"
#include "../core/drawers/drawerbase.h"
#include "../core/system/eptexception.h"

GraphicsViewAdapterForQt::GraphicsViewAdapterForQt(QWidget *parent, DrawerBase *drawer, QRectF sceneRect)
    : QGraphicsView(parent),
      GraphicsViewAdapter(),
      mDrawer(drawer),
      mScene(sceneRect),
      mSceneRect(sceneRect) {
    setScene(&mScene);

    // antialiasing for nice lines
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
}

GraphicsViewAdapterForQt::~GraphicsViewAdapterForQt()
{
    // upon delete the QGraphicItems may not be deleted, this was already dont by the QGraphicsView
    for (GraphicsItem *item : getGraphicItems()) {
        // so set them to 0
        static_cast<GraphicsItemForQt*>(item)->setItem(nullptr);
    }
}

void GraphicsViewAdapterForQt::setSceneRect(const QRectF &rect) {
    mSceneRect = rect;
    // force redraw
    mDrawer->redraw(true);
}

void GraphicsViewAdapterForQt::showEvent(QShowEvent *event) {
    fitInView(mScene.sceneRect());
    QGraphicsView::showEvent(event);
}

void GraphicsViewAdapterForQt::resizeEvent(QResizeEvent *event) {
    fitInView(mScene.sceneRect());
    QGraphicsView::resizeEvent(event);
}

void GraphicsViewAdapterForQt::clear() {
    GraphicsViewAdapter::clear();
    mScene.clear();
}

GraphicsItem *GraphicsViewAdapterForQt::drawLine(double x1, double y1, double x2, double y2, PenType pen) {
    if (x1 < 0 || x1 > 1) {return nullptr;}
    if (y1 < 0 || y1 > 1) {return nullptr;}
    if (x2 < 0 || x2 > 1) {return nullptr;}
    if (y2 < 0 || y2 > 1) {return nullptr;}
    QGraphicsLineItem *line(mScene.addLine(convertRelToAbsLine(0, 0, x2 - x1, y2 - y1), getPen(pen)));
    line->setPos(convertRelToAbs(QPointF(x1, y1)));
    return new GraphicsItemForQt(this, line);
}

GraphicsItem *GraphicsViewAdapterForQt::drawChart(const std::vector<Point> &points, PenType pen) {
    if (points.size() <= 1) {
        return nullptr;
    }

    QPainterPath path;
    size_t firstValidPointIndex = 0;
    for (; firstValidPointIndex < points.size(); firstValidPointIndex++) {
        if (points[firstValidPointIndex].x < 0 || points[firstValidPointIndex].x > 1) {continue;}
        if (points[firstValidPointIndex].y < 0 || points[firstValidPointIndex].y > 1) {continue;}
        path.moveTo(convertRelToAbs(QPointF(points[firstValidPointIndex].x, points[firstValidPointIndex].y)));
        break;
    }
    for (size_t i = firstValidPointIndex; i < points.size(); i++) {
        if (points[i].x < 0 || points[i].x > 1) {continue;}
        if (points[i].y < 0 || points[i].y > 1) {continue;}
        path.lineTo(convertRelToAbs(QPointF(points[i].x, points[i].y)));
    }

    return new GraphicsItemForQt(this, mScene.addPath(path, getPen(pen)));
}

GraphicsItem* GraphicsViewAdapterForQt::drawFilledRect(double x, double y, double w, double h, PenType pen, FillTypes fill)
{
    if (x < 0 || x > 1) {return nullptr;}
    if (y < 0 || y > 1) {return nullptr;}
    if (x + w > 1) {return nullptr;}
    if (y + h > 1) {return nullptr;}
    return new GraphicsItemForQt(this,
                                 mScene.addRect(QRectF(convertRelToAbs(QPointF(x, y)),
                                                       convertRelToAbs(QSizeF(w, h))),
                                                getPen(pen),
                                                getFill(fill)));
}

GraphicsItem* GraphicsViewAdapterForQt::drawStroboscope (const ComplexVector &data)
{
    qreal W = mSceneRect.width();
    qreal H = mSceneRect.height();
    QImage image (W, H, QImage::Format_RGB32);

    const int N = data.size();
    std::vector<double> phase(N),saturation(N), value(N);
    for (int n=0; n<N; n++)
    {
        phase[n]      = std::arg(data[n])/M_PI/2 + 0.5;
//        saturation[n] = std::abs(data[n]);
        saturation[n] = 1;
        value[n]      = 1;
    }

    std::vector<std::vector<QColor>> colors(W);
    for (int x=0; x<W; x++)
    {
        colors[x].resize(N+2);
        colors[x][0]=colors[x][N+1]=Qt::black;
        for (int n=0; n<N; n++)
        {
            double xc = 1.0*(n+1)*(x/W+phase[n]/(n+1));
            double hue = xc - floor(xc);
            colors[x][n+1].setHsvF(hue,saturation[n],value[n],1);
        }
    }

    for (int y=0; y<H; y++)
    {
        // According to the Qt manual, fast access to the pixels requires to
        // get the pointer to a whole horizontal line (function scalinline).
        auto line = image.scanLine(y);

        double z = 1.0*y*(N+1)/H;
        int index = static_cast<int>(z);
        EptAssert(index >= 0 and index <= N,"The index has to be within the array");
        double a = z-index;
        for (int x=0; x<W; x++)
        {
            QColor C1 = colors[x][index];
            QColor C2 = colors[x][index+1];
            const double margin = 0.05*W;
            double brightness = std::min(std::min(1.0,x/margin),(W-x)/margin);
            double lower = brightness*(1-a);
            double upper = brightness*a;
            line[4*x]   = lower*C1.red()   + upper*C2.red();
            line[4*x+1] = lower*C1.green() + upper*C2.green();
            line[4*x+2] = lower*C1.blue()  + upper*C2.blue();
            line[4*x+3] = 0;
        }
    }
    QPixmap pixmap(W,H);
    pixmap.convertFromImage(image);
    return new GraphicsItemForQt(this,mScene.addPixmap(pixmap));
}


QPen GraphicsViewAdapterForQt::getPen(PenType penType, bool cosmetic) const {
    QPen pen;

    switch (penType) {
    case PEN_THIN_BLACK:
        pen = QPen(QBrush(Qt::black), 1);
        break;
    case PEN_MEDIUM_BLACK:
        pen = QPen(QBrush(Qt::black), 2);
        break;
    case PEN_THIN_VERY_LIGHT_GRAY:
        pen = QPen(QBrush(QColor(220,220,220)), 1);
        break;
    case PEN_THIN_LIGHT_GRAY:
        pen = QPen(QBrush(Qt::lightGray), 1);
        break;
    case PEN_MEDIUM_LIGHT_GRAY:
        pen = QPen(QBrush(Qt::lightGray), 2);
        break;
    case PEN_THIN_DARK_GRAY:
        pen = QPen(QBrush(Qt::darkGray), 1);
        break;
    case PEN_MEDIUM_DARK_GRAY:
        pen = QPen(QBrush(Qt::darkGray), 2);
        break;
    case PEN_MEDIUM_GRAY:
        pen = QPen(QBrush(Qt::gray), 2);
        break;
    case PEN_MEDIUM_DARK_GREEN:
        pen = QPen(QBrush(Qt::darkGreen), 2);
        break;
    case PEN_THIN_CYAN:
        pen = QPen(QBrush(Qt::cyan), 1);
        break;
    case PEN_MEDIUM_CYAN:
        pen = QPen(QBrush(Qt::cyan), 2);
        break;
    case PEN_MEDIUM_ORANGE:
        pen = QPen(QBrush(QColor(0xff,0x66,0)), 2);
        break;
    case PEN_MEDIUM_MAGENTA:
        pen = QPen(QBrush(QColor(0xff,0,0xff)), 2);
        break;
    case PEN_THIN_MAGENTA:
        pen = QPen(QBrush(QColor(0xff,0,0xff)), 1);
        break;
    case PEN_THIN_RED:
        pen = QPen(QBrush(Qt::red), 1);
        break;
    case PEN_MEDIUM_RED:
        pen = QPen(QBrush(Qt::red), 2);
        break;
    case PEN_THIN_BLUE:
        pen = QPen(QBrush(Qt::blue), 1);
        break;
    case PEN_THIN_TRANSPARENT:
        pen = QPen(QBrush(Qt::transparent), 1);
        break;
    }

    pen.setCosmetic(cosmetic);

    return pen;
}

QBrush GraphicsViewAdapterForQt::getFill(FillTypes fill) const {
    switch (fill) {
    case FILL_TRANSPARENT:
        return QBrush(Qt::transparent);
    case FILL_LIGHT_GRAY:
        return QBrush(Qt::lightGray);
    case FILL_RED:
        return QBrush(Qt::red);
    case FILL_ORANGE:
        return QBrush(QColor(255,165,0));
    case FILL_GREEN:
        return QBrush(Qt::green);
    case FILL_LIGHT_GREEN:
        return QBrush(QColor(200,255,200));
    case FILL_BLUE:
        return QBrush(Qt::blue);
    }

    return QBrush();
}

QPointF GraphicsViewAdapterForQt::convertRelToAbs(const QPointF &p) const {
    return QPointF(p.x() * mSceneRect.width(), p.y() * mSceneRect.height());
}

QSizeF GraphicsViewAdapterForQt::convertRelToAbs(const QSizeF &s) const {
    return QSizeF(s.width() * mSceneRect.width(), s.height() * mSceneRect.height());
}
QLineF GraphicsViewAdapterForQt::convertRelToAbsLine(qreal x1, qreal y1, qreal x2, qreal y2) const {
    return QLineF(x1 * mSceneRect.width(),
                 y1 * mSceneRect.height(),
                 x2 * mSceneRect.width(),
                 y2 * mSceneRect.height());
}

QPointF GraphicsViewAdapterForQt::convertAbsToRel(const QPointF &p) const {
    return QPointF(p.x() / mSceneRect.width(), p.y() / mSceneRect.height());
}
