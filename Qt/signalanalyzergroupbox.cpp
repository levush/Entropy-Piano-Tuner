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

#include "signalanalyzergroupbox.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "recordingqualitybar.h"
#include "recordingstatusgraphicsview.h"

SignalAnalyzerGroupBox::SignalAnalyzerGroupBox(QWidget *parent) :
    DisplaySizeDependingGroupBox(parent, new QHBoxLayout, toFlag(MODE_IDLE) | toFlag(MODE_RECORDING) | toFlag(MODE_TUNING))
{
    setTitle(tr("Signal analyzer"));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *mainLayout = qobject_cast<QHBoxLayout*>(mMainWidgetContainer->layout());
    mainLayout->setSpacing(0);

    RecordingQualityBar *qualityBar = new RecordingQualityBar(this);
    mainLayout->addWidget(qualityBar);

    RecordingStatusGraphicsView *statusView = new RecordingStatusGraphicsView(this);
    mainLayout->addWidget(statusView);


    QVBoxLayout *labelsLayout = new QVBoxLayout;
    mainLayout->addLayout(labelsLayout);
    labelsLayout->setMargin(0);

    labelsLayout->addStretch();

    mKeyLabel = new QLabel("-");
    mKeyLabel->setAlignment(Qt::AlignCenter);
    QFont keyLabelFont;
    keyLabelFont.setPointSize(26);
    mKeyLabel->setFont(keyLabelFont);
    mKeyLabel->setFixedWidth(mKeyLabel->fontMetrics().width("A#0"));
    labelsLayout->addWidget(mKeyLabel);

    labelsLayout->addSpacing(2);

    QHBoxLayout *frequencyLayout = new QHBoxLayout;
    labelsLayout->addLayout(frequencyLayout);

    mFrequencyLabel = new QLabel("-");
    mFrequencyLabel->setFixedWidth(mFrequencyLabel->fontMetrics().width("0000.0"));
    mFrequencyLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    frequencyLayout->addWidget(mFrequencyLabel);

    QLabel *hzLabel = new QLabel(tr("Hz"));
    frequencyLayout->addWidget(hzLabel);
    hzLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hzLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);


    labelsLayout->addStretch();


    // set whats this texts
    mKeyLabel->setWhatsThis(tr("This label displays the current selected key."));
    mFrequencyLabel->setWhatsThis(tr("This label shows the ground frequency of the selected key."));
    hzLabel->setWhatsThis(mFrequencyLabel->whatsThis());
}

