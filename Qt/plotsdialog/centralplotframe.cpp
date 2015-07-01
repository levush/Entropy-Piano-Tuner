#include "centralplotframe.h"
#include <QGestureEvent>
#include <QPinchGesture>
#include <QApplication>
#include "qwt_plot_panner.h"
#include "qwt_plot_magnifier.h"
#include "qwt_plot_zoomer.h"
#include "core/system/log.h"
#include "keyindexscaledraw.h"
#include "keyindexscaleengine.h"

CentralPlotFrame::CentralPlotFrame(int numberOfKeys, int keynumberOfA) :
    mNumberOfKeys(numberOfKeys),
    mKeynumberOfA(keynumberOfA)
{
    Q_UNUSED(keynumberOfA);

    setAxisScaleEngine(xBottom, new KeyIndexScaleEngine(numberOfKeys));

    KeyIndexScaleDraw *xScaleDraw = new KeyIndexScaleDraw;
    setAxisScaleDraw(xBottom, xScaleDraw);
    QObject::connect(this, SIGNAL(keyWidthChanged(double)), xScaleDraw, SLOT(setKeyWidth(double)));

    // panning with the left mouse button
    QwtPlotPanner *panner = new QwtPlotPanner(canvas());
    panner->setMouseButton(Qt::MidButton);
    QObject::connect(this, SIGNAL(moveCanvas(int,int)), panner, SLOT(moveCanvas(int,int)));

    // zoom in/out with the wheel
    //( void ) new QwtPlotMagnifier( canvas() );

    mPlotZoomer = new QwtPlotZoomer(canvas());
    mPlotZoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ShiftModifier);


    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
}

double CentralPlotFrame::currentTickDistanceInPixel() const {
    return transform(QwtPlot::xBottom, 3) - transform(QwtPlot::xBottom, 2);
}

void CentralPlotFrame::applyTouchTransform() {
    QStack< QRectF > zoomStack = mPlotZoomer->zoomStack();
    QRectF newRect;

    auto invTransform = [this](const QPointF &p) {
        return QPointF(this->invTransform(xBottom, p.x()),
                       this->invTransform(yLeft, p.y()));
    };

    const QwtInterval xInterval(axisInterval(xBottom));
    const QwtInterval yInterval(axisInterval(yLeft));
    const QPointF topLeft(xInterval.minValue(), yInterval.maxValue());
    const QPointF botRight(xInterval.maxValue(), yInterval.minValue());

    const QList<QTouchEvent::TouchPoint> &points(mTouchPoints);
    if (points.size() == 1) {
        const QTouchEvent::TouchPoint &p(points.first());
        QPointF d = invTransform(p.pos()) - invTransform(p.lastPos());
        newRect = QRectF(topLeft, botRight);
        newRect.translate(-d);
    } else if (points.size() == 2) {
        const QTouchEvent::TouchPoint &p1(points.first());
        const QTouchEvent::TouchPoint &p2(points.last());

        auto mult = [](const QPointF &p1, const QPointF &p2) {
            return QPointF(p1.x() * p2.x(), p1.y() * p2.y());
        };
        auto div = [](const QPointF &p1, const QPointF &p2) {
            return QPointF(p1.x() / p2.x(), p1.y() / p2.y());
        };

        const QPointF p1Last = invTransform(p1.lastPos());
        const QPointF p2Last = invTransform(p2.lastPos());
        const QPointF p1Curr = invTransform(p1.pos());
        const QPointF p2Curr = invTransform(p2.pos());

        const QPointF deltaPrev = p1Last - p2Last;
        const QPointF deltaCurr = p1Curr - p2Curr;

        const QPointF scale(div(deltaCurr, deltaPrev));
        const QPointF offset(p1Curr - mult(scale, p1Last));

        auto invPointTransform = [&scale, &offset, div](const QPointF &p) {
            return div(p - offset, scale);
        };

        newRect.setTopLeft(invPointTransform(topLeft));
        newRect.setBottomRight(invPointTransform(botRight));
    }

    if (newRect.isNull() == false) {
        zoomStack << newRect.normalized();
        mPlotZoomer->setZoomStack(zoomStack);
    }

    mTouchPoints.clear();
}

bool CentralPlotFrame::event(QEvent *e) {
    if (dynamic_cast<QTouchEvent*>(e)) {
        return touchEvent(static_cast<QTouchEvent *>(e));
    }
    return QwtPlot::event(e);
}

bool CentralPlotFrame::touchEvent(QTouchEvent *e) {
    QList<QTouchEvent::TouchPoint> points(e->touchPoints());
    for (auto it = points.begin(); it != points.end(); ) {
        if (it->state() == Qt::TouchPointReleased || it->state() == Qt::TouchPointPressed) {
            it = points.erase(it);
        } else {
            it++;
        }
    }
    if (points.size() != mTouchPoints.size()) {
        applyTouchTransform();
        mTouchPoints = points;
    } else {
        for (int i = 0; i < points.size(); ++i) {
            mTouchPoints[i].setPos(points[i].pos());
        }
    }

   return true;
}

void CentralPlotFrame::updateLayout() {
    emit keyWidthChanged(currentTickDistanceInPixel());
    QwtPlot::updateLayout();
}

void CentralPlotFrame::showEvent(QShowEvent *e) {
    emit keyWidthChanged(currentTickDistanceInPixel());
    QwtPlot::showEvent(e);
}

void CentralPlotFrame::resizeEvent(QResizeEvent *e) {
    QwtPlot::resizeEvent(e);
    emit keyWidthChanged(currentTickDistanceInPixel());
}

void CentralPlotFrame::paintEvent(QPaintEvent *e) {
    QwtPlot::paintEvent(e);
}

void CentralPlotFrame::resetView() {
    setAxisAutoScale(QwtPlot::yLeft);
    setAxisScale(QwtPlot::xBottom, 0, mNumberOfKeys, 12);
    replot();
}

void CentralPlotFrame::zoomGoFirst() {
    mPlotZoomer->zoom(mPlotZoomer->maxStackDepth());
}

void CentralPlotFrame::zoomGoLast() {
    mPlotZoomer->zoom(-mPlotZoomer->maxStackDepth());
}

void CentralPlotFrame::zoomGoNext() {
    mPlotZoomer->zoom(1);
}

void CentralPlotFrame::zoomGoPrevious() {
    mPlotZoomer->zoom(-1);
}
