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

//=============================================================================
//            Class holding the program settings on the core level
//=============================================================================

#ifndef SETTINGS_H
#define SETTINGS_H

#include "prerequisites.h"
#include "audio/player/soundgenerator.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief The Settings class
///
/// This class holds and manages the piano-independent settings of the
/// application.
///
/// This class is a singleton class (one global instance)
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN Settings
{
public:
    Settings();     ///< Constructor
    ~Settings(){}   ///< Empty destructor

    static Settings &getSingleton();

    /// Get the language id (en, de, fr,-...)
    const std::string &getLanguageId() const {return mLanguageId;}
    /// Set the language id (en, de, fr,-...)
    virtual void setLanguageId(const std::string &id) {mLanguageId = id;}

    virtual std::string getUserLanguageId() const;

    /// Get the last operation mode of the sound generator
    SoundGenerator::SoundGeneratorMode getSoundGeneratorMode() const {return mSoundGeneratorMode;}
    /// Set the last operation mode of the sound generator
    virtual void setSoundGeneratorMode(SoundGenerator::SoundGeneratorMode mode) {mSoundGeneratorMode = mode;}

    /// Get flag for dynamic volume adaption in the tuning mode
    bool isSoundGeneratorVolumeDynamic() const {return mSoundGeneratorVolumeDynamic;}
    /// Set flag for dynamic volume adaption in the tuning mode
    virtual void setSoundGeneratorVolumeDynamic(bool dynamic) {mSoundGeneratorVolumeDynamic = dynamic;}

    /// Get flag disabling automatic selection of the key during tuning
    bool isAutomaticKeySelectionDisabled() const {return mDisableAutomaticKeySelection;}
    /// Set flag disabling automatic selection of the key during tuning
    virtual void setDisableAutomaticKeySelection(bool disable) {mDisableAutomaticKeySelection = disable;}

    /// Get flag indicating the stroboscopic mode of the tuning indicator
    bool isStroboscopeActive() const {return mStroboscopeActive;}
    /// Set flag indicating the stroboscopic mode of the tuning indicator
    virtual void setStroboscopeMode(bool enable) {mStroboscopeActive = enable;}

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Language Id
    ///
    /// This member variable holds the language shortcut (en, de, fr, ...).
    /// If empty the default system language will be used.
    ///////////////////////////////////////////////////////////////////////////////
    std::string mLanguageId;

    SoundGenerator::SoundGeneratorMode mSoundGeneratorMode;     ///< The sound generator mode (sine or synthesizer)
    bool mSoundGeneratorVolumeDynamic;                          ///< Flag for automatic volume adjustment
    bool mDisableAutomaticKeySelection;                         ///< Flag suppressing automatic key selection
    bool mStroboscopeActive;                                    ///< Flag indicating stroboscopic tuning indicator mode

private:
    static std::unique_ptr<Settings> mSingleton;                ///< Singleton pointer
};

#endif // SETTINGS_H
