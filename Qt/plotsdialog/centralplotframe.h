#ifndef CENTRALPLOTFRAME_H
#define CENTRALPLOTFRAME_H

#include "qwt_plot.h"
#include "qwt_plot_zoomer.h"
#include <QTouchEvent>

class CentralPlotFrame : public QwtPlot
{
    Q_OBJECT
public:
    CentralPlotFrame(int numberOfKeys, int keyOffset);

    double currentTickDistanceInPixel() const;
protected:
    void applyTouchTransform();

    virtual bool event(QEvent *) override;
    virtual bool touchEvent(QTouchEvent *e);
    virtual void updateLayout() override;
    virtual void showEvent(QShowEvent *e) override;
    virtual void resizeEvent(QResizeEvent *e) override;
    virtual void paintEvent(QPaintEvent *e) override;

signals:
    void keyWidthChanged(double width);
    void moveCanvas(int dx, int dy);

public slots:
    void resetView();
    void zoomGoNext();
    void zoomGoPrevious();
    void zoomGoLast();
    void zoomGoFirst();


private:
    const int mNumberOfKeys;
    const int mKeyOffset;
    QwtPlotZoomer *mPlotZoomer;


    QList<QTouchEvent::TouchPoint> mTouchPoints;
};

#endif // CENTRALPLOTFRAME_H
