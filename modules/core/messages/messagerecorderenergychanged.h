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
//                              Message handler
//=============================================================================

#ifndef MESSAGERECORDERENERGYCHANGED_H
#define MESSAGERECORDERENERGYCHANGED_H

#include "message.h"

class MessageRecorderEnergyChanged : public Message
{
public:
    enum class LevelType
    {
        LEVEL_INPUT,       ///< The input level changed (actual signal)
        LEVEL_OFF,         ///< The off level changed
    };

public:
    MessageRecorderEnergyChanged(LevelType type, double level);
    ~MessageRecorderEnergyChanged(){};

    LevelType getLevelType() const {return mLevelType;}
    double getLevel() const {return mLevel;}

private:
    const LevelType mLevelType;
    const double mLevel;
};

#endif // MESSAGERECORDERENERGYCHANGED_H
