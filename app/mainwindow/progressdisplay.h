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

#ifndef PROGRESSDISPLAY_H
#define PROGRESSDISPLAY_H

#include <QFrame>
#include <QLabel>
#include <QProgressBar>

#include "prerequisites.h"

#include "core/audio/player/waveformgenerator.h"


///////////////////////////////////////////////////////////////////////////////
/// \brief The ProgressDisplay class shows a small progress indicator
///
/// This indicator is on the lower right side of the screen.
/// It is resized automatically depending on its content.
/// It is invisible to mouse events.
/// It displays the progress of the waveformgenerator (percentage of completed)
class ProgressDisplay :
        public QFrame,
        public WaveformGeneratorStatusCallback
{
    Q_OBJECT
public:
    ///
    /// \brief The constuctor
    /// \param mainWindow pointer to the main window as parent
    ///
    /// This setups the ProgressDialog as an overlay
    ProgressDisplay(QWidget *mainWindow);

private:
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *o, QEvent *e);

    void queueSizeChanged(size_t size, size_t total_size);
signals:
    ///
    /// \brief Emitted by queueSizeChanged(size_t, size_t)
    /// \param percentage the new percentage to display
    ///
    /// Required function to enable thread safety
    void wgPercentageChanged(int percentage);

public slots:
    ///
    /// \brief Updates the position of the window
    ///
    ///
    /// This is called if its own size changed or the size of the parent.
    /// See eventFilter(QObject *, QEvent *)
    ///
    void updatePosition();

private slots:
    ///
    /// \brief Updates the percentage of the waveformgenerator progress bar
    /// \param percentage the percentage to display
    /// \see wgPercentageChanged(int)
    ///
    /// It will automatically hide the waveform generator elements if the
    /// percentage is greater or equal than 100.
    /// Afterwards it will check the visibility of the complete ProgressDialog
    /// via updateVisibility()
    void updateWGPercentage(int percentage);

    ///
    /// \brief Checks if the ProgressDialog shall be shown or hidden
    ///
    /// This function is usually called if a sub widget changed its visibility
    /// e.g. updateWGPercentage(int)
    ///
    void updateVisibility();

private:
    QLabel *mWaveformGeneratorStatusLabel;          ///< The label of the waveformgenerator
    QProgressBar *mWaveformGeneratorStatusBar;      ///< The progress bar of the waveformgenerator
};

#endif // PROGRESSDISPLAY_H
