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

#include "optionspageenvironmenttuningpage.h"

#include <QGridLayout>

#include "core/audio/player/soundgenerator.h"

#include "implementations/settingsforqt.h"
#include "widgets/preferredtextsizelabel.h"

namespace options {

PageEnvironmentTuning::PageEnvironmentTuning(OptionsDialog *optionsDialog)
{
    QGridLayout *layout = new QGridLayout;
    mCentralWidget->setLayout(layout);
    layout->setColumnStretch(1, 1);

    mSynthesizerModeComboBox = new QComboBox;
    mSynthesizerModeComboBox->addItem(tr("Disabled"), QVariant(SoundGenerator::SGM_DEACTIVATED));
    mSynthesizerModeComboBox->addItem(tr("Synthesized key sound"), QVariant(SoundGenerator::SGM_SYNTHESIZE_KEY));
    mSynthesizerModeComboBox->addItem(tr("Reference key"), QVariant(SoundGenerator::SGM_REFERENCE_TONE));
    layout->addWidget(new QLabel(tr("Synthesizer mode")), 0, 0);
    layout->addWidget(mSynthesizerModeComboBox, 0, 1);

    mSynthesizerVolumeDynamicCheckBox = new QCheckBox;
    QLabel *synthesizerVolumeDynamicLabel = new PreferredTextSizeLabel(tr("Dynamic synthesizer volume"));
    synthesizerVolumeDynamicLabel->setWordWrap(true);
    layout->addWidget(synthesizerVolumeDynamicLabel, 1, 0);
    layout->addWidget(mSynthesizerVolumeDynamicCheckBox, 1, 1);

    mStroboscopeCheckBox = new QCheckBox;
    QLabel *stroboscopeLabel = new PreferredTextSizeLabel(tr("Stroboscopic tuning indicator"));
    stroboscopeLabel->setWordWrap(true);
    layout->addWidget(stroboscopeLabel, 2, 0);
    layout->addWidget(mStroboscopeCheckBox, 2, 1);

    mDisableAutomaticKeySelecetionCheckBox = new QCheckBox;
    QLabel *disableAutomaticKeySelectionLabel = new PreferredTextSizeLabel(tr("Disable automatic key selection"));
    disableAutomaticKeySelectionLabel->setWordWrap(true);
    layout->addWidget(disableAutomaticKeySelectionLabel, 3, 0);
    layout->addWidget(mDisableAutomaticKeySelecetionCheckBox, 3, 1);

    layout->setRowStretch(5, 1);

    mSynthesizerModeComboBox->setCurrentIndex(mSynthesizerModeComboBox->findData(QVariant(SettingsForQt::getSingleton().getSoundGeneratorMode())));
    mSynthesizerVolumeDynamicCheckBox->setChecked(SettingsForQt::getSingleton().isSoundGeneratorVolumeDynamic());
    mStroboscopeCheckBox->setChecked(SettingsForQt::getSingleton().isStroboscopeActive());
    mDisableAutomaticKeySelecetionCheckBox->setChecked(SettingsForQt::getSingleton().isAutomaticKeySelectionDisabled());

    QObject::connect(mSynthesizerModeComboBox, SIGNAL(currentIndexChanged(int)), optionsDialog, SLOT(onChangesMade()));
    QObject::connect(mSynthesizerVolumeDynamicCheckBox, SIGNAL(toggled(bool)), optionsDialog, SLOT(onChangesMade()));
    QObject::connect(mStroboscopeCheckBox, SIGNAL(toggled(bool)), optionsDialog, SLOT(onChangesMade()));
    QObject::connect(mDisableAutomaticKeySelecetionCheckBox, SIGNAL(toggled(bool)), optionsDialog, SLOT(onChangesMade()));
}

void PageEnvironmentTuning::apply()
{
    SettingsForQt::getSingleton().setSoundGeneratorMode(
                static_cast<SoundGenerator::SoundGeneratorMode>(mSynthesizerModeComboBox->currentData().toInt()));
    SettingsForQt::getSingleton().setSoundGeneratorVolumeDynamic(mSynthesizerVolumeDynamicCheckBox->isChecked());
    SettingsForQt::getSingleton().setStroboscopeMode(mStroboscopeCheckBox->isChecked());
    SettingsForQt::getSingleton().setDisableAutomaticKeySelection(mDisableAutomaticKeySelecetionCheckBox->isChecked());

}

}  // namespace options

