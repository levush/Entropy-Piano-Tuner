#ifndef ALGORITHMDIALOGPARAMETERUPDATETIMER_H
#define ALGORITHMDIALOGPARAMETERUPDATETIMER_H

#include <QObject>
#include <QWidget>

#include "core/calculation/algorithmparameterdescription.h"
#include "core/calculation/algorithmparameters.h"

class AlgorithmDialogParameterUpdateTimer : public QObject
{
    Q_OBJECT
public:
    explicit AlgorithmDialogParameterUpdateTimer(
            const AlgorithmParameterDescription &description,
            ConstSingleAlgorithmParametersPtr parameters,
            QWidget *outputWidget,
            QObject *parent = 0);

signals:

private slots:
    void update();

private:
    QWidget *mOutputWidget;

    const AlgorithmParameterDescription mParameterDescription;
    ConstSingleAlgorithmParametersPtr mAlgorithmParameters;
};

#endif // ALGORITHMDIALOGPARAMETERUPDATETIMER_H
