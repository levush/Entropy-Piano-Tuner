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

#ifndef LOG_H
#define LOG_H

#include "tp3log/tp3log.h"
#include "../config.h"


#define ERROR_BUFFER_SIZE 1024

// Log level range check
#if LOG_LEVEL < 1 || LOG_LEVEL > 5
#   error Log level has to be in [1, 5], see config.h
#endif

#if LOG_LEVEL <= 1
#   define LogV(...) LOGVA(DEBUG, __VA_ARGS__)
#else
#   define LogV(...)
#endif

#if LOG_LEVEL <= 2
#   define LogD(...) LOGVA(DEBUG, __VA_ARGS__)
#else
#   define LogD(...)
#endif

#if LOG_LEVEL <= 3
#   define LogI(...) LOGVA(INFO, __VA_ARGS__)
#else
#   define LogI(...)
#endif

#if LOG_LEVEL <= 4
#   define LogW(...) LOGVA(WARNING, __VA_ARGS__)
#else
#   define LogW(...)
#endif

#if LOG_LEVEL <= 5
#   define LogE(...) LOGVA(FATAL, __VA_ARGS__)
#else
#   define LogE(...)
#endif

#endif // LOG_H
