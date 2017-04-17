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

#include "recordingstatusgraphicsview.h"
#include "../core/messages/messagenewfftcalculated.h"
#include "../core/messages/messagemodechanged.h"
#include "../core/messages/messagesignalanalysis.h"
#include <QBoxLayout>
#include <QDebug>

const QRectF RecordingStatusGraphicsView::SCENE_RECT(-50, -50, 100, 100);

RecordingStatusGraphicsView::RecordingStatusGraphicsView(QWidget *parent)
    : QGraphicsView(parent),
      mScene(SCENE_RECT)
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    setWhatsThis(tr("This item displays the status of the recorder. A red circle indicates that the audio signal is currently recorded. A blue rotating circle is shown when the program processes the recorded signal. A green pause symbol is displayed if you can record the next key."));

    // no background, and no border!
    setStyleSheet("background: transparent; border: none");

    setScene(&mScene);

    // record
    mRecordItems.push_back(mScene.addEllipse(-44, -42, 91, 91, QPen(Qt::darkRed), QBrush(Qt::darkRed)));
    mRecordItems.push_back(mScene.addEllipse(-45, -45, 90, 90, QPen(), QBrush(Qt::red)));

    // pause rects
    mPauseItems.push_back(mScene.addRect(-45, -45, 35, 90, QPen(Qt::darkGreen), QBrush(Qt::darkGreen)));
    mPauseItems.push_back(mScene.addRect(-45, -45, 31, 87, QPen(Qt::lightGray), QBrush(Qt::green)));
    mPauseItems.push_back(mScene.addRect(10, -45, 35, 90, QPen(Qt::darkGreen), QBrush(Qt::darkGreen)));
    mPauseItems.push_back(mScene.addRect(10, -45, 31, 87, QPen(Qt::lightGray), QBrush(Qt::green)));


    // progress arcs
    QPainterPath path;
    path.arcMoveTo(-45, -45, 90, 90, 0);
    path.arcTo(-45, -45, 90, 90, 0, 360);
    QConicalGradient gradient(0, 0, 0);
    gradient.setColorAt(0, QColor(0, 0, 0, 255));
    gradient.setColorAt(1, QColor(0, 0, 0, 0));
    mProgressItems.push_back(mScene.addPath(path, QPen(QBrush(gradient), 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)));

    path = QPainterPath();
    path.arcMoveTo(-45, -45, 90, 90, 0);
    path.arcTo(-45, -45, 90, 90, 0, 360);
    gradient = QConicalGradient(0, 0, 0);
    gradient.setColorAt(0, QColor(50, 50, 255, 255));
    gradient.setColorAt(1, QColor(50, 50, 255, 0));
    mProgressItems.push_back(mScene.addPath(path, QPen(QBrush(gradient), 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)));

    QObject::connect(&mProgressTimer, SIGNAL(timeout()), this, SLOT(onProcess()));

    // hide all, show pause
    setItemVisible(mRecordItems, false);
    setItemVisible(mProgressItems, false);
    setItemVisible(mPauseItems, true);
}

RecordingStatusGraphicsView::~RecordingStatusGraphicsView()
{

}

QSize RecordingStatusGraphicsView::sizeHint() const {
    double h = height();
    return QSize(h, h);
}

void RecordingStatusGraphicsView::showEvent(QShowEvent *event) {
    fitInView(SCENE_RECT, Qt::KeepAspectRatio);
    QGraphicsView::showEvent(event);
}

void RecordingStatusGraphicsView::resizeEvent(QResizeEvent *event) {
    fitInView(SCENE_RECT, Qt::KeepAspectRatio);
    updateGeometry();
    QGraphicsView::resizeEvent(event);
}

void RecordingStatusGraphicsView::handleMessage(MessagePtr m) {
    switch (m->getType()) {
    case Message::MSG_SIGNAL_ANALYSIS: {
        auto msa(std::static_pointer_cast<MessageSignalAnalysis>(m));
        if (msa->status() == MessageSignalAnalysis::Status::STARTED) {
            setItemVisible(mRecordItems, false);
            setItemVisible(mPauseItems, false);
            setItemVisible(mProgressItems, true);
            mProgressTimer.start(1000.0 / 24);  // 24 fps
        } else {
            setItemVisible(mRecordItems, false);
            setItemVisible(mProgressItems, false);
            setItemVisible(mPauseItems, true);
            mProgressTimer.stop();
        }
        break;
    }
    case Message::MSG_RECORDING_STARTED:
        setItemVisible(mRecordItems, true);
        setItemVisible(mProgressItems, false);
        setItemVisible(mPauseItems, false);
        mProgressTimer.stop();
        break;
    default:
        break;
    }
}

void RecordingStatusGraphicsView::setItemVisible(const std::vector<QGraphicsItem*> &items, bool visible) {
    for (auto item : items) {
        item->setVisible(visible);
    }
}

void RecordingStatusGraphicsView::onProcess() {
    for (auto item : mProgressItems) {
        item->setRotation(item->rotation() + 5);
    }
}
