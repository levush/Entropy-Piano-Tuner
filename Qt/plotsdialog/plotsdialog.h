#ifndef PLOTSDIALOG_H
#define PLOTSDIALOG_H

#include <QDialog>
#include <QToolButton>
#include <array>
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
