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

#ifndef CENTRALPLOTFRAME_H
#define CENTRALPLOTFRAME_H

#include <QTouchEvent>
#include <QTime>

#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>

#include "prerequisites.h"

class CentralPlotFrame : public QwtPlot
{
    Q_OBJECT
public:
    static const int FLYING_UPDATE_INTERVALL_IN_MS;

    CentralPlotFrame(int numberOfKeys, int keyOffset);

    double currentTickDistanceInPixel() const;
protected:
    void applyTouchTransform(int final);

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
    QwtPlotZoomer *mNonStackInvisibleZoomer;

    QTime mPlotTimer;


    QList<QTouchEvent::TouchPoint> mTouchPoints;
};

#endif // CENTRALPLOTFRAME_H
