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

#include "optionspageaudiomidipage.h"
#include <QGridLayout>
#include <QLabel>

namespace options {

PageAudioMidi::PageAudioMidi(OptionsDialog *optionsDialog, MidiAdapter *midiInterface)
    : mMidiInterface(midiInterface) {
    QGridLayout *inputLayout = new QGridLayout;
    this->setLayout(inputLayout);

    inputLayout->setColumnStretch(1, 1);

    inputLayout->addWidget(new QLabel(tr("Midi device")), 0, 0);
    inputLayout->addWidget(mDeviceSelection = new QComboBox(), 0, 1);

    int numberOfPorts = mMidiInterface->GetNumberOfPorts();
    for (int i = 0; i < numberOfPorts; ++i) {
        mDeviceSelection->addItem(QString::fromStdString(mMidiInterface->GetPortName(i)), QVariant::fromValue(i));
    }


    inputLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding), 20, 0);

    if (numberOfPorts == 0) {
        this->setDisabled(true);
    }
    mDeviceSelection->setCurrentIndex(mMidiInterface->getCurrentPort());

    // notify if changes are made
    QObject::connect(mDeviceSelection, SIGNAL(currentIndexChanged(int)), optionsDialog, SLOT(onChangesMade()));
}

void PageAudioMidi::apply() {
    if (mDeviceSelection->currentIndex() >= 0) {
        int midiPort = mDeviceSelection->currentData().toInt();
        mMidiInterface->OpenPort(midiPort);
    }
}

}  // namespace midi
