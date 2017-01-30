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

#ifndef ALGORITHMPARAMETERDESCRIPTION_H
#define ALGORITHMPARAMETERDESCRIPTION_H


#include "prerequisites.h"

class EPT_EXTERN AlgorithmParameterDescription
{
public:
    using StringParameterList = std::vector<std::pair<std::string, std::wstring> >;
public:
    enum Type {
        TYPE_UNSET,
        TYPE_DOUBLE,
        TYPE_INT,
        TYPE_LIST,
        TYPE_BOOL,
    };

public:
    AlgorithmParameterDescription();

    const std::string &getID() const {return mID;}
    std::string &getID() {return mID;}

    const std::wstring &getLabel() const {return mLabel;}
    std::wstring &getLabel() {return mLabel;}

    const std::wstring &getDescription() const {return mDescription;}
    std::wstring &getDescription() {return mDescription;}

    Type getType() const {return mParameterType;}
    void setType(Type t) {mParameterType = t;}

    // double parameters
    // ----------------------------------------------------------------------------
    double getDoubleDefaultValue() const {return mDoubleDefaultValue;}
    double &getDoubleDefaultValue() {return mDoubleDefaultValue;}

    double getDoubleMinValue() const {return mDoubleMinValue;}
    double &getDoubleMinValue() {return mDoubleMinValue;}

    double getDoubleMaxValue() const {return mDoubleMaxValue;}
    double &getDoubleMaxValue() {return mDoubleMaxValue;}

    int    getDoublePrecision() const {return mDoublePrecision;}
    int    &getDoublePrecision() {return mDoublePrecision;}

    // int parameters
    // ----------------------------------------------------------------------------
    int getIntDefaultValue() const {return mIntDefaultValue;}
    int &getIntDefaultValue() {return mIntDefaultValue;}

    int getIntMinValue() const {return mIntMinValue;}
    int &getIntMinValue() {return mIntMinValue;}

    int getIntMaxValue() const {return mIntMaxValue;}
    int &getIntMaxValue() {return mIntMaxValue;}

    // string parameters
    // ----------------------------------------------------------------------------
    const std::string &getStringDefaultValue() const {return mStringDefaultValue;}
    std::string &getStringDefaultValue() {return mStringDefaultValue;}

    const StringParameterList &getStringList() const {return mStringList;}
    StringParameterList &getStringList() {return mStringList;}

    // bool parameters
    bool getBoolDefaultValue() const {return mBoolDefaultValue;}
    bool &getBoolDefaultValue() {return mBoolDefaultValue;}

    // global parameters
    // ----------------------------------------------------------------------------
    bool displayLineEdit() const {return mDisplayLineEdit;}
    bool &displayLineEdit() {return mDisplayLineEdit;}

    bool displaySpinBox() const {return mDisplaySpinBox;}
    bool &displaySpinBox() {return mDisplaySpinBox;}

    bool displaySlider() const {return mDisplaySlider;}
    bool &displaySlider() {return mDisplaySlider;}

    bool displaySetDefaultButton() const {return mDisplaySetDefaultButton;}
    bool &displaySetDefaultButton() {return mDisplaySetDefaultButton;}

    bool readOnly() const {return mReadOnly;}
    bool &readOnly() {return mReadOnly;}

    int updateIntervalInMS() const {return mUpdateIntervalInMS;}
    int &updateIntervalInMS() {return mUpdateIntervalInMS;}
    void setUpdateIntervalInMS(int i) {mUpdateIntervalInMS = i;}

private:
    Type mParameterType;
    std::string mID;

    std::wstring mLabel;
    std::wstring mDescription;

    double mDoubleDefaultValue;
    double mDoubleMinValue;
    double mDoubleMaxValue;
    int    mDoublePrecision;

    int    mIntDefaultValue;
    int    mIntMinValue;
    int    mIntMaxValue;

    std::string mStringDefaultValue;
    StringParameterList mStringList;

    bool mBoolDefaultValue;

    bool   mDisplayLineEdit;
    bool   mDisplaySpinBox;
    bool   mDisplaySlider;
    bool   mDisplaySetDefaultButton;

    bool   mReadOnly;

    int    mUpdateIntervalInMS;
};

#endif // ALGORITHMPARAMETERDESCRIPTION_H
