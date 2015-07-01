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

