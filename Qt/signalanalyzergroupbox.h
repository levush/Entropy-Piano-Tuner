#ifndef SIGNALANALYZERGROUPBOX_H
#define SIGNALANALYZERGROUPBOX_H

#include <QLabel>

#include "displaysizedependinggroupbox.h"


class SignalAnalyzerGroupBox : public DisplaySizeDependingGroupBox
{
    Q_OBJECT
public:
    SignalAnalyzerGroupBox(QWidget *parent);


    void setKey(QString key) {mKeyLabel->setText(key);}
    void setFrequency(QString f) {mFrequencyLabel->setText(f);}

private:
    QLabel *mKeyLabel;
    QLabel *mFrequencyLabel;
};

#endif // SIGNALANALYZERGROUPBOX_H
