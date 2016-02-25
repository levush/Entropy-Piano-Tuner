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

#include "algorithmparameter.h"

AlgorithmParameter::AlgorithmParameter(const std::string &id,
                                       const std::string &label,
                                       const std::string &description,
                                       double defaultValue,
                                       double minValue,
                                       double maxValue,
                                       int precision,
                                       bool displayLineEdit,
                                       bool displaySpinBox,
                                       bool displaySlider,
                                       bool displaySetDefaultButton,
                                       bool readOnly) :
    mParameterType(TYPE_DOUBLE),
    mID(id),
    mLabel(label),
    mDescription(description),
    mDoubleDefaultValue(defaultValue),
    mDoubleMinValue(minValue),
    mDoubleMaxValue(maxValue),
    mDoublePrecision(precision),
    mDisplayLineEdit(displayLineEdit),
    mDisplaySpinBox(displaySpinBox),
    mDisplaySlider(displaySlider),
    mDisplaySetDefaultButton(displaySetDefaultButton),
    mReadOnly(readOnly)
{

}

AlgorithmParameter::AlgorithmParameter(const std::string &id,
                                       const std::string &label,
                                       const std::string &description,
                                       int defaultValue,
                                       int minValue,
                                       int maxValue,
                                       bool displayLineEdit,
                                       bool displaySpinBox,
                                       bool displaySlider,
                                       bool displaySetDefaultButton,
                                       bool readOnly) :
    mParameterType(TYPE_INT),
    mID(id),
    mLabel(label),
    mDescription(description),
    mIntDefaultValue(defaultValue),
    mIntMinValue(minValue),
    mIntMaxValue(maxValue),
    mDisplayLineEdit(displayLineEdit),
    mDisplaySpinBox(displaySpinBox),
    mDisplaySlider(displaySlider),
    mDisplaySetDefaultButton(displaySetDefaultButton),
    mReadOnly(readOnly)
{
}

AlgorithmParameter::AlgorithmParameter(const std::string &id,
                                       const std::string &label,
                                       const std::string &description,
                                       const std::string &defaultValue,
                                       const StringParameterList &list,
                                       bool displaySetDefaultButton) :
    mParameterType(TYPE_LIST),
    mID(id),
    mLabel(label),
    mDescription(description),
    mStringDefaultValue(defaultValue),
    mStringList(list),
    mDisplaySetDefaultButton(displaySetDefaultButton)
{
}

AlgorithmParameter::~AlgorithmParameter()
{

}

