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

#include "plotsdialog.h"
#include <cmath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QToolBar>
#include <QToolButton>
#include <QDialogButtonBox>
#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_zoomer.h>
#include "core/system/eptexception.h"
#include "qtconfig.h"
#include "displaysize.h"
#include "keyindexscaleengine.h"
#include "keyindexscaledraw.h"

PlotsDialog::PlotsDialog(const Piano &piano, QWidget *parent) :
    QDialog(parent, Qt::Window),
    mPiano(piano),
    mKeyboard(piano.getKeyboard())
{
    std::fill(mCurves.begin(), mCurves.end(), nullptr);
    const bool vNav(DisplaySizeDefines::getSingleton()->showPlotNavigationVertical());

    CentralPlotFrame *plot = new CentralPlotFrame(mKeyboard.getNumberOfKeys(), getKeyOffset());

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    setLayout(mainLayout);

    QLayout *plotLayout = mainLayout;

    QToolBar *toolBar = new QToolBar;
    int iconSize = DisplaySizeDefines::getSingleton()->getMediumIconSize();
    toolBar->setIconSize(QSize(iconSize, iconSize));
    mainLayout->addWidget(toolBar);

    QHBoxLayout *barLayout = new QHBoxLayout;
    barLayout->setMargin(0);
    mainLayout->addLayout(barLayout);

    QToolBar *navBar = toolBar;
    if (vNav) {
        navBar = new QToolBar;
        navBar->setIconSize(QSize(iconSize, iconSize));
        navBar->setOrientation(Qt::Vertical);

        QHBoxLayout *vTbExtraLayout = new QHBoxLayout;
        vTbExtraLayout->setMargin(0);
        mainLayout->addLayout(vTbExtraLayout);
        vTbExtraLayout->addWidget(navBar);

        plotLayout = vTbExtraLayout;
    }

    auto makeColorIcon = [](QColor c) {
        int size = DisplaySizeDefines::getSingleton()->getSmallIconSize() / 2;
        QPixmap p(size, size);
        p.fill(c);
        return QIcon(p);
    };

    auto makeToolButton = [&toolBar, &makeColorIcon, this](QColor c, QString title, Curves curve) {
        PlotToolButton *b = new PlotToolButton(curve);
        toolBar->addWidget(b);
        b->setFocusPolicy(Qt::NoFocus);
        b->setCheckable(true);
        b->setIcon(makeColorIcon(c));
        b->setText(title);
        b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        QObject::connect(b, SIGNAL(toggled(bool)), this, SLOT(plotToolButtonToggled(bool)));

        this->mPlotToolButtons[curve] = b;

        return b;
    };

    navBar->addAction(QIcon(":/media/icons/view-fullscreen.png"), tr("Reset view"), plot, SLOT(resetView()));
    navBar->addAction(QIcon(":/media/icons/go-first.png"), tr("Go first"), plot, SLOT(zoomGoFirst()));
    navBar->addAction(QIcon(":/media/icons/go-previous.png"), tr("Go previous"), plot, SLOT(zoomGoPrevious()));
    navBar->addAction(QIcon(":/media/icons/go-next.png"), tr("Go next"), plot, SLOT(zoomGoNext()));
    navBar->addAction(QIcon(":/media/icons/go-last.png"), tr("Go last"), plot, SLOT(zoomGoLast()));

    if (!vNav) {
        toolBar->addSeparator();
    }
    if (DisplaySizeDefines::getSingleton()->abbrevPlotLabels()) {
        makeToolButton(Qt::darkGreen, tr("Inh."), CURVE_INHARMONICITY);
        makeToolButton(Qt::red, tr("Rec."), CURVE_RECORDED);
        makeToolButton(Qt::blue, tr("Comp."), CURVE_COMPUTED);
        makeToolButton(Qt::green, tr("Tun."), CURVE_TUNED);
    } else {
        makeToolButton(Qt::darkGreen, tr("Inharmonicity"), CURVE_INHARMONICITY);
        makeToolButton(Qt::red, tr("Recorded"), CURVE_RECORDED);
        makeToolButton(Qt::blue, tr("Computed"), CURVE_COMPUTED);
        makeToolButton(Qt::green, tr("Tuned"), CURVE_TUNED);
    }

    toolBar->addSeparator();

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    toolBar->addWidget(buttonBox);
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


    mPlot = plot;
    plot->setCanvasBackground( Qt::white );
    // plot->insertLegend( new QwtLegend() );
    plot->setAxisTitle(QwtPlot::xBottom, tr("Key index"));
    plot->resetView();

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMinorPen(Qt::darkGray, 1);
    grid->setMajorPen(Qt::black, 1);
    grid->enableXMin(true);
    grid->attach( plot );
    grid->setZ(-200);

    QwtPlotMarker *marker = new QwtPlotMarker;
    marker->setLinePen(Qt::black, 2);
    marker->setLineStyle(QwtPlotMarker::HLine);
    marker->setYValue(0);
    marker->attach(plot);
    marker->setZ(-100);

    plotLayout->addWidget(plot);

    for (int i = 0; i < CURVE_COUNT; i++) {
        prepareCurve(static_cast<Curves>(i));
    }

    //QwtPlotPicker *pickers = new QwtPlotPicker(plot->canvas());
    //pickers->setTrackerMode(QwtPlotPicker::AlwaysOn);

    QObject::connect(mPlot, SIGNAL(keyWidthChanged(double)), this, SLOT(updateTickWidth(double)));

    // show last shown charts
    QSettings s;
    for (int i = 0; i < CURVE_COUNT; i++) {
        bool defaultShown = i == CURVE_RECORDED || i == CURVE_COMPUTED;
        mPlotToolButtons[i]->setChecked(s.value(QString("plots/curve%1").arg(i), defaultShown).toBool());
    }

#if CONFIG_DIALOG_SIZE == 1
    // load stored geometry
    restoreGeometry(s.value("plots/geometry").toByteArray());
    setWindowState(static_cast<Qt::WindowState>(s.value("plots/windowState").toInt()));
#else
    SHOW_DIALOG(this);
#endif

    mPlot->updateAxes();
}

PlotsDialog::~PlotsDialog() {
    // store shown curves
    QSettings s;
    for (int i = 0; i < CURVE_COUNT; ++i) {
        s.setValue(QString("plots/curve%1").arg(i), mCurves[i]->isVisible());
    }

#if CONFIG_DIALOG_SIZE == 1
    // load stored geometry
    s.setValue("plots/geometry", saveGeometry());
    s.setValue("plots/windowState", (int)windowState());
#endif

    for (auto *curve : mCurves) {
        if (curve) {delete curve;}
    }

}

void PlotsDialog::updateTickWidth(double d) {
    for (QwtPlotCurve *c : mCurves) {
        if (c) {
            QwtSymbol *s = const_cast<QwtSymbol*>(c->symbol());
            if (s) {
                s->setSize(QSize(d, 1));
            }
        }
    }
}

int PlotsDialog::getKeyOffset()
{
    return 48 - mKeyboard.getKeyNumberOfA4();
}

void PlotsDialog::prepareCurve(Curves curve) {
    if (mCurves[curve]) {delete mCurves[curve];}

    QwtPlotCurve *c = mCurves[curve] = new QwtPlotCurve;

    auto cents = [] (double ratio) { return 1200.0 * log(ratio)/log(2); };

    if (curve == CURVE_INHARMONICITY) {
        c->setStyle(QwtPlotCurve::NoCurve);
        c->setSymbol(new QwtSymbol(QwtSymbol::HLine, QBrush(), QPen(Qt::darkGreen, 2), QSize(mPlot->currentTickDistanceInPixel(), 1)));


        QPolygonF points;
        for (int i = 0; i < mKeyboard.getNumberOfKeys(); ++i) {
            if (mKeyboard[i].getMeasuredInharmonicity() > 0) {
                points << QPointF(i + 0.5 + getKeyOffset(), mKeyboard[i].getMeasuredInharmonicity());
            }
        }
        c->setSamples( points );
    } else if (curve == CURVE_RECORDED) {
        c->setStyle(QwtPlotCurve::NoCurve);
        c->setSymbol(new QwtSymbol(QwtSymbol::HLine, QBrush(), QPen(Qt::red, 2), QSize(mPlot->currentTickDistanceInPixel(), 1)));

        int keyA4 = mPiano.getKeyboard().getKeyNumberOfA4();
        double fA4 = mPiano.getKey(keyA4).getRecordedFrequency();

        QPolygonF points;
        for (int i = 0; i < mKeyboard.getNumberOfKeys(); ++i) {
            points << QPointF(i + 0.5 + getKeyOffset(), cents(mKeyboard[i].getRecordedFrequency() / mPiano.getEqualTempFrequency(i, 0, fA4)));
        }
        c->setSamples(points);
    } else if (curve == CURVE_COMPUTED) {
        c->setStyle(QwtPlotCurve::NoCurve);
        c->setSymbol(new QwtSymbol(QwtSymbol::HLine, QBrush(), QPen(Qt::blue, 2), QSize(mPlot->currentTickDistanceInPixel(), 1)));

        QPolygonF points;
        for (int i = 0; i < mKeyboard.getNumberOfKeys(); ++i) {
            points << QPointF(i + 0.5 + getKeyOffset(), cents(mKeyboard[i].getComputedFrequency() / mPiano.getDefiningTempFrequency(i,0,440)));
        }
        c->setSamples(points);
    } else if (curve == CURVE_TUNED) {
        c->setStyle(QwtPlotCurve::NoCurve);
        c->setSymbol(new QwtSymbol(QwtSymbol::HLine, QBrush(), QPen(Qt::green, 2), QSize(mPlot->currentTickDistanceInPixel(), 1)));

        QPolygonF points;
        for (int i = 0; i < mKeyboard.getNumberOfKeys(); ++i) {
            points << QPointF(i + 0.5 + getKeyOffset(), cents(mKeyboard[i].getTunedFrequency() / mPiano.getEqualTempFrequency(i)));
        }
        c->setSamples(points);
    }


    c->attach(mPlot);
    c->hide();
}

void PlotsDialog::plotToolButtonToggled(bool b) {
    const PlotToolButton *tb = qobject_cast<const PlotToolButton*>(sender());
    EptAssert(tb, "ToolButton has to be PlotToolButton type");

    const Curves curve = tb->getCurve();

    if (b) {
        mCurves[curve]->show();
        if (curve == CURVE_INHARMONICITY) {
            mPlotToolButtons[CURVE_RECORDED]->setChecked(false);
            mPlotToolButtons[CURVE_COMPUTED]->setChecked(false);
            mPlotToolButtons[CURVE_TUNED]->setChecked(false);
            mPlot->setAxisTitle(QwtPlot::yLeft, tr("Inharmonicity"));
            mPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
        } else {
            mPlotToolButtons[CURVE_INHARMONICITY]->setChecked(false);
            mPlot->setAxisTitle(QwtPlot::yLeft, tr("Frequency deviation [cent]"));
            mPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
        }
    } else {
        mCurves[curve]->hide();
    }
    mPlot->replot();
}


