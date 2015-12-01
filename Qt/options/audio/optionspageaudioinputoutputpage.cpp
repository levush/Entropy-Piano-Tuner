/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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

#include "optionspageaudioinputoutputpage.h"
#include <QMessageBox>
#include "../../../core/core.h"
#include "../../../core/audio/player/audioplayeradapter.h"
#include "core/config.h"
#include "../../../core/audio/recorder/audiorecorderadapter.h"
#include "../../volumecontrollevel.h"
#include "../../mainwindow.h"
#include "../../settingsforqt.h"

namespace options {

PageAudioInputOutput::PageAudioInputOutput(OptionsDialog *optionsDialog, QAudio::Mode mode)
    : mOptionsDialog(optionsDialog),
      mAudioBase(nullptr),
      mMode(mode) {

    if (mode == QAudio::AudioInput) {
        mAudioBase = optionsDialog->getCore()->getAudioRecorder();
    } else {
        mAudioBase = optionsDialog->getCore()->getAudioPlayer();
    }

    QGridLayout *inputLayout = new QGridLayout;
    this->setLayout(inputLayout);

    inputLayout->setColumnStretch(1, 1);

    mDeviceSelection = new QComboBox;

    QList<QAudioDeviceInfo> deviceInfos(QAudioDeviceInfo::availableDevices(mode));
    for (QAudioDeviceInfo info : deviceInfos) {
        if (!info.isFormatSupported(info.preferredFormat())) {
            // no supported formats, dont list
            LogI("%s is not supported.", info.deviceName().toStdString().c_str());
            continue;
        }

        LogI("%s is supported.", info.deviceName().toStdString().c_str());

        mDeviceSelection->addItem(info.deviceName(), QVariant::fromValue<QAudioDeviceInfo>(info));
    }

    // select input device
    QPushButton *selectDefaultInputDeviceButton = new QPushButton(tr("Default"));
    inputLayout->addWidget(new QLabel(tr("Input device")), 0, 0);
    inputLayout->addWidget(mDeviceSelection, 0, 1);
    inputLayout->addWidget(selectDefaultInputDeviceButton, 0, 2);

    // sampling rate
    QPushButton *selectDefaultSamplingRateButton = new QPushButton(tr("Default"));
    inputLayout->addWidget(new QLabel(tr("Sampling rates")), 1, 0);
    inputLayout->addWidget(mSamplingRates = new QComboBox, 1, 1);
    inputLayout->addWidget(selectDefaultSamplingRateButton, 1, 2);

    // add open sound settings button
    if (mOptionsDialog->getMainWindow()->isSoundControlSupported()) {
        QPushButton *openSystemSettingsButton = new QPushButton(tr("Open system settings"));
        inputLayout->addWidget(openSystemSettingsButton, 10, 0);
        QObject::connect(openSystemSettingsButton, SIGNAL(clicked()), mOptionsDialog->getMainWindow(), SLOT(onOpenSoundControl()));
    }

    // add stretch
    inputLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding), 20, 0);


    // set current values
    mDeviceSelection->setCurrentText(QString::fromStdString(mAudioBase->getDeviceName()));
    onDeviceSelectionChanged(mDeviceSelection->currentIndex());
    mSamplingRates->setCurrentText(QString("%1").arg(mAudioBase->getSamplingRate()));

    // connect widgets
    QObject::connect(mDeviceSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(onDeviceSelectionChanged(int)));

    QObject::connect(selectDefaultInputDeviceButton, SIGNAL(clicked()), this, SLOT(onDefaultDevice()));
    QObject::connect(selectDefaultSamplingRateButton, SIGNAL(clicked()), this, SLOT(onDefaultSamplingRate()));

    // notify if changes are made
    QObject::connect(mDeviceSelection, SIGNAL(currentIndexChanged(int)), optionsDialog, SLOT(onChangesMade()));
    QObject::connect(mSamplingRates, SIGNAL(currentIndexChanged(int)), optionsDialog, SLOT(onChangesMade()));
}

void PageAudioInputOutput::apply() {
    assert(mSamplingRates->currentIndex() != -1);
    assert(mSamplingRates->currentText().isEmpty() == false);

    PCMWriterInterface *writerInterfaceBackup = nullptr;
    if (mMode == QAudio::AudioOutput) {
        writerInterfaceBackup = dynamic_cast<AudioPlayerAdapter*>(mAudioBase)->getWriter();
    }

    mAudioBase->stop();
    mAudioBase->exit();
    if (mMode == QAudio::AudioOutput) {
        SettingsForQt::getSingleton().setOutputDeviceName(mDeviceSelection->currentText());
        SettingsForQt::getSingleton().setOutputDeviceSamplingRate(mSamplingRates->currentText().toInt());
    } else {
        SettingsForQt::getSingleton().setInputDeviceName(mDeviceSelection->currentText());
        SettingsForQt::getSingleton().setInputDeviceSamplingRate(mSamplingRates->currentText().toInt());
    }
    mAudioBase->setDeviceName(mDeviceSelection->currentText().toStdString());
    mAudioBase->setSamplingRate(mSamplingRates->currentText().toInt());
    mAudioBase->init();
    mAudioBase->start();

    if (mMode == QAudio::AudioOutput) {
        dynamic_cast<AudioPlayerAdapter*>(mAudioBase)->setWriter(writerInterfaceBackup);
        mOptionsDialog->getCore()->getSoundGenerator().init();
    }
}

void PageAudioInputOutput::onDeviceSelectionChanged(int row) {
    QAudioDeviceInfo info(mDeviceSelection->itemData(row).value<QAudioDeviceInfo>());

    mSamplingRates->clear();

    for (int rate : info.supportedSampleRates()) {
        if (rate < 9000) {
            // drop out 8000 sampling rate. Frequencies above 4000 Hz cant be recorded/played
            // elsewise. This is the uppermost key!
            continue;
        }
        mSamplingRates->addItem(QString("%1").arg(rate));
    }
    onDefaultSamplingRate();
}

void PageAudioInputOutput::onDefaultDevice() {
    if (mMode == QAudio::AudioInput) {
        mDeviceSelection->setCurrentText(QAudioDeviceInfo::defaultInputDevice().deviceName());
    }
    else {
        mDeviceSelection->setCurrentText(QAudioDeviceInfo::defaultOutputDevice().deviceName());
    }
    onDefaultSamplingRate();
}

void PageAudioInputOutput::onDefaultSamplingRate() {
    QAudioDeviceInfo info(mDeviceSelection->currentData().value<QAudioDeviceInfo>());
    if (info.deviceName().isEmpty()) {
        // no device found
        mSamplingRates->setCurrentText(QString());
        return;
    }

    // use 22050/44100 as default sampling rate if the device supports it,
    // since the synthesizer cant handle too many samples
    // and we need at least 11025 samples for the fft.
    if (mMode == QAudio::AudioOutput && info.supportedSampleRates().contains(22050)) {
        mSamplingRates->setCurrentText(QString("22050"));
    } else if (mMode == QAudio::AudioInput && info.supportedSampleRates().contains(44100)) {
        mSamplingRates->setCurrentText(QString("44100"));
    } else {
        // select prefered
        mSamplingRates->setCurrentText(QString("%1").arg(info.preferredFormat().sampleRate()));
        if (info.preferredFormat().sampleRate() < 9000) {
            QMessageBox::warning(this, tr("Warning"), tr("You need at least a sampling rate of %1 to record and play all keys.").arg(11025));
        }
    }
}

}  // namespace options
