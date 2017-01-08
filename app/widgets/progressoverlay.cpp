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

#include "progressoverlay.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QFontMetrics>

ProgressOverlay::ProgressOverlay(QWidget *parent, const QString &label, bool deleteOnFinish) :
    QFrame(parent),
    mDeleteOnFinish(deleteOnFinish)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    class FixedSizeLabel : public QLabel
    {
    public:
        FixedSizeLabel(QString s) :
            QLabel(s)
        {
            QRect br(fontMetrics().boundingRect(text()));
            setFixedSize(br.size());
        }
    };

    class FixedSizeProgressBar : public QProgressBar
    {
    public:
        FixedSizeProgressBar()
        {
            QRect br(fontMetrics().boundingRect('A'));
            setFixedHeight(br.height());
            setTextVisible(false);
        }

    };

    layout->addWidget(mLabel = new FixedSizeLabel(label));
    layout->addWidget(mProgressBar = new FixedSizeProgressBar);

    parent->installEventFilter(this);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    setAutoFillBackground(true);
    setFrameShape(QFrame::StyledPanel);
    show();

    adjustSize();

    updatePercentage(0);
}


void ProgressOverlay::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);

    updatePosition();
}

bool ProgressOverlay::eventFilter(QObject *o, QEvent *e)
{
    Q_UNUSED(o);

    if (e->type() == QEvent::Resize) {
        updatePosition();
    }
    return false;
}

void ProgressOverlay::setText(QString label)
{
    mLabel->setText(label);
}

void ProgressOverlay::updatePercentage(int percentage)
{
    mProgressBar->setValue(percentage);

    if (percentage >= 100) {
        if (mDeleteOnFinish) {
            this->deleteLater();
        }
        mProgressBar->hide();
        mLabel->hide();
    } else {
        mProgressBar->show();
        mLabel->show();
    }

    updateVisibility();
    adjustSize();
}

void ProgressOverlay::updateVisibility()
{
    if (!mProgressBar->isHidden()) {
        show();
    } else {
        hide();
    }
}

void ProgressOverlay::updatePosition()
{
    QSize pos(parentWidget()->size() - size());
    move(pos.width(), pos.height());
}
