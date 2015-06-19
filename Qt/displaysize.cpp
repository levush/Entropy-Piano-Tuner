#include "displaysize.h"
#include <QFont>

#include "core/system/eptexception.h"

std::unique_ptr<DisplaySizeDefines> DisplaySizeDefines::mSingleton;

const std::unique_ptr<DisplaySizeDefines> &DisplaySizeDefines::getSingleton() {
    EptAssert(mSingleton, "Singleton has not been created yet!");
    return mSingleton;
}

DisplaySizeDefines::DisplaySizeDefines(DisplaySize size) :
    mDisplaySize(size) {
    EptAssert(!mSingleton, "Singleton may only be created once!");
    mSingleton.reset(this);
}

DisplaySizeDefines::DisplaySizeDefines(double displaySizeInInch) :
    mDisplaySize(inchToDisplaySize(displaySizeInInch)) {
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
    QFont f;
    return f.pointSizeF();
}

double DisplaySizeDefines::getMediumIconSize() {
    return getSmallIconSize() * 1.5;
}

double DisplaySizeDefines::getLargeIconSize() {
    return getSmallIconSize() * 2;
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
    if (mDisplaySize <= DS_XSMALL) {
        return MGBDS_WIDGETS;
    } else {
        return MGBDS_NORMAL;
    }
}
