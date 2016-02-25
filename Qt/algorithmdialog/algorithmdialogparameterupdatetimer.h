#ifndef ALGORITHMDIALOGPARAMETERUPDATETIMER_H
#define ALGORITHMDIALOGPARAMETERUPDATETIMER_H

#include <QObject>
#include <QWidget>

#include "core/calculation/algorithmparameter.h"
#include "core/calculation/algorithmparameters.h"

class AlgorithmDialogParameterUpdateTimer : public QObject
{
    Q_OBJECT
public:
    explicit AlgorithmDialogParameterUpdateTimer(
            const AlgorithmParameter &description,
            ConstSingleAlgorithmParametersPtr parameters,
            QWidget *outputWidget,
            QObject *parent = 0);

signals:

private slots:
    void update();

private:
    QWidget *mOutputWidget;

    const AlgorithmParameter mParameterDescription;
    ConstSingleAlgorithmParametersPtr mAlgorithmParameters;
};

#endif // ALGORITHMDIALOGPARAMETERUPDATETIMER_H
