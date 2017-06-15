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
//                              Piano-Manager
//=============================================================================

#ifndef PIANOMANAGER_H
#define PIANOMANAGER_H

#include "prerequisites.h"
#include "core/system/eptexception.h"
#include "piano.h"
#include "../messages/messagelistener.h"
#include "../adapters/modeselectoradapter.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief The PianoManager class
///
/// This class holds the instance of the Piano and manages the recording
/// process.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN PianoManager : public MessageListener
{
private:
    static std::unique_ptr<PianoManager> THE_ONE_AND_ONLY;
public:
    PianoManager();
    ~PianoManager() {}

    static std::unique_ptr<PianoManager> &getSingletonPtr() {return THE_ONE_AND_ONLY;}

    Piano &getPiano()             {return mPiano;}
    const Piano &getPiano() const {return mPiano;}

    void resetPitches ();

protected:
    void handleMessage(MessagePtr m) override final;

private:
    void handleNewKey (int keynumber, std::shared_ptr<Key> keyptr);
    int findNextKey (int keynumber);

    Piano mPiano;                   ///< Instance of the piano
    int mSelectedKey;               ///< Local copy of the selected key
    bool mForcedRecording;          ///< Flag for forced recording
    OperationMode mOperationMode;   ///< Local copy of the operation mode

};

#endif // PIANOMANAGER_H
