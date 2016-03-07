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

#ifndef OPTIONSPAGEENVIRONMENTTUNINGPAGE_H
#define OPTIONSPAGEENVIRONMENTTUNINGPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>

#include "prerequisites.h"

#include "../optionscontentswidgetinterface.h"
#include "../optionstabcontentsvscrollarea.h"

namespace options {

class PageEnvironmentTuning : public OptionsTabContentsVScrollArea, public ContentsWidgetInterface
{
    Q_OBJECT
public:
    PageEnvironmentTuning (OptionsDialog *optionsDialog);

    void apply() override final;

private:
    QComboBox *mSynthesizerModeComboBox;
    QCheckBox *mSynthesizerVolumeDynamicCheckBox;
    QCheckBox *mStroboscopeCheckBox;
    QCheckBox *mDisableAutomaticKeySelecetionCheckBox;
};

}  // namespace options

#endif // OPTIONSPAGEENVIRONMENTTUNINGPAGE_H
