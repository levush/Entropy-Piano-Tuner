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

#ifndef DISPLAYSIZE
#define DISPLAYSIZE

#include <memory>
#include <qnamespace.h>

#include "prerequisites.h"

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

    bool isLEq(DisplaySize ds) const {return ds <= mDisplaySize;}
    bool isGEq(DisplaySize ds) const {return ds >= mDisplaySize;}

    double getSmallIconSize();
    double getMediumIconSize();
    double getLargeIconSize();

    Qt::ToolButtonStyle optionsDialogToolButtonStyle() const;
    bool showVolumeGroupBoxInTuningMode() const;
    bool keepKeyboardRatioFixed() const;
    GraphDisplayMode getGraphDisplayMode() const;
    KeyboardDisplayMode getKeyboardDisplayMode() const;
    MainGroupBoxDisplayStyle getMainGroupBoxDisplayStyle() const;

    bool showPlotNavigationVertical() const;
    bool abbrevPlotLabels() const;
    bool showMuteOutputButton() const;
    bool showMultiLineEditPianoDataSheet() const;

};

#endif // DISPLAYSIZE

