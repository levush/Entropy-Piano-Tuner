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

//=============================================================================
//                          Draw the tuning curve
//=============================================================================

#include "tuningcurvegraphdrawer.h"

#include <iostream>

#include "../system/eptexception.h"
#include "../system/log.h"
#include "../messages/messagehandler.h"
#include "../messages/messageprojectfile.h"
#include "../messages/messagemodechanged.h"
#include "../messages/messagechangetuningcurve.h"
#include "../messages/messagekeyselectionchanged.h"
#include "../messages/messagekeydatachanged.h"
#include "../math/mathtools.h"
#include "../piano/piano.h"
#include "../core/system/log.h"


// Various pen types used by this drawer:

const PenType TuningCurveGraphDrawer::gridcolor  = GraphicsViewAdapter::PEN_THIN_DARK_GRAY;
const PenType TuningCurveGraphDrawer::middleline = GraphicsViewAdapter::PEN_MEDIUM_DARK_GRAY;
const PenType TuningCurveGraphDrawer::centlines  = GraphicsViewAdapter::PEN_THIN_VERY_LIGHT_GRAY;
const PenType TuningCurveGraphDrawer::bmarkers   = GraphicsViewAdapter::PEN_MEDIUM_LIGHT_GRAY;
const PenType TuningCurveGraphDrawer::frmarkers  = GraphicsViewAdapter::PEN_THIN_BLUE;
const PenType TuningCurveGraphDrawer::fgmarkers  = GraphicsViewAdapter::PEN_THIN_LIGHT_GRAY;
const PenType TuningCurveGraphDrawer::fcmarkers  = GraphicsViewAdapter::PEN_MEDIUM_DARK_GREEN;
const PenType TuningCurveGraphDrawer::ftmarkers  = GraphicsViewAdapter::PEN_MEDIUM_RED;
const PenType TuningCurveGraphDrawer::opmarkers  = GraphicsViewAdapter::PEN_MEDIUM_MAGENTA;
const FillType TuningCurveGraphDrawer::allowdAreaFill = GraphicsViewAdapter::FILL_LIGHT_GREEN;

///////////////////////////////////////////////////////////////////////////////
/// \brief Constant, defining the visible range above and below the equal
/// temperament level in cents.
///////////////////////////////////////////////////////////////////////////////

const int TuningCurveGraphDrawer::rangeInCents = 60;


//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, calls the DrawerBase constructor and resets variables.
///////////////////////////////////////////////////////////////////////////////

TuningCurveGraphDrawer::TuningCurveGraphDrawer(GraphicsViewAdapter *graphics)
    : DrawerBase(graphics, 1.0),
      mPiano(nullptr),
      mConcertPitch(0),
      mKeyNumberOfA4(0),
      mNumberOfKeys(0),
      mOperationMode(MODE_COUNT)
{
}


//-----------------------------------------------------------------------------
//                      Message dispatcher
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Message listener and dispatcher
///
/// This function is called by the message handler and takes action in the
/// tuning curve window according to the incoming messages.
/// \param m : Pointer to the incoming message
///////////////////////////////////////////////////////////////////////////////

void TuningCurveGraphDrawer::handleMessage(MessagePtr m)
{
    EptAssert(m, "Message has to exist");

    switch (m->getType())
    {
        case Message::MSG_PROJECT_FILE:
        {
            // get the piano data and its most important parameters:
            auto mpf(std::static_pointer_cast<MessageProjectFile>(m));
            mPiano = &mpf->getPiano();
            mConcertPitch  = mPiano->getConcertPitch();
            mNumberOfKeys  = mPiano->getKeyboard().getNumberOfKeys();
            mKeyNumberOfA4 = mPiano->getKeyboard().getKeyNumberOfA4();
            redraw();
            break;
        }
        case Message::MSG_KEY_DATA_CHANGED:
        {
            // upate the marker positions upon key data change
            auto mkdc(std::static_pointer_cast<MessageKeyDataChanged>(m));
            int index = mkdc->getIndex();
            updateMarkerPosition(index, ROLE_COMPUTED_FREQUENCY);
            updateMarkerPosition(index, ROLE_INHARMONICITY);
            updateMarkerPosition(index, ROLE_RECORDED_FREQUENCY);
            updateMarkerPosition(index, ROLE_TUNED_FREQUENCY);
            updateMarkerPosition(index, ROLE_OVERPULL);
            break;
        }
        case Message::MSG_CLEAR_RECORDING:
        {
            redraw(true);
            break;
        }
        case Message::MSG_MODE_CHANGED:
        {
            auto mmc(std::static_pointer_cast<MessageModeChanged>(m));
            mOperationMode = mmc->getMode();
            redraw(true);
            break;
        }
        default:
            break;
    }
}


//-----------------------------------------------------------------------------
//                   Convert cents to Y-coordinate
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief  Convert cents to y-coordinate between 0 and 1
///
/// This function converts a given deviation in cent to the corresponding
/// y-coordinate in the tuning curve window. In the tuning mode this map is
/// nonlinear, amplifying the vicinity of optimal tune visually, while in all
/// other operation modes the map is linear.
/// \param cents : Deviation in cents.
/// \return Corresponding y-coordinate
///////////////////////////////////////////////////////////////////////////////

double TuningCurveGraphDrawer::convertCentsToY (double cents)
{
    if (cents>rangeInCents) return 0;
    else if (cents<-rangeInCents) return 1;
    else if (mOperationMode == MODE_TUNING)
        return 0.5 - 0.5 / atan(3.1) * atan(3*cents/rangeInCents);
    else return 0.5 - 0.5*cents/rangeInCents;
}

//-----------------------------------------------------------------------------
//                   Manually edit tuning curve by mouse click
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief  Manually edit tuning curve by mouse click
///
/// In the calculation mode this function allows the user to manually edit
/// the computed tuning curve by clicking into the tuning curve window.
/// In the recording and tuning mode this functionality is used to
/// select the corresponding key.
/// \param relX : Relative x-coordinate of the mouse click.
/// \param relY : Relative y-coordinate of the mouse click.
///////////////////////////////////////////////////////////////////////////////

void TuningCurveGraphDrawer::manuallyEditTuningCurveByClick (double relX, double relY)
{
    if (relX < 0 or relX > 1 or relY < 0 or relY > 1) return; // out of range

    int keynumber = (int)(mNumberOfKeys * relX);
    if (keynumber < 0 or keynumber >= mNumberOfKeys) return;

    if (keynumber == mKeyNumberOfA4)
    {
        LogI("The pitch of A4 cannot be changed manually.");
        return;
    }
    double yfrac=1 - relY;
    int cents = MathTools::roundToInteger(rangeInCents * (2 * yfrac - 1));
    LogW("Set tuningcurve manually keynumber=%d, cents=%d.",keynumber,cents);
    MessageHandler::send<MessageChangeTuningCurve>
            (keynumber,mPiano->getDefiningTempFrequency(keynumber,cents,440));
}


//-----------------------------------------------------------------------------
//                       Draw a grid in the background
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Draw the background grid in the tuning curve panel.
///
/// This function draws the background grid consisting of gray vertical lines
/// according to the key positions and thin horizontal lines spaced by 10 cents.
/// The middle line is emphasized.
///////////////////////////////////////////////////////////////////////////////

void TuningCurveGraphDrawer::drawGrid ()
{
    // Draw a horizontal background line for each 10 cents
    for (int cents=-rangeInCents+1; cents<rangeInCents; cents++)
        if (cents%10==0 or (abs(cents)<10 and mOperationMode==MODE_TUNING))
        {
            double y = convertCentsToY(cents);
            auto type = (cents ? centlines : middleline);
            mGraphics->drawLine(0, y, 1, y, type)->setItemRole(ROLE_GRID);
        }
    // Draw vertical lines according to the keys
    if (mNumberOfKeys > 0) {
        const double dx = 1.0 / mNumberOfKeys;
        for (int i = 0; i <= mNumberOfKeys; i++)
            mGraphics->drawLine(i*dx, 0,i*dx, 1, gridcolor)->setItemRole(ROLE_GRID);
    }

    // Draw green background in tuning mode
    if (mOperationMode == MODE_TUNING) {
        const double allowedDeviationInCents = 3;
        double allowedStartCents = convertCentsToY(allowedDeviationInCents);
        GraphicsItem *item = mGraphics->drawFilledRect(0, allowedStartCents, 1,
                             convertCentsToY(-allowedDeviationInCents) - allowedStartCents,
                             GraphicsViewAdapter::PEN_THIN_TRANSPARENT, allowdAreaFill);
        item->setItemRole(ROLE_GRID);
        item->setZOrder(-1);
    }
}


//-----------------------------------------------------------------------------
//                    Draw the markers of the tuning curve
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Draw the tuning markers in the tuning curve panel.
/// \param keynumber : Number of the key
///////////////////////////////////////////////////////////////////////////////

void TuningCurveGraphDrawer::drawMarkers(int8_t keynumber)
{
    if (!mPiano) return;  // no keys set yet (see setKeys)

    // Draw inharmonicity levels
    updateMarkerPosition (keynumber, ROLE_INHARMONICITY);

    // Draw tuning curve
    updateMarkerPosition (keynumber, ROLE_COMPUTED_FREQUENCY);

    // Draw recorded frequency levels
    updateMarkerPosition (keynumber, ROLE_RECORDED_FREQUENCY);

    // Draw tuned frequency levels
    updateMarkerPosition (keynumber, ROLE_TUNED_FREQUENCY);

    // Draw tuned frequency levels
    updateMarkerPosition (keynumber, ROLE_OVERPULL);
}


//-----------------------------------------------------------------------------
//                           Main drawing function
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Main drawing function
///
/// This function draws the background grid and the markers
/// in the tuning curve panel.
///////////////////////////////////////////////////////////////////////////////

void TuningCurveGraphDrawer::draw()
{
    drawGrid();
    for (int i = 0; i < mNumberOfKeys; i++) drawMarkers(i);
}


//-----------------------------------------------------------------------------
//                     Compute the position of a marker
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute the position of a marker.
///
/// This function computes the vertical position (the y-coordinate) of a
/// marker. The y-coordinate depends on the role of the element (such as
/// recorded, computed and tuned frequency as well as the inharmonicity)
/// and the actual operation mode of the tuner.
///
/// \param keyindex : Number of the key where the marker is located.
/// \param role : RoleType of the marker.
/// \return y-coordinate between 0 and 1.
///////////////////////////////////////////////////////////////////////////////

double TuningCurveGraphDrawer::getMarkerPosition(int keyindex, RoleType role)
{
    // Get the key data of the piano
    if (not mPiano) return -1;
    const Key &key = mPiano->getKey(keyindex);

    // lambda function converting a frequency ratio to cents
    auto ratioToCents = [this](double ratio)
                        { return 1200*log(ratio)/MathTools::LOG2; };

    if (role & ROLE_INHARMONICITY)
    {
        if (mOperationMode == MODE_RECORDING)
        {
           double B = key.getMeasuredInharmonicity();
           if (B > 0)return (log(B)+10.5)/8;
        }
    }
    else if (role & ROLE_COMPUTED_FREQUENCY)
    {
        if (mOperationMode == MODE_TUNING) return 0.5; // horizontal line
        else if (mOperationMode == MODE_CALCULATION)
        {
            double f = mPiano->getDefiningTempFrequency(keyindex,0,440);
            return convertCentsToY(ratioToCents(key.getComputedFrequency() / f));
        }
    }
    else if (role & ROLE_RECORDED_FREQUENCY)
    {
        if (mOperationMode == MODE_RECORDING or mOperationMode == MODE_IDLE)
        {
            double ratio = key.getRecordedFrequency() /
                           mPiano->getEqualTempFrequency(keyindex);
            if (ratio > 0) return convertCentsToY(ratioToCents(ratio));
        }
        else if (mOperationMode == MODE_CALCULATION)
        {
            int keyA4 = mPiano->getKeyboard().getKeyNumberOfA4();
            double fA4 = mPiano->getKey(keyA4).getRecordedFrequency();
            double ratio = key.getRecordedFrequency() /
                           mPiano->getEqualTempFrequency(keyindex,0,fA4);
            if (ratio > 0) return convertCentsToY(ratioToCents(ratio));
        }
    }
    else if (role & ROLE_TUNED_FREQUENCY)
    {
        if (mOperationMode == MODE_TUNING)
        {
            double ratio = key.getTunedFrequency() /
                           key.getComputedFrequency() * 440.0 / mConcertPitch;
            if (ratio > 0) return convertCentsToY(ratioToCents(ratio));
        }
        else if (mOperationMode == MODE_IDLE)
        {
            double ratio = key.getTunedFrequency() /
                           mPiano->getEqualTempFrequency(keyindex);
            if (ratio > 0) return convertCentsToY(ratioToCents(ratio));
        }
    }
    else if (role & ROLE_OVERPULL)
    {
        if (mOperationMode == MODE_TUNING)
        {
            double overpull = key.getOverpull();
            if (fabs(overpull)>0.1) return convertCentsToY(overpull);
        }
    }
    return -1;
}


//-----------------------------------------------------------------------------
//             Get the pen type of a marker with a given role
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the pen type of a marker with a given role
///
/// This function maps a role of a marker (see enum definded above) onto
/// the required pen type (thickness and color) depending on the actual
/// operation mode.
/// \param role : Role of the marker (see enum RoleType).
/// \return PenType of the marker depending on the actual operation mode.
///////////////////////////////////////////////////////////////////////////////

PenType TuningCurveGraphDrawer::getMarkerPenType(RoleType role)
{
    if      (role & ROLE_INHARMONICITY) return bmarkers;
    else if (role & ROLE_COMPUTED_FREQUENCY) return fcmarkers;
    else if (role & ROLE_TUNED_FREQUENCY) return ftmarkers;
    else if (role & ROLE_OVERPULL) return opmarkers;
    else if (role & ROLE_RECORDED_FREQUENCY)
        return (mOperationMode == MODE_CALCULATION ? fgmarkers : frmarkers);
    else EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Color type not implemented");
}


//-----------------------------------------------------------------------------
//                  Update the position of a marker
//-----------------------------------------------------------------------------

void TuningCurveGraphDrawer::updateMarkerPosition(int keyindex, RoleType role)
{
    // Compute the y coordinate where the marker should be
    double y = getMarkerPosition(keyindex, role);

    // See whether the marker already exists
    GraphicsItem *item = mGraphics->getGraphicItem(keyindex, role);
    if (item)
    {
        // if y-range valid upate position, otherwise delete it
        if (y > 0 and y < 1)
            item->setPosition(static_cast<double>(keyindex) / mNumberOfKeys, y);
        else delete item;
    }
    else // if the marker does not exist
    {
        // compute x-coordinate (y-coordinate already exists)
        double x  = (double)(keyindex) / mNumberOfKeys;
        double dx = 1.0 / mNumberOfKeys;
        if (y > 0 && y < 1)
        {
            item = mGraphics->drawLine(x, y, x + dx, y, getMarkerPenType(role));
            item->setKeyIndexAndItemRole(keyindex, role);
        }
    }
}



