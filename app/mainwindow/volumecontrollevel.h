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

#ifndef VOLUMECONTROLLEVEL_H
#define VOLUMECONTROLLEVEL_H

#include <QProgressBar>
#include <QTimer>

#include "prerequisites.h"

#include "core/adapters/recorderlevel.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief QProgressBar to display the current input level.
///
/// It inheriated RecorderLevel to listen for the levelChanged message.
/// Since it can happen that the rate of levelChanged(float) calls is very
/// high, there is a timer that only will update the value with 24 fps.
///////////////////////////////////////////////////////////////////////////////
class VolumeControlLevel : public QProgressBar, public RecorderLevel
{
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor.
    /// \param parent : The parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit VolumeControlLevel(QWidget *parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Empty virtual destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~VolumeControlLevel();

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function that is called when the level has changed.
    /// \param value : The new value in [0, 1]
    ///
    /// This will change the value of mValueBuffer.
    ///////////////////////////////////////////////////////////////////////////////
    void levelChanged(double value) override;

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Update the displayed value with the mValueBuffer.
    ///
    /// This function will be called by mUpdateTimer with 24 fps.
    ///////////////////////////////////////////////////////////////////////////////
    void updateValue();
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief The timer that will handle the update of the QProgressBar.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QTimer mUpdateTimer;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief The buffered value.
    ///
    /// It will be displayed by updateValue() and set by levelChanged(float).
    ///////////////////////////////////////////////////////////////////////////////
    double mValueBuffer;
};

#endif // VOLUMECONTROLLEVEL_H
