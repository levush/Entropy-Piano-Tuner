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

#ifndef OPTIONSPAGEAUDIOINPUTOUTPUTPAGE_H
#define OPTIONSPAGEAUDIOINPUTOUTPUTPAGE_H

#include <QSpinBox>
#include "../optionscontentswidgetinterface.h"

namespace options {

class PageAudioInputOutput : public QWidget, public ContentsWidgetInterface {
    Q_OBJECT
public:
    PageAudioInputOutput(OptionsDialog *optionsDialog, QAudio::Mode mode);
    void apply() override final;


private slots:
    void onDeviceSelectionChanged(int);

    void onDefaultDevice();
    void onDefaultSamplingRate();


    // special audio output
    void onDefaultChannel();
    void onDefaultBufferSize();

private:
    OptionsDialog *mOptionsDialog;
    AudioBase *mAudioBase;
    QAudio::Mode mMode;

    QComboBox *mDeviceSelection;

    QComboBox *mSamplingRates;

    // special audio output
    QComboBox *mChannelsSelect;
    QSpinBox *mBufferSizeEdit;
};

}  // namespace options

#endif // OPTIONSPAGEAUDIOINPUTOUTPUTPAGE_H
