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

#include "volumecontrolgroupbox.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLine>
#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QShortcut>
#include <QKeySequence>

#include "displaysize.h"

VolumeControlGroupBox::VolumeControlGroupBox(QWidget *parent) :
    DisplaySizeDependingGroupBox(parent, new QVBoxLayout,
                                 toFlag(MODE_IDLE) | toFlag(MODE_RECORDING)
                                 | ((DisplaySizeDefines::getSingleton()->showVolumeGroupBoxInTuningMode()) ? toFlag(MODE_TUNING) : 0))
{
    setTitle(tr("Volume control"));
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(mMainWidgetContainer->layout());
    mainLayout->setSpacing(0);

    mVolumeControlLevel = new VolumeControlLevel(this);
    mVolumeControlLevel->setFormat(QString());
    if (!mGroupBox) {
        mVolumeControlLevel->setFormat(tr("Volume"));
    }
    mainLayout->addWidget(mVolumeControlLevel);

    QHBoxLayout *linesLayout = new QHBoxLayout;
    mLinesLayout = linesLayout;
    mainLayout->addLayout(linesLayout);

    class Line : public QFrame {
    public:
        Line() {
            setFrameShape(QFrame::VLine);
            setFrameShadow(QFrame::Plain);
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
            setLineWidth(3);
        }
    private:
        virtual QSize minimumSizeHint() const override final {
            return QSize(DisplaySizeDefines::getSingleton()->getSmallIconSize() / 8,
                         DisplaySizeDefines::getSingleton()->getSmallIconSize() / 4);
        }
        virtual QSize sizeHint() const override final {
            return QSize(DisplaySizeDefines::getSingleton()->getSmallIconSize() / 8,
                         DisplaySizeDefines::getSingleton()->getSmallIconSize());
        }
    };

    Line *offLine = new Line;
    Line *onLine = new Line;

    linesLayout->addStretch();
    linesLayout->addWidget(offLine);
    linesLayout->addStretch();
    linesLayout->addWidget(onLine);
    linesLayout->addStretch();


    QHBoxLayout *textLayout = new QHBoxLayout;
    mTextLayout = textLayout;
    mainLayout->addLayout(textLayout);
    textLayout->setMargin(0);
    textLayout->setSpacing(0);
    textLayout->addStretch();

    QLabel *offLabel = new QLabel("Off");
    offLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    textLayout->addWidget(offLabel);

    QHBoxLayout *onLayout = new QHBoxLayout;
    onLayout->setMargin(0);
    textLayout->addLayout(onLayout);

    QLabel *onLabel = new QLabel("On");
    onLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    onLayout->addWidget(onLabel);
    onLayout->addStretch();

    QToolButton *refreshButton = new QToolButton;
    onLayout->addWidget(refreshButton);
    refreshButton->setIcon(QIcon(":/media/images/refresh.png"));
    refreshButton->setIconSize(QSize(1, 1) * DisplaySizeDefines::getSingleton()->getMediumIconSize());
    QObject::connect(refreshButton, SIGNAL(clicked(bool)), this, SLOT(onRefreshClicked()));

    QToolButton *muteMicroButton = new QToolButton;
    onLayout->addWidget(muteMicroButton);
    muteMicroButton->setCheckable(true);
    QIcon muteMicroIcons;
    muteMicroIcons.addFile(QStringLiteral(":/media/images/micro_active.png"), QSize(), QIcon::Normal, QIcon::Off);
    muteMicroIcons.addFile(QStringLiteral(":/media/images/micro_mute.png"), QSize(), QIcon::Normal, QIcon::On);
    muteMicroButton->setIcon(muteMicroIcons);
    muteMicroButton->setIconSize(QSize(1, 1) * DisplaySizeDefines::getSingleton()->getMediumIconSize());
    QObject::connect(muteMicroButton, SIGNAL(toggled(bool)), this, SLOT(onMicroMuteToggled(bool)));
    new QShortcut(QKeySequence(Qt::Key_M), muteMicroButton, SLOT(toggle()));

    if (DisplaySizeDefines::getSingleton()->showMuteOutputButton()) {
        QToolButton *muteSpeakerButton = new QToolButton;
        onLayout->addWidget(muteSpeakerButton);
        muteSpeakerButton->setCheckable(true);
        QIcon muteSpeakerIcons;
        muteSpeakerIcons.addFile(QStringLiteral(":/media/images/speaker_active.png"), QSize(), QIcon::Normal, QIcon::Off);
        muteSpeakerIcons.addFile(QStringLiteral(":/media/images/speaker_mute.png"), QSize(), QIcon::Normal, QIcon::On);
        muteSpeakerButton->setIcon(muteSpeakerIcons);
        muteSpeakerButton->setIconSize(QSize(1, 1) * DisplaySizeDefines::getSingleton()->getMediumIconSize());
        QObject::connect(muteSpeakerButton, SIGNAL(toggled(bool)), this, SLOT(onSpeakerMuteToggled(bool)));
        new QShortcut(QKeySequence(Qt::Key_S), muteSpeakerButton, SLOT(toggle()));
        muteSpeakerButton->setWhatsThis(tr("Click this button to mute the speaker or headphone."));
    }

    mainLayout->addStretch();

    // some rudementary values
    updateLevels(0.1, 0.5);



    // set what's this textsClick this button to reset the automatic calibration of the input volume.
    this->setWhatsThis(tr("This widgets provides settings and information about the input level of the input device."));
    offLabel->setWhatsThis(tr("If the input level drops below this mark the recorder stops and does not process the input signal."));
    onLabel->setWhatsThis(tr("If the input level reaches this threshold the recorder starts analyzing the signal of the input device until the level drops below the 'Off' mark."));
    refreshButton->setWhatsThis(tr("Click this button to reset the automatic calibration of the input volume."));
    muteMicroButton->setWhatsThis(tr("Click this button to mute the input device."));
    offLine->setWhatsThis(offLabel->whatsThis());
    onLine->setWhatsThis(onLabel->whatsThis());
}

void VolumeControlGroupBox::updateLevels(double stopLevel, double onLevel) {
    int max = 10000;
    int pos0 = stopLevel * max;
    int pos1 = onLevel * max;

    mTextLayout->setStretch(0, pos0);
    mTextLayout->setStretch(1, pos1 - pos0);
    mTextLayout->setStretch(2, max - pos1);

    mLinesLayout->setStretch(0, pos0);
    mLinesLayout->setStretch(1, 0);
    mLinesLayout->setStretch(2, pos1 - pos0);
    mLinesLayout->setStretch(3, 0);
    mLinesLayout->setStretch(4, max - pos1);
}

void VolumeControlGroupBox::onRefreshClicked() {
    emit refreshInputLevels();
}

void VolumeControlGroupBox::onMicroMuteToggled(bool b) {
    emit muteMicroToggled(b);
    if (b) {
        mVolumeControlLevel->setValue(0);
    }
}

void VolumeControlGroupBox::onSpeakerMuteToggled(bool b) {
    emit muteSpeakerToggled(b);
}
