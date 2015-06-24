/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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

#include "zoomedspectrumgraphicsview.h"

ZoomedSpectrumGraphicsView::ZoomedSpectrumGraphicsView(QWidget *parent)
    : GraphicsViewAdapterForQt(parent, this, QRect(0, 0, 200, 200)),
      ZoomedSpectrumDrawer(this) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWhatsThis(tr("This is the tuning device. You should bring the peak and the indicator bar in the middle of the window for an optimal tuning. When tuing several strings at once, there might appear several peaks. All of them should be tuned to match the center."));
}

ZoomedSpectrumGraphicsView::~ZoomedSpectrumGraphicsView()
{

}
