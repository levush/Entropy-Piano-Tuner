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

#include "simplethreadhandler.h"

SimpleThreadHandler::SimpleThreadHandler()
    : mCancelThread(false),
      mRunning(false) {
}

SimpleThreadHandler::~SimpleThreadHandler() {
    stop();
}

void SimpleThreadHandler::start() {
    stop();
    setCancelThread(false);
    mThread = std::thread(&SimpleThreadHandler::simpleWorkerFunction, this);
}

void SimpleThreadHandler::stop() {
    setCancelThread(true);                  // Set cancel flag to true
    if (mThread.joinable()) mThread.join(); // Wait for thread to terminate
}

void SimpleThreadHandler::simpleWorkerFunction() {
    mRunning = true;

    try
    {
        workerFunction();
    }
    catch (const EptException &e)
    {
        LogE("Worker thread stopped with EptException: %s", e.what());
        exceptionCaught(e);
    }
    catch (const std::exception &e)
    {
        LogE("Worker thread stopped with std::exception: %s", e.what());
    }
    catch (...) {
        LogE("Worker thread stopped with an unknown exception");
    }

    mRunning = false;
}
