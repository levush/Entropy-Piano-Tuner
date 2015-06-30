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

