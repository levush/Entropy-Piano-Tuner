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

#include "settingsforqt.h"
#include <assert.h>
#include "../core/audio/recorder/audiorecorderadapter.h"
#include "donotshowagainmessagebox.h"
#include "options/optionsdialog.h"


const QString SettingsForQt::KEY_CURRENT_FILE_DIALOG_PATH = "app/currentFileDialogPath";

SettingsForQt::SettingsForQt()
    : mApplicationRuns(0),
      mLastVisitedOptionsPage(options::OptionsDialog::PAGE_ENVIRONMENT) {
}

SettingsForQt::~SettingsForQt()
{

}


SettingsForQt &SettingsForQt::getSingleton() {
    return static_cast<SettingsForQt&>(Settings::getSingleton());
}


void SettingsForQt::load() {
    mApplicationRuns = mSettings.value("app/runs", 0).toLongLong();
    mLanguageId = mSettings.value("app/languageId", QString()).toString().toStdString();

    for (int i = 0; i < DoNotShowAgainMessageBox::COUNT; ++i) {
        if (mSettings.value(QString("doNotShowAgain/id%1").arg(i), false).toBool()) {
            mDoNotShowAgainMessageBoxes.push_back(i);
        }
    }

    mLastVisitedOptionsPage = mSettings.value("app/lastOptionsPage", options::OptionsDialog::PAGE_ENVIRONMENT).toInt();

    mInputDeviceName = mSettings.value("audio/inputDeviceName", QString()).toString();
    mInputDeviceSamplingRate = mSettings.value("audio/inputDeviceSamplingRate", 44100).toInt();

    mOutputDeviceName = mSettings.value("audio/outputDeviceName", QString()).toString();
    mOutputDeviceSamplingRate = mSettings.value("audio/outputDeviceSamplingRate", 22050).toInt();


    mLastUsedAlgorithm = mSettings.value("core/lastUsedAlgorithm", QString()).toString().toStdString();
    mSoundGeneratorMode = static_cast<SoundGenerator::SoundGeneratorMode>(
                mSettings.value("core/soundGeneratorMode", static_cast<int>(SoundGenerator::SGM_SYNTHESIZE_KEY)).toInt());
    mSoundGeneratorVolumeDynamic = mSettings.value("core/soundGeneratorVolumeDynamic", true).toBool();
    mStroboscopeActive = mSettings.value("core/stroboscopeMode", true).toBool();
    mDisableAutomaticKeySelection = mSettings.value("core/disableAutomaticKeySelection", false).toBool();
}

qlonglong SettingsForQt::getApplicationRuns() const {
    return mApplicationRuns;
}

void SettingsForQt::increaseApplicationRuns() {
    ++mApplicationRuns;
    mSettings.setValue("app/runs", mApplicationRuns);
}

void SettingsForQt::setLanguageId(const std::string &s) {
    Settings::setLanguageId(s);
    mSettings.setValue("app/languageId", QString::fromStdString(mLanguageId));
}

std::string SettingsForQt::getUserLanguageId() const {
    if (mLanguageId.size() > 0) {
        return mLanguageId;
    } else {
        return QLocale::system().name().left(2).toStdString();
    }
}

bool SettingsForQt::doNotShowAgainMessageBox(int id) const {
    return std::find(mDoNotShowAgainMessageBoxes.begin(), mDoNotShowAgainMessageBoxes.end(), id) != mDoNotShowAgainMessageBoxes.end();
}

void SettingsForQt::setDoNotShowAgainMessageBox(int id, bool doNotShowAgain) {
    auto entry(std::find(mDoNotShowAgainMessageBoxes.begin(), mDoNotShowAgainMessageBoxes.end(), id));
    if (doNotShowAgain && entry == mDoNotShowAgainMessageBoxes.end()) {
        mDoNotShowAgainMessageBoxes.push_back(id);
        mSettings.setValue(QString("doNotShowAgain/id%1").arg(id), true);
    } else if (entry != mDoNotShowAgainMessageBoxes.end()) {
        mDoNotShowAgainMessageBoxes.erase(entry);
        mSettings.remove(QString("doNotShowAgain/id%1").arg(id));
    }
}

void SettingsForQt::setLastVisitedOptionsPage(int id) {
    mLastVisitedOptionsPage = id;
    mSettings.setValue("app/lastOptionsPage", mLastVisitedOptionsPage);
}

void SettingsForQt::setInputDeviceName(const QString &s) {
    mInputDeviceName = s;
    mSettings.setValue("audio/inputDeviceName", mInputDeviceName);
}

void SettingsForQt::setInputDeviceSamplingRate(int rate) {
    mInputDeviceSamplingRate = rate;
    mSettings.setValue("audio/inputDeviceSamplingRate", mInputDeviceSamplingRate);
}

void SettingsForQt::setOutputDeviceName(const QString &s) {
    mOutputDeviceName = s;
    mSettings.setValue("audio/outputDeviceName", mOutputDeviceName);
}

void SettingsForQt::setOutputDeviceSamplingRate(int rate) {
    mOutputDeviceSamplingRate = rate;
    mSettings.setValue("audio/outputDeviceSamplingRate", mOutputDeviceSamplingRate);
}

void SettingsForQt::setLastUsedAlgorithm(const std::string &name) {
    Settings::setLastUsedAlgorithm(name);
    mSettings.setValue("core/lastUsedAlgorithm", QString::fromStdString(name));
}

void SettingsForQt::setSoundGeneratorMode(SoundGenerator::SoundGeneratorMode mode) {
    Settings::setSoundGeneratorMode(mode);
    mSettings.setValue("core/soundGeneratorMode", static_cast<int>(mode));
}

void SettingsForQt::setSoundGeneratorVolumeDynamic(bool dynamic) {
    Settings::setSoundGeneratorVolumeDynamic(dynamic);
    mSettings.setValue("core/soundGeneratorVolumeDynamic", dynamic);
}

void SettingsForQt::setStroboscopeMode(bool enable) {
    Settings::setStroboscopeMode(enable);
    mSettings.setValue("core/stroboscopeMode", enable);
}

void SettingsForQt::setDisableAutomaticKeySelection(bool disable) {
    Settings::setDisableAutomaticKeySelection(disable);
    mSettings.setValue("core/disableAutomaticKeySelection", disable);
}
