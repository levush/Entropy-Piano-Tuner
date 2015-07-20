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

#include "optionspageenvironmentgeneralpage.h"
#include "../../settingsforqt.h"
#include "../../donotshowagainmessagebox.h"
#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>

namespace options {

PageEnvironmentGeneral::PageEnvironmentGeneral(OptionsDialog *optionsDialog)
{
    QGridLayout *inputLayout = new QGridLayout;
    this->setLayout(inputLayout);

    inputLayout->setColumnStretch(1, 1);

    QGroupBox *userInterface = new QGroupBox(tr("User Interface"));
    inputLayout->addWidget(userInterface, 0, 0);

    QFormLayout *userInterfaceLayout = new QFormLayout;
    userInterface->setLayout(userInterfaceLayout);

    mLanguageSelection = new QComboBox;
    userInterfaceLayout->addRow(new QLabel(tr("Language")), mLanguageSelection);

    mLanguageSelection->addItem(tr("<System Language>"), QVariant(""));
    mLanguageSelection->addItem("English", QVariant("en"));
    mLanguageSelection->addItem("Deutsch", QVariant("de"));
    mLanguageSelection->addItem("Español", QVariant("es"));
    mLanguageSelection->addItem("Polski", QVariant("pl"));
    mLanguageSelection->addItem("Português", QVariant("pt"));
    mLanguageSelection->addItem("Korean", QVariant("kr"));
    //mLanguageSelection->addItem("Pусский", QVariant("ru"));

    mLanguageSelection->setCurrentIndex(0);

    for (int i = 0; i < mLanguageSelection->count(); i++) {
        if (mLanguageSelection->itemData(i).toString().toStdString() == SettingsForQt::getSingleton().getLanguageId()) {
            mLanguageSelection->setCurrentIndex(i);
            break;
        }
    }

    mResetWarningsButton = new QPushButton(tr("Reset warnings"));
    userInterfaceLayout->addRow(mResetWarningsButton, (QWidget*)nullptr);
    mResetWarningsButton->setEnabled(SettingsForQt::getSingleton().countCurrentDoNotShowAgainMessageBoxes() > 0);

    // notify if changes are made
    QObject::connect(mLanguageSelection, SIGNAL(currentIndexChanged(int)), optionsDialog, SLOT(onChangesMade()));

    // buttons
    QObject::connect(mResetWarningsButton, SIGNAL(clicked()), this, SLOT(onResetWarnings()));
}

void PageEnvironmentGeneral::apply() {
    if (mLanguageSelection->currentData().toString().toStdString() != SettingsForQt::getSingleton().getLanguageId()) {
        QMessageBox::information(this, tr("Information"), tr("The language change will take effect after a restart of the entropy piano tuner."));
        SettingsForQt::getSingleton().setLanguageId(mLanguageSelection->currentData().toString().toStdString());
    }
}

void PageEnvironmentGeneral::onResetWarnings() {
    for (int i = 0; i < DoNotShowAgainMessageBox::COUNT; ++i) {
        SettingsForQt::getSingleton().setDoNotShowAgainMessageBox(i, false);
    }
    mResetWarningsButton->setEnabled(false);
}

}  // namespace options
