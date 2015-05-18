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

#ifndef PREREQUISITES
#define PREREQUISITES

// additional defines for windows (msvc)
#if defined(_WIN32) || defined(_WIN64)
#include <iso646.h>
using uint = unsigned int;
#endif

#include <stdlib.h>

enum OperationMode      ///  Operation mode of the tuner
{
    MODE_IDLE = 0,      ///< Do nothing
    MODE_RECORDING,     ///< Mode for recording the piano keys
    MODE_CALCULATION,   ///< Mode where the entropy optimization is carried out
    MODE_TUNING,        ///< Mode for manually tuning the piano

    MODE_COUNT,         ///< The count of modes
};

#endif // PREREQUISITES

