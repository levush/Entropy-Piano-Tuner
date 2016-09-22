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
//              Message reporting a change of the operation mode
//=============================================================================

#ifndef MESSAGEMODECHANGED_H
#define MESSAGEMODECHANGED_H

#include "message.h"
#include "prerequisites.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Message reporting a change of the operation mode
///
/// The EPT can be in four different operation modes. These can be changed
/// by clicking or by pressing the TAB key. Whenever the operation mode
/// changes, this message informs the other components of the EPT
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN MessageModeChanged : public Message
{
public:
    MessageModeChanged(OperationMode mode, OperationMode previousMode);
    ~MessageModeChanged();

    OperationMode getMode() const {return mMode;}
    OperationMode getPreviousMode() const {return mPreviousMode;}

private:
    OperationMode mMode;
    OperationMode mPreviousMode;
};

#endif // MESSAGEMODECHANGED_H
