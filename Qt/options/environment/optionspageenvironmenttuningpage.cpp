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

#include "optionspageenvironmenttuningpage.h"
#include "core/audio/soundgeneratormode.h"
#include "../../settingsforqt.h"
#include "preferredtextsizelabel.h"
#include <QGridLayout>

namespace options {

PageEnvironmentTuning::PageEnvironmentTuning(OptionsDialog *optionsDialog)
{
    QGridLayout *layout = new QGridLayout;
    mCentralWidget->setLayout(layout);
    layout->setColumnStretch(1, 1);

    mSynthesizerMode = new QComboBox;

    mSynthesizerMode->addItem(tr("Disabled"), QVariant(SGM_DEACTIVATED));
    mSynthesizerMode->addItem(tr("Synthesized key sound"), QVariant(SGM_SYNTHESIZE_KEY));
    mSynthesizerMode->addItem(tr("Reference key"), QVariant(SGM_REFERENCE_TONE));

    layout->addWidget(new QLabel(tr("Synthesizer mode")), 0, 0);
    layout->addWidget(mSynthesizerMode, 0, 1);

    mSynthesizerVolumeDynamic = new QCheckBox;
    QLabel *synthesizerVolumeDynamicLabel = new PreferredTextSizeLabel(tr("Dynamic synthesizer volume"));
    synthesizerVolumeDynamicLabel->setWordWrap(true);
    layout->addWidget(synthesizerVolumeDynamicLabel, 1, 0);
    layout->addWidget(mSynthesizerVolumeDynamic, 1, 1);

    mDisableAutomaticKeySelecetion = new QCheckBox;
    QLabel *disableAutomaticKeySelectionLabel = new PreferredTextSizeLabel(tr("Disable automatic key selection"));
    disableAutomaticKeySelectionLabel->setWordWrap(true);
    layout->addWidget(disableAutomaticKeySelectionLabel, 2, 0);
    layout->addWidget(mDisableAutomaticKeySelecetion, 2, 1);

    layout->setRowStretch(5, 1);

    mSynthesizerMode->setCurrentIndex(mSynthesizerMode->findData(QVariant(SettingsForQt::getSingleton().getSoundGeneratorMode())));
    mSynthesizerVolumeDynamic->setChecked(SettingsForQt::getSingleton().isSoundGeneratorVolumeDynamic());
    mDisableAutomaticKeySelecetion->setChecked(SettingsForQt::getSingleton().isAutomaticKeySelectionDisabled());

    QObject::connect(mSynthesizerMode, SIGNAL(currentIndexChanged(int)), optionsDialog, SLOT(onChangesMade()));
    QObject::connect(mSynthesizerVolumeDynamic, SIGNAL(toggled(bool)), optionsDialog, SLOT(onChangesMade()));
    QObject::connect(mDisableAutomaticKeySelecetion, SIGNAL(toggled(bool)), optionsDialog, SLOT(onChangesMade()));
}

void PageEnvironmentTuning::apply() {
    SettingsForQt::getSingleton().setSoundGeneratorMode(
                static_cast<SoundGeneratorMode>(mSynthesizerMode->currentData().toInt()));
    SettingsForQt::getSingleton().setSoundGeneratorVolumeDynamic(mSynthesizerVolumeDynamic->isChecked());
    SettingsForQt::getSingleton().setDisableAutomaticKeySelection(mDisableAutomaticKeySelecetion->isChecked());
}

}  // namespace options

