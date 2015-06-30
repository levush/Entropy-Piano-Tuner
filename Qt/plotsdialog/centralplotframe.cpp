#include "centralplotframe.h"
#include "keyindexscaledraw.h"
#include "keyindexscaleengine.h"

CentralPlotFrame::CentralPlotFrame(int numberOfKeys, int keynumberOfA)
{
    Q_UNUSED(keynumberOfA);

    setAxisScaleEngine(xBottom, new KeyIndexScaleEngine(numberOfKeys));

    KeyIndexScaleDraw *xScaleDraw = new KeyIndexScaleDraw;
    setAxisScaleDraw(xBottom, xScaleDraw);
    QObject::connect(this, SIGNAL(keyWidthChanged(double)), xScaleDraw, SLOT(setKeyWidth(double)));
}

double CentralPlotFrame::currentTickDistanceInPixel() const {
    return transform(QwtPlot::xBottom, 3) - transform(QwtPlot::xBottom, 2);
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
