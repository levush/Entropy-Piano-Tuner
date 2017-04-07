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

#ifndef SETTINGSFORQT_H
#define SETTINGSFORQT_H

#include <QSettings>
#include <QString>
#include <vector>

#include "prerequisites.h"

#include "core/settings.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Qt specific settings.
///
///////////////////////////////////////////////////////////////////////////////
class SettingsForQt : public Settings
{
public:
    // non globally stored settings ids
    static const QString KEY_CURRENT_FILE_DIALOG_PATH;

    static const QString KEY_LANGUAGE_ID;
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
    /// \return -1 if show, >0 for stored result
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int doNotShowAgainMessageBox(int id) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Sets a AutoClosingMessageBox state whether to show again.
    /// \param id : The id of the AutoClosingMessageBox
    /// \param doNotShowAgain : The state of the message box
    ///
    /// This will automatically store its new value to the QSettings.
    ///////////////////////////////////////////////////////////////////////////////
    void setDoNotShowAgainMessageBox(int id, bool doNotShowAgain, int value);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Sets all results to -1 and stored to false
    void clearAllDoNotShowAgainMessageBoxes();


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Counts how many DoNotShowAgainMessageBoxes have a stored state
    /// \return Number
    ///
    int countActiveDoNotShowAgainMessageBoxes() const;

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

    virtual void setSoundGeneratorMode(SoundGenerator::SoundGeneratorMode mode) override final;
    virtual void setSoundGeneratorVolumeDynamic(bool dynamic) override final;
    virtual void setStroboscopeMode(bool enable) override final;
    virtual void setDisableAutomaticKeySelection(bool disable) override final;

protected:
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Counter for the application runs.
    ///
    /// This is used to detect whether this is the first run.
    ///////////////////////////////////////////////////////////////////////////////
    qlonglong mApplicationRuns;


    /// last activated options page
    int mLastVisitedOptionsPage;

private:
    /// The QSettings that will store all values to the storage.
    QSettings mSettings;
};

#endif // SETTINGSFORQT_H
