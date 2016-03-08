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

#ifndef TUNINGCURVEGRAPHADAPTER_H
#define TUNINGCURVEGRAPHADAPTER_H

#include "prerequisites.h"
#include "drawerbase.h"
#include "../piano/piano.h"
#include "../messages/messagelistener.h"

#include <cstdint>

using PenType = GraphicsViewAdapter::PenType;
using FillType = GraphicsViewAdapter::FillTypes;
using RoleType = GraphicsItem::RoleType;

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for drawing various tuning curves
///
/// This drawer class take draws various curves, namely, the recorded pitch
/// compared to equal temperament, the inharmonicity, the computed pitch
/// and the tuning deviations. In order to update the markers individually,
/// a different role is assigned to each marker.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN TuningCurveGraphDrawer : public DrawerBase, public MessageListener
{
protected:
    static const int rangeInCents;

private:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The ItemRoles used in the TuningCurveGraphDrawer
    ///
    /// This enum defines the possible roles of the marker. It is encoded
    /// in terms of dual numbers so that bitwise logical operations are
    /// possible.
    ///////////////////////////////////////////////////////////////////////////

    enum ItemRoles
    {
        ROLE_GRID               = 0x01,     ///< Grid line
        ROLE_INHARMONICITY      = 0x02,     ///< Inharmonicity marker (shown upside down)
        ROLE_RECORDED_FREQUENCY = 0x04,     ///< Marker indicating recorded frequency
        ROLE_COMPUTED_FREQUENCY = 0x08,     ///< Marker indicating computed frequency
        ROLE_TUNED_FREQUENCY    = 0x10,     ///< Marker indicating tuned frequency
        ROLE_OVERPULL           = 0x20,     ///< Marker indicating overpull

        ROLE_GLOBAL = ROLE_GRID,            ///< Union of all global markers
        ROLE_KEY_INDIVIDUAL = ROLE_INHARMONICITY | ROLE_RECORDED_FREQUENCY |
                              ROLE_COMPUTED_FREQUENCY | ROLE_TUNED_FREQUENCY
                                            ///< Union of all key-specific markers
    };

public:
    TuningCurveGraphDrawer(GraphicsViewAdapter *graphics);  // Constructor
    ~TuningCurveGraphDrawer(){}                             ///< Empty destructor

    // Modify the tuning curve by mouse clicks:
    void manuallyEditTuningCurveByClick (double relX, double relY);

    OperationMode getOperationMode() const {return mOperationMode;}
    const Piano *getPiano() const {return mPiano;}
    int getNumberOfKeys() const {return mNumberOfKeys;}

protected:
    virtual void draw() final;
    virtual void handleMessage(MessagePtr m) override;

private:
    static const PenType gridcolor;         ///< Pen type for background grid pentype
    static const PenType middleline;        ///< Pen type for bold middle line of grid
    static const PenType centlines;         ///< Pen type for thin grid indicating cents
    static const PenType bmarkers;          ///< Pen type for inharmonicity markers
    static const PenType frmarkers;         ///< Pen type for recorded frequency markers
    static const PenType fgmarkers;         ///< Pen type for recorded frequency in background
    static const PenType fcmarkers;         ///< Pen type for computed frequency markers
    static const PenType ftmarkers;         ///< Pen type for tuned frequency markers
    static const PenType opmarkers;         ///< Pen type for tuned frequency markers
    static const FillType allowdAreaFill;   ///< Filling for the allowed tuning area

    const Piano *mPiano;                    ///< Pointer to the actual piano
    double      mConcertPitch;              ///< Chosen concert pitch in Hz
    int         mKeyNumberOfA4;             ///< Index of A4 (reference key)
    int         mNumberOfKeys;              ///< Total number of keys (88)

    OperationMode mOperationMode;           ///< Current operation mode

    // Private functions:
    double convertCentsToY (double cents);
    double getMarkerPosition(int keyindex, RoleType role);
    void drawMarkers(int8_t key);
    void drawGrid ();
    PenType getMarkerPenType(RoleType role);
    void updateMarkerPosition(int keyindex, RoleType role);
};

#endif // TUNINGCURVEGRAPHADAPTER_H
