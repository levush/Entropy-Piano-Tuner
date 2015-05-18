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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <memory>
#include <string>

#include "piano/soundgeneratormode.h"

class Settings
{
public:
    Settings();
    ~Settings();

    static Settings &getSingleton();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for mLanguageId.
    /// \return mLanguageId
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const std::string &getLanguageId() const {return mLanguageId;}
    virtual std::string getUserLanguageId() const;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Setter function for mLanguageId.
    /// \param s : The new lanuage id.
    ///
    /// This will automatically store its new value to the QSettings.
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setLanguageId(const std::string &id) {mLanguageId = id;}

    const std::string &getLastUsedAlgorithm() const {return mLastUsedAlgorithm;}
    virtual void setLastUsedAlgorithm(const std::string &name) {mLastUsedAlgorithm = name;}

    SoundGeneratorMode getSoundGeneratorMode() const {return mSoundGeneratorMode;}
    virtual void setSoundGeneratorMode(SoundGeneratorMode mode) {mSoundGeneratorMode = mode;}

    bool isSoundGeneratorVolumeDynamic() const {return mSoundGeneratorVolumeDynamic;}
    virtual void setSoundGeneratorVolumeDynamic(bool dynamic) {mSoundGeneratorVolumeDynamic = dynamic;}

protected:

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief the language id
    /// If empty the default system language will be used
    /// allowed are e.g. (de, en)
    ///////////////////////////////////////////////////////////////////////////////
    std::string mLanguageId;

    std::string mLastUsedAlgorithm;
    SoundGeneratorMode mSoundGeneratorMode;
    bool mSoundGeneratorVolumeDynamic;

private:
    static std::unique_ptr<Settings> mSingleton;
};

#endif // SETTINGS_H
