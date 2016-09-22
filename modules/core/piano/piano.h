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
//                                Piano class
//=============================================================================

#ifndef PIANO_H
#define PIANO_H

#include <string>
#include <fstream>

#include "prerequisites.h"
#include "keyboard.h"
#include "pianodefines.h"
#include "core/calculation/algorithmparameters.h"


////////////////////////////////////////////////////////////////////////
/// \class Class that defines the properties of a piano
///
/// It contains variables such as name, serial number,
////////////////////////////////////////////////////////////////////////

class EPT_EXTERN Piano
{
public:
    static const int DEFAULT_NUMBER_OF_KEYS;
    static const int DEFAULT_KEY_NUMBER_OF_A;
    static const int DEFAULT_KEYS_ON_BASS_BRIDGE;
    static const double DEFAULT_CONCERT_PITCH;    ///< default concert pitch of A

public:
    Piano();
    ~Piano() {}

    using Keys = std::vector<Key>;

    // access functions

    void setName(const std::wstring &name) {mName = name;}
    const std::wstring &getName() const {return mName;}

    void setType(piano::PianoType type) {mType = type;}
    piano::PianoType getPianoType() const {return mType;}
    piano::PianoType &getPianoType() {return mType;}

    void setSerialNumber(const std::wstring &number) {mSerialNumber = number;}
    const std::wstring &getSerialNumber() const {return mSerialNumber;}

    void setManufactureYear(const std::wstring &year) {mManufactureYear = year;}
    const std::wstring &getManufactionYear() const {return mManufactureYear;}

    void setManufactureLocation(const std::wstring &loc) {mManufactureLocation = loc;}
    const std::wstring &getManufactionLocation() const {return mManufactureLocation;}

    void setTuningLocation(const std::wstring &loc) {mTuningLocation = loc;}
    const std::wstring &getTuningLocation() const {return mTuningLocation;}

    void setTuningTime(const std::wstring &time) {mTuningTime = time;}
    void setTuningTimeToCurrentTime();
    const std::wstring &getTuningTime() const {return mTuningTime;}

    void setConcertPitch(double pitch) {mConcertPitch = pitch;}
    const double &getConcertPitch() const {return mConcertPitch;}
    double &getConcertPitch() {return mConcertPitch;}

    const Keyboard &getKeyboard() const {return mKeyboard;}
    Keyboard &getKeyboard() {return mKeyboard;}

    const Key &getKey(int i) const {return mKeyboard[i];}
    Key &getKey(int i) {return mKeyboard[i];}
    const Key*getKeyPtr(int i) const {if (i < 0) return nullptr; else return &(mKeyboard[i]);}
    Key *getKeyPtr(int i) {if (i<0) return nullptr; else return &(mKeyboard[i]);}

    void setKey (int i, const Key &key) { mKeyboard[i] = key; }

    // other:

    double getExpectedInharmonicity (double f) const;

    double getEqualTempFrequency (int keynumber, double cents=0, double A4=0) const;

    double getDefiningTempFrequency (int keynumber, double cents=0, double A4=0) const;

    AlgorithmParameters &getAlgorithmParameters() {return mAlgorithmParameters;}
    const AlgorithmParameters &getAlgorithmParameters() const {return mAlgorithmParameters;}

private:
    /// name of the real piano
    std::wstring mName;
    /// type of the real piano
    piano::PianoType mType;
    /// serial number of the real piano
    std::wstring mSerialNumber;
    /// manufaction year of the real piano
    std::wstring mManufactureYear;
    /// location where the piano was produced
    std::wstring mManufactureLocation;

    /// location where the piano is now
    std::wstring mTuningLocation;
    /// time when the tuning hast been started (format yyyy-mm-dd HH:MM:SS, UTC)
    std::wstring mTuningTime;
    /// concert pitch (default 440 Hz)
    double mConcertPitch;

    /// the keyboard
    Keyboard mKeyboard;


    /// current parameters of the algorithms
    AlgorithmParameters mAlgorithmParameters;
};

#endif // PIANO_H
