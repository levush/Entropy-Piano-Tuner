#ifndef CENTRALPLOTFRAME_H
#define CENTRALPLOTFRAME_H

#include "qwt_plot.h"

class CentralPlotFrame : public QwtPlot
{
    Q_OBJECT
public:
    CentralPlotFrame(int numberOfKeys, int keynumberOfA);

    double currentTickDistanceInPixel() const;
protected:
    virtual void updateLayout() override;
    virtual void showEvent(QShowEvent *e) override;
    virtual void resizeEvent(QResizeEvent *e) override;

signals:
    void keyWidthChanged(double width);
};

#endif // CENTRALPLOTFRAME_H
