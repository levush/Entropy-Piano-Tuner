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

#ifndef RECORDINGSTATUSGRAPHICSVIEW_H
#define RECORDINGSTATUSGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QTimer>

#include "prerequisites.h"

#include "core/messages/messagelistener.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief The QGraphicsView that will display the current recording status.
///
/// It will draw a red circle if recording is active. A blue rotating circle
/// is drawn if the signal is analiysed. A green pause symbol is shown if the
/// recorder is inactive and ready for a new keystroke.
///////////////////////////////////////////////////////////////////////////////
class RecordingStatusGraphicsView : public QGraphicsView, protected MessageListener
{
    Q_OBJECT

    /// The scene rect of the QGraphicsScene.
    static const QRectF SCENE_RECT;

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor.
    /// \param parent : The parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    RecordingStatusGraphicsView(QWidget *parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Emptry virtual destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~RecordingStatusGraphicsView();

    virtual QSize minimumSizeHint() const override final {return QSize(20, 20);}

protected:
    virtual QSize sizeHint() const override final;
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented show event that will fit the SCENE_RECT into the view.
    /// \param event : The QShowEvent
    ///////////////////////////////////////////////////////////////////////////////
    void showEvent(QShowEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented resize event that will fit the SCENE_RECT
    ///        into the view.
    /// \param event : The QResizeEvent
    ///////////////////////////////////////////////////////////////////////////////
    void resizeEvent(QResizeEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Message handling.
    /// \param m : The message
    ///
    /// It will listen to the messages for recording and signal analysis and set
    /// the displayed graphic.
    ///////////////////////////////////////////////////////////////////////////////
    void handleMessage(MessagePtr m) override final;
private:

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to show or hide a group of items.
    /// \param items : The item group
    /// \param visible : Hide or show
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setItemVisible(const std::vector<QGraphicsItem*> &items, bool visible);

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Called by the timer to rotate the blue circle.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onProcess();

private:
    /// The graphics scene.
    QGraphicsScene mScene;

    /// Items that are used to draw the recording state.
    std::vector<QGraphicsItem* > mRecordItems;

    /// Items that are used to draw the pause state.
    std::vector<QGraphicsItem* > mPauseItems;

    /// Items that are used to draw the analysing state.
    std::vector<QGraphicsItem* > mProgressItems;

    /// Timer to shoot a onProgress() at a fixed time intervall.
    QTimer mProgressTimer;
};

#endif // RECORDINGSTATUSGRAPHICSVIEW_H
