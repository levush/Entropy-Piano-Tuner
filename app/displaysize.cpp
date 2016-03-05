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

#include "displaysize.h"
#include <QGuiApplication>
#include <QFont>
#include <QFontDatabase>

#include "core/system/eptexception.h"

std::unique_ptr<DisplaySizeDefines> DisplaySizeDefines::mSingleton;

const std::unique_ptr<DisplaySizeDefines> &DisplaySizeDefines::getSingleton() {
    EptAssert(mSingleton, "Singleton has not been created yet!");
    return mSingleton;
}

DisplaySizeDefines::DisplaySizeDefines(double displaySizeInInch, double fontPointSize) :
    mDisplaySize(inchToDisplaySize(displaySizeInInch)),
    mFontPointSize(fontPointSize) {
    EptAssert(!mSingleton, "Singleton may only be created once!");
    mSingleton.reset(this);
}

DisplaySize DisplaySizeDefines::inchToDisplaySize(double inch) {
    if (inch < 4.5) {
        return DS_XSMALL;
    } else if (inch < 7) {
        return DS_SMALL;
    } else if (inch < 12) {
        return DS_MEDIUM;
    } else {
        return DS_LARGE;
    }
}

double DisplaySizeDefines::getSmallIconSize() {
    return mFontPointSize;
}

double DisplaySizeDefines::getMediumIconSize() {
    return getSmallIconSize() * 1.5;
}

double DisplaySizeDefines::getLargeIconSize() {
    return getSmallIconSize() * 2;
}

Qt::ToolButtonStyle DisplaySizeDefines::optionsDialogToolButtonStyle() const {
    if (mDisplaySize >= DS_LARGE) {
        return Qt::ToolButtonTextBesideIcon;
    } else {
        return Qt::ToolButtonTextUnderIcon;
    }
}

bool DisplaySizeDefines::showVolumeGroupBoxInTuningMode() const {
    return mDisplaySize >= DS_SMALL;
}

bool DisplaySizeDefines::keepKeyboardRatioFixed() const {
    return mDisplaySize >= DS_MEDIUM;
}

GraphDisplayMode DisplaySizeDefines::getGraphDisplayMode() const {
    if (mDisplaySize <= DS_SMALL) {
        return GDM_ONE_VISIBLE;
    } else {
        return GDM_ALL_VISIBLE;
    }
}

KeyboardDisplayMode DisplaySizeDefines::getKeyboardDisplayMode() const {
    if (mDisplaySize <= DS_SMALL) {
        return KDM_SCROLLABLE_FIT_TO_HEIGHT;
    } else {
        return KDM_NORMAL;
    }
}

MainGroupBoxDisplayStyle DisplaySizeDefines::getMainGroupBoxDisplayStyle() const {
    if (mDisplaySize <= DS_SMALL) {
        return MGBDS_WIDGETS;
    } else {
        return MGBDS_NORMAL;
    }
}

bool DisplaySizeDefines::showPlotNavigationVertical() const {
    return mDisplaySize <= DS_SMALL;
}

bool DisplaySizeDefines::abbrevPlotLabels() const {
    return mDisplaySize <= DS_XSMALL;
}

bool DisplaySizeDefines::showMuteOutputButton() const {
    return mDisplaySize > DS_XSMALL;
}

bool DisplaySizeDefines::showMultiLineEditPianoDataSheet() const {
    return mDisplaySize < DS_SMALL;
}
