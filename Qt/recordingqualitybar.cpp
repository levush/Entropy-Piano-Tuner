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

#include "recordingqualitybar.h"
#include <QStyleFactory>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include <iostream>
#include <cmath>
#include "../core/messages/messagefinalkey.h"
#include "../core/messages/messagekeyselectionchanged.h"

//-----------------------------------------------------------------------------
//                               Constructor
//-----------------------------------------------------------------------------

RecordingQualityBar::RecordingQualityBar(QWidget *parent) :
    QProgressBar(parent) {

    setFormat(tr("Quality"));
    setWhatsThis(tr("This bar displays the quality of the recording. All of the recorded keys should have an almost equal quality before starting the calculation."));
    setTextVisible(false);

    setRange(0, 10000);

    setStyle(QStyleFactory::create("fusion"));

    setOrientation(Qt::Vertical);
    setTextDirection(BottomToTop);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    updateValue(0);
}


//-----------------------------------------------------------------------------
//                   Functions suggesting the minimal sizes
//-----------------------------------------------------------------------------

QSize RecordingQualityBar::minimumSizeHint() const {
    QFontMetrics fm(fontMetrics());
    QRect br(fm.boundingRect(text()));
    return QSize(QProgressBar::minimumSizeHint().width(), br.width());
}

QSize RecordingQualityBar::sizeHint() const {
    QFontMetrics fm(fontMetrics());
    QRect br(fm.boundingRect(text()));
    return QSize(QProgressBar::sizeHint().width(), br.width() + 10);
}


//-----------------------------------------------------------------------------
//                      Update the quality measure bar
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief RecordingQualityBar::updateValue
///
/// Update the quality measure bar. According to the argument v in the range
/// from 0 to 1 the bar is filled from bottom to top. The color changes
/// continuously with the value from red over orange to green.
///
/// \param v : Quality between 0 and 1
///////////////////////////////////////////////////////////////////////////////

void RecordingQualityBar::updateValue(float v) {
    if (value() == v * 10000) {
        return;
    }
    setValue(v * 10000);
    QPalette p = this->palette();

    int red = static_cast<int>(255*std::pow(1-v,0.3));
    int green = static_cast<int>(255*std::pow(v,0.5));
    p.setColor(QPalette::Highlight, QColor(red,green,0));

    this->setPalette(p);

    update();
    show();
}


//-----------------------------------------------------------------------------
//                             Message handler
//-----------------------------------------------------------------------------

void RecordingQualityBar::handleMessage(MessagePtr m) {
    switch (m->getType())
    {
    case Message::MSG_FINAL_KEY:
    {
        auto message(std::static_pointer_cast<MessageFinalKey>(m));
        double quality = message->getFinalKey()->getRecognitionQuality();
        updateValue(quality);
        break;
    }
    case Message::MSG_KEY_SELECTION_CHANGED: {
        auto message(std::static_pointer_cast<MessageKeySelectionChanged>(m));
        const Key *key = message->getKey();
        if (!key) {
            updateValue(0);
        } else {
            updateValue(key->getRecognitionQuality());
        }
        break;
    }
    case Message::MSG_RECORDING_STARTED:
        updateValue(0);
        break;
    default:
        break;
    }
}


//-----------------------------------------------------------------------------
//                          Paint the quality bar
//-----------------------------------------------------------------------------

void RecordingQualityBar::paintEvent(QPaintEvent * event) {
    QProgressBar::paintEvent(event);

    QPainter painter(this);

    const QRect cr(contentsRect());

    painter.save();
    painter.translate(contentsRect().center());
    if (orientation() == Qt::Vertical) {
        painter.rotate(-90);
        painter.drawText(QRect(-cr.height() / 2, -cr.width() / 2, cr.height(), cr.width()), Qt::AlignHCenter | Qt::AlignVCenter, text());
    } else {
        painter.drawText(QRect(-cr.width() / 2, -cr.height() / 2, cr.width(), cr.height()), Qt::AlignCenter, text());
    }
    painter.restore();
}
