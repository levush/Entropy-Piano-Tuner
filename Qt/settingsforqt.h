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

#ifndef SETTINGSFORQT_H
#define SETTINGSFORQT_H

#include <QSettings>
#include <QString>
#include <vector>
#include "../core/settings.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Qt specific settings.
///
///////////////////////////////////////////////////////////////////////////////
class SettingsForQt : public Settings
{
public:
    // non globally stored settings ids
    static const QString KEY_CURRENT_FILE_DIALOG_PATH;
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Empty default constructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    SettingsForQt();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Empty defaul destructor.
    ///////////////////////////////////////////////////////////////////////////////
    ~SettingsForQt();


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for the singleton.
    /// \return mSingleton converted to SettingsForQt
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static SettingsForQt &getSingleton();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to load the settings from the storage.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void load();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for mApplicationRuns.
    /// \return mApplicationRuns
    ///
    ///////////////////////////////////////////////////////////////////////////////
    qlonglong getApplicationRuns() const;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Increments mApplicationRuns.
    ///
    /// This will automatically store its new value to the QSettings.
    ///////////////////////////////////////////////////////////////////////////////
    void increaseApplicationRuns();

    virtual void setLanguageId(const std::string &s) override final;
    virtual std::string getUserLanguageId() const override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function to check if the given AutoClosingMessageBox shall
    ///        not be shown again.
    /// \param id : Id of the message box
    /// \return Show the message box again?
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool doNotShowAgainMessageBox(int id) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a AutoClosingMessageBox state whether to show again.
    /// \param id : The id of the AutoClosingMessageBox
    /// \param doNotShowAgain : The state of the message box
    ///
    /// This will automatically store its new value to the QSettings.
    ///////////////////////////////////////////////////////////////////////////////
    void setDoNotShowAgainMessageBox(int id, bool doNotShowAgain);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Get the count of all do not show again message boxes whose states
    ///        are stored.
    /// \return Number of message boxes.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t countCurrentDoNotShowAgainMessageBoxes() const {return mDoNotShowAgainMessageBoxes.size();}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for mLastVisitedOptionsPage.
    /// \return mLastVisitedOptionsPage
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int getLastVisitedOptionsPage() const {return mLastVisitedOptionsPage;}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Setter function for mLastVisitedOptionsPage.
    /// \param id : Id of the last visited options page
    ///
    /// This will automatically store its new value to the QSettings.
    ///////////////////////////////////////////////////////////////////////////////
    void setLastVisitedOptionsPage(int id);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for mInputDeviceName.
    /// \return mInputDeviceName
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const QString &getInputDeviceName() const {return mInputDeviceName;}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Setter function for mInputDeviceName.
    /// \param s : The name of the input device
    ///
    /// This will automatically store its new value to the QSettings.
    ///////////////////////////////////////////////////////////////////////////////
    void setInputDeviceName(const QString &s);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for mInputDeviceSamplingRate.
    /// \return mInputDeviceSamplingRate
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int getInputDeviceSamplingRate() const {return mInputDeviceSamplingRate;}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Setter function for mInputDeviceSamplingRate.
    /// \param rate : The sampling rate
    ///
    /// This will automatically store its new value to the QSettings.
    ///////////////////////////////////////////////////////////////////////////////
    void setInputDeviceSamplingRate(int rate);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for mOutputDeviceName.
    /// \return mOutputDeviceName
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const QString &getOuputDeviceName() const {return mOutputDeviceName;}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Setter function for mOutputDeviceName.
    /// \param s : The name of the input device
    ///
    /// This will automatically store its new value to the QSettings.
    ///////////////////////////////////////////////////////////////////////////////
    void setOutputDeviceName(const QString &s);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for mOutputDeviceSamplingRate.
    /// \return mOutputDeviceSamplingRate
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int getOutputDeviceSamplingRate() const {return mOutputDeviceSamplingRate;}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Setter function for mOutputDeviceSamplingRate.
    /// \param b : The sampling rate
    ///
    /// This will automatically store its new value to the QSettings.
    ///////////////////////////////////////////////////////////////////////////////
    void setOutputDeviceSamplingRate(int rate);

    virtual void setLastUsedAlgorithm(const std::string &name) override final;
    virtual void setSoundGeneratorMode(SoundGeneratorMode mode) override final;
    virtual void setSoundGeneratorVolumeDynamic(bool dynamic) override final;

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Counter for the application runs.
    ///
    /// This is used to detect whether this is the first run.
    ///////////////////////////////////////////////////////////////////////////////
    qlonglong mApplicationRuns;

    /// store the ids of the message boxes that wont show again
    std::vector<int> mDoNotShowAgainMessageBoxes;


    /// last activated options page
    int mLastVisitedOptionsPage;

    // Audio settings (devices, samling rates)

    /// input device name
    QString mInputDeviceName;

    /// input device sampling rate
    int mInputDeviceSamplingRate;

    /// output device name
    QString mOutputDeviceName;

    /// output device sampling rate
    int mOutputDeviceSamplingRate;

private:
    /// The QSettings that will store all values to the storage.
    QSettings mSettings;
};

#endif // SETTINGSFORQT_H
