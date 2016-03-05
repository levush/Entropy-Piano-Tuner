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

#include "pagesavingtabwidget.h"
#include <QSettings>

PageSavingTabWidget::PageSavingTabWidget(QString id) :
    mID(id)
{
}

void PageSavingTabWidget::storePageToSettings(int index)
{
    QSettings settings;
    settings.setValue("settings/" + mID + "/page", index);
}

void PageSavingTabWidget::restorePageFromSettings()
{
    QObject::connect(this, SIGNAL(currentChanged(int)), this, SLOT(storePageToSettings(int)));

    QSettings settings;
    int index = settings.value("settings/" + mID + "/page", 0).toInt();

    if (index < count()) {
        setCurrentIndex(index);
    }
}
