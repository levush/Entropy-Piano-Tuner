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

#ifndef VOLUMECONTROLGROUPBOX_H
#define VOLUMECONTROLGROUPBOX_H

#include <QHBoxLayout>

#include "prerequisites.h"

#include "widgets/displaysizedependinggroupbox.h"

#include "volumecontrollevel.h"

class VolumeControlGroupBox : public DisplaySizeDependingGroupBox
{
    Q_OBJECT
public:
    VolumeControlGroupBox(QWidget *parent);

private:
    VolumeControlLevel *mVolumeControlLevel = nullptr;
    QHBoxLayout *mLinesLayout = nullptr;
    QHBoxLayout *mTextLayout = nullptr;

signals:
    void refreshInputLevels();
    void muteMicroToggled(bool);
    void muteSpeakerToggled(bool);

public slots:
    void updateLevels(double stopLevel, double onLevel);

private slots:
    void onRefreshClicked();
    void onMicroMuteToggled(bool);
    void onSpeakerMuteToggled(bool);
};

#endif // VOLUMECONTROLGROUPBOX_H
