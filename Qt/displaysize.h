#ifndef DISPLAYSIZE
#define DISPLAYSIZE

#include <memory>

enum DisplaySize {
    DS_XSMALL  = 1,          //< Normal smartphones up to 4.5'
    DS_SMALL   = 2,          //< Large smartphones down to 4.5'
    DS_MEDIUM  = 3,          //< Medium size, usually tablets down to 7'
    DS_LARGE   = 4,          //< Normal size, suitable down to 12'
};

enum GraphDisplayMode {
    GDM_ALL_VISIBLE,         //< all graphs are visibly permanent
    GDM_ONE_VISIBLE,         //< only show one graph at the same time
};

enum KeyboardDisplayMode {
    KDM_NORMAL,                     //< normal keyboard: completly visible, klick to select
    KDM_POPUP,                      //< completly visible, click opens a fullscreen keyboard
    KDM_SCROLLABLE_FIT_TO_HEIGHT,   //< only part visible, fit to available height, scrollable
};

enum MainGroupBoxDisplayStyle {
    MGBDS_NORMAL,                   //< normal group boxes to display
    MGBDS_WIDGETS,                  //< no group boxes, use widgets instead
};

class DisplaySizeDefines {
private:
    static std::unique_ptr<DisplaySizeDefines> mSingleton;
    const DisplaySize mDisplaySize;
    const double mFontPointSize;

public:
    DisplaySizeDefines(double displaySizeInInch, double fontPointSize);

    static DisplaySize inchToDisplaySize(double inch);
    static const std::unique_ptr<DisplaySizeDefines> &getSingleton();

    double getSmallIconSize();
    double getMediumIconSize();
    double getLargeIconSize();

    bool keepKeyboardRatioFixed() const;
    GraphDisplayMode getGraphDisplayMode() const;
    KeyboardDisplayMode getKeyboardDisplayMode() const;
    MainGroupBoxDisplayStyle getMainGroupBoxDisplayStyle() const;
};

#endif // DISPLAYSIZE

