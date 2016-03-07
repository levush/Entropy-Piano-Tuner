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

#ifndef PAGESAVINGTABWIDGET_H
#define PAGESAVINGTABWIDGET_H

#include <QTabWidget>

#include "prerequisites.h"

///
/// \brief QTabWidget that stores the last visible page
///
/// Inherit and call restorePageFromSettings() as the last
/// command in the parent constructor after creating all tabs
///
/// It will use as QSettings path "settings/" + mID + "/page"
///
class PageSavingTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    ///
    /// \brief Constructor
    /// \param id The id of the widget
    ///
    PageSavingTabWidget(QString id);

protected slots:

    ///
    /// \brief Store the given page to the settings
    /// \param index page to store
    ///
    /// Called when the current page changed
    ///
    void storePageToSettings(int index);

    ///
    /// \brief Call this once to restore the settings
    ///
    /// It will also connect currentIndexChanged(int) to
    /// storePageToSettings(index)
    ///
    void restorePageFromSettings();

private:
    /// The id for the page used in QSettings
    const QString mID;
};

#endif // PAGESAVINGTABWIDGET_H
