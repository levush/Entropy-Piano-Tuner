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

#ifndef PLOTSDIALOG_H
#define PLOTSDIALOG_H

#include <QDialog>
#include <QToolButton>
#include <array>

#include "prerequisites.h"

#include "centralplotframe.h"
#include "core/piano/piano.h"

class QwtPlotCurve;
class QwtPlotZoomer;
class PlotToolButton;

class PlotsDialog : public QDialog
{
    Q_OBJECT
public:
    enum Curves {
        CURVE_INHARMONICITY = 0,
        CURVE_RECORDED,
        CURVE_COMPUTED,
        CURVE_TUNED,

        CURVE_COUNT
    };

    PlotsDialog(const Piano &piano, QWidget *parent);
    virtual ~PlotsDialog();

private:
    int getKeyOffset();
    void prepareCurve(Curves curve);

private slots:

    void updateTickWidth(double d);
    void plotToolButtonToggled(bool);

private:
    CentralPlotFrame *mPlot;
    std::array<QwtPlotCurve *, CURVE_COUNT> mCurves;
    std::array<PlotToolButton *, CURVE_COUNT> mPlotToolButtons;

    const Piano &mPiano;
    const Keyboard &mKeyboard;
};

class PlotToolButton : public QToolButton {
    Q_OBJECT
public:
    PlotToolButton(PlotsDialog::Curves curve) :
        mCurve(curve) {
    }
    const PlotsDialog::Curves &getCurve() const {return mCurve;}
private:
    const PlotsDialog::Curves mCurve;
};
#endif // PLOTSDIALOG_H
