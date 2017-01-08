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

#ifndef PROGRESSOVERLAY_H
#define PROGRESSOVERLAY_H

#include <QFrame>
#include <QLabel>
#include <QProgressBar>

#include "prerequisites.h"


///////////////////////////////////////////////////////////////////////////////
/// \brief The ProgressOverlay class shows a small progress indicator
///
/// This indicator is on the lower right side of the parent widget
/// It is resized automatically depending on its content.
/// It is invisible to mouse events.
class ProgressOverlay :
        public QFrame
{
    Q_OBJECT
public:
    ///
    /// \brief The constuctor
    /// \param parent the parent widget
    /// \param label the label to be shown
    /// \param deleteOnFinish indicates whether the widget shall delete itself
    ///        if it reaches 100%
    ///
    /// This setups the ProgressDialog as an overlay
    ProgressOverlay(QWidget *parent, const QString &label = QString(), bool deleteOnFinish = true);

private:
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *o, QEvent *e);

public slots:
    void setText(QString label);

    ///
    /// \brief Updates the percentage of the  progress bar
    /// \param percentage the percentage to display
    ///
    void updatePercentage(int percentage);

private slots:
    ///
    /// \brief Updates the position of the window
    ///
    ///
    /// This is called if its own size changed or the size of the parent.
    /// See eventFilter(QObject *, QEvent *)
    ///
    void updatePosition();


    ///
    /// \brief Checks if the ProgressDialog shall be shown or hidden
    ///
    /// This function is usually called if a sub widget changed its visibility
    /// e.g. updateWGPercentage(int)
    ///
    void updateVisibility();

private:
    QLabel *mLabel;              ///< The label
    QProgressBar *mProgressBar;  ///< The progress bar
    bool mDeleteOnFinish;        ///< Delete widget if finished
};

#endif // PROGRESSOVERLAY_H
