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

#ifndef SIGNALANALYZERGROUPBOX_H
#define SIGNALANALYZERGROUPBOX_H

#include <QLabel>

#include "prerequisites.h"

#include "widgets/displaysizedependinggroupbox.h"


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
