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

#ifndef OPTIONSPAGEENVIRONMENTGENERALPAGE_H
#define OPTIONSPAGEENVIRONMENTGENERALPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>

#include "prerequisites.h"

#include "../optionstabcontentsvscrollarea.h"
#include "../optionscontentswidgetinterface.h"

namespace options {

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for general system settings
///
/// This is the class that manages the system setiings in the options dialog.
/// Options -> Environment -> General
/// Here you can choose the language and you can reactivate all warnings.
///////////////////////////////////////////////////////////////////////////////

class PageEnvironmentGeneral : public OptionsTabContentsVScrollArea, public ContentsWidgetInterface
{
    Q_OBJECT
public:
    PageEnvironmentGeneral(OptionsDialog *optionsDialog);

    void apply() override final;

private slots:
    void onReactivateWarnings();

private:
    QComboBox   *mLanguageSelection;
    QPushButton *mResetWarningsButton;
};

}  // namespace options

#endif // OPTIONSPAGEENVIRONMENTGENERALPAGE_H
