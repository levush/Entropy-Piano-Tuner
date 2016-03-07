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

#ifndef OPTIONSPAGEAUDIOINPUTOUTPUTPAGE_H
#define OPTIONSPAGEAUDIOINPUTOUTPUTPAGE_H

#include <QSpinBox>

#include "prerequisites.h"

#include "../optionscontentswidgetinterface.h"

namespace options {

///
/// \brief Page of the audio options that displays content for input or output device
///
/// If this is for the input device, the user can set the device and its sample rate.
///
/// If this is for the output device, the user can set the device and its sample rate.
/// Moreover the user can set the number of output channels and the buffer size for the
/// synthesizer.
///
class PageAudioInputOutput : public QWidget, public ContentsWidgetInterface {
    Q_OBJECT
public:
    ///
    /// \brief Create a input or output options audio page
    /// \param optionsDialog The options dialog
    /// \param mode The mode of the audio settings page
    ///
    PageAudioInputOutput(OptionsDialog *optionsDialog, QAudio::Mode mode);


    ///
    /// \brief Stops the audio, changes the device and restarts the audio
    ///
    void apply() override final;


private slots:
    ///
    /// \brief Called if the device (input/output) changed
    ///
    void onDeviceSelectionChanged(int);

    ///
    /// \brief Call if the user wants to select the default device
    ///
    void onDefaultDevice();

    ///
    /// \brief Call to select the default sampling rate of the current selected device
    ///
    void onDefaultSamplingRate();


    // special audio output
    // ====================================================================

    ///
    /// \brief Call to select the default channels (max = 2)
    ///
    void onDefaultChannel();

    ///
    /// \brief Call to set the buffer size to its default
    ///
    void onDefaultBufferSize();

private:
    OptionsDialog *mOptionsDialog;      ///< Pointer to the options dialog
    AudioBase *mAudioBase;              ///< Pointer to the audio base (input/output)
    QAudio::Mode mMode;                 ///< Mode of this page

    QComboBox *mDeviceSelection;        ///< Item to select the device
    QComboBox *mSamplingRates;          ///< Item to select the sampling rate

    // special audio output
    // =====================================================================
    QComboBox *mChannelsSelect;         ///< Item to select the number of channels
    QSpinBox *mBufferSizeEdit;          ///< Item to select the buffer size for output
};

}  // namespace options

#endif // OPTIONSPAGEAUDIOINPUTOUTPUTPAGE_H
