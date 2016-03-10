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
//                         Quality recording bar
//=============================================================================

#ifndef RECORDINGQUALITYBAR_H
#define RECORDINGQUALITYBAR_H

#include <QProgressBar>

#include "prerequisites.h"

#include "core/messages/messagelistener.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Quality recording bar
///
/// In the Signal-Analyzer panel the tuner application shows a vertical
/// progress bar which indicates the quality of the recorded key. The quality
/// is a number between 0 and 1 which is computed heuristically by the
/// FFTAnalyzer and can be interpreted as a measure how nicely the observed
/// partials match with the usual inharmonicity formula.
///////////////////////////////////////////////////////////////////////////////

class RecordingQualityBar : public QProgressBar, public MessageListener
{
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    /// \param parent : The parent widget.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    RecordingQualityBar(QWidget *parent = nullptr);


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Empty destructor.
    ///////////////////////////////////////////////////////////////////////////////
    ~RecordingQualityBar() {}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Hint for the minimal widget size.
    /// \return QSize hint.
    ///
    /// The minimum size is set by the minimum space the text requires.
    ///////////////////////////////////////////////////////////////////////////////
    virtual QSize minimumSizeHint() const override final;


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Hint for the optimal widget size.
    /// \return QSize hint.
    ///
    /// The optimal size is the minimuimSizeHint() with a small padding.
    ///////////////////////////////////////////////////////////////////////////////
    virtual QSize sizeHint() const override final;

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to update the current value.
    /// \param v : The new value to display
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void updateValue (float v);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Message handling.
    /// \param m : The message.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void handleMessage(MessagePtr m) override final;


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented paint function that will draw the vertical text.
    /// \param event : The QPaintEvent.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void paintEvent(QPaintEvent *event) override final;
};

#endif // RECORDINGQUALITYBAR_H
