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

#include "volumecontrollevel.h"
#include <QLabel>
#include <QDebug>

VolumeControlLevel::VolumeControlLevel(QWidget *parent)
    : QProgressBar(parent),
      mValueBuffer(0)
{
    QObject::connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateValue()));
    mUpdateTimer.start(1000 / 24);  // 24 fps
    setMaximum(100000);
    setWhatsThis(tr("This bar displays the current input level."));
}

VolumeControlLevel::~VolumeControlLevel()
{
    mUpdateTimer.stop();
}


void VolumeControlLevel::levelChanged(double value)
{
    mValueBuffer = std::max(0.0, std::min(value, 1.0)) * maximum();
}

void VolumeControlLevel::updateValue()
{
    setValue(mValueBuffer);
    repaint();
}
