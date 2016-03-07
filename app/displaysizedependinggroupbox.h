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

#ifndef DISPLAYSIZEDEPENDINGGROUPBOX_H
#define DISPLAYSIZEDEPENDINGGROUPBOX_H

#include <QGroupBox>
#include <QLayout>

#include "prerequisites.h"

class DisplaySizeDependingGroupBox : public QWidget
{
    Q_OBJECT
public:
    DisplaySizeDependingGroupBox(QWidget *parent, QLayout *mainLayout, int operationModeFlags);

    void setTitle(QString title);


protected:
    QGroupBox *mGroupBox = nullptr;

    QWidget *mMainWidgetContainer = nullptr;

private:
    const int mOperationModeFlags = 0;

protected slots:
    virtual void onModeChanged(OperationMode mode);
};

#endif // DISPLAYSIZEDEPENDINGGROUPBOX_H
