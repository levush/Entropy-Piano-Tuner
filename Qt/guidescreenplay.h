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

#ifndef GUIDESCREENPLAY_H
#define GUIDESCREENPLAY_H

#include "../core/messages/messagelistener.h"

class MainWindow;

/// \brief This class will handle the display of guide message boxes
class GuideScreenplay : public MessageListener
{
    // use negative values, since positive values match the id of the next guide box
    enum GuideStatus {
        STATUS_WAITING_FOR_INIT = -10,
        STATUS_IDLE = -1,
    };

public:
    GuideScreenplay(MainWindow *mainWindow);
    ~GuideScreenplay();

    void start();

    void guideActivated();

protected:
    void handleMessage(MessagePtr m);

private:
    MainWindow *mMainWindow;
    int mNextExpectedGuideBoxType;
};

#endif // GUIDESCREENPLAY_H
