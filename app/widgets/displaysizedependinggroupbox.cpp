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

#include "displaysizedependinggroupbox.h"
#include "displaysize.h"

DisplaySizeDependingGroupBox::DisplaySizeDependingGroupBox(QWidget *parent, QLayout *mainLayout, int operationModeFlags) :
    QWidget(parent),
    mOperationModeFlags(operationModeFlags)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    mMainWidgetContainer = this;

    if (DisplaySizeDefines::getSingleton()->getMainGroupBoxDisplayStyle() == MGBDS_NORMAL) {
        // on normal displays draw a group box around the data
        QVBoxLayout *dummyLayout = new QVBoxLayout;
        this->setLayout(dummyLayout);
        dummyLayout->setMargin(0);
        mGroupBox = new QGroupBox;
        dummyLayout->addWidget(mGroupBox);
        mMainWidgetContainer = mGroupBox;
    } else {
        mainLayout->setMargin(0);
    }

    mMainWidgetContainer->setLayout(mainLayout);
}

void DisplaySizeDependingGroupBox::setTitle(QString title) {
    if (mGroupBox) {
        mGroupBox->setTitle(title);
    }
}

void DisplaySizeDependingGroupBox::onModeChanged(OperationMode mode) {
    setVisible((mOperationModeFlags & toFlag(mode)) > 0);
}

