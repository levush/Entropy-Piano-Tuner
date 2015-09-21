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

//=============================================================================
//                         Stroboscope-Drawer
//=============================================================================

#include "stroboscopedrawer.h"

//#include <iostream> // kann weg

#include "../messages/messagemodechanged.h"
#include "../messages/messagestroboscope.h"

StroboscopeDrawer::StroboscopeDrawer (GraphicsViewAdapter *graphics) :
    DrawerBase(graphics),
    mDataVector()
{
if (graphics) {};
}



//-----------------------------------------------------------------------------
//                            Message listener
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Message listener
/// \param m : Pointer to the incoming message
///////////////////////////////////////////////////////////////////////////////

void StroboscopeDrawer::handleMessage(MessagePtr m)
{
    switch (m->getType())
    {
        case Message::MSG_MODE_CHANGED:
        {
            auto mmc(std::static_pointer_cast<MessageModeChanged>(m));
            //mOperationMode = mmc->getMode();
            redraw(true);
            break;
        }
    case Message::MSG_STROBOSCOPE_EVENT:
        {
            auto mmc(std::static_pointer_cast<MessageStroboscope>(m));
            mDataVector = mmc->getData();
//            std::cout << "and here is the data:" << std::endl;
//            for (uint i=0; i<vector.size(); ++i)
//            {
//                std::cout << i << " " << std::abs(vector[i]) << "\t"
//                          << std::arg(vector[i]) << std::endl;
//            }
//            std::cout << std::endl;


        }
        default:
        {
        }
    }
}


//-----------------------------------------------------------------------------
//                                  Reset
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Reset
///////////////////////////////////////////////////////////////////////////////

void StroboscopeDrawer::reset()
{
}


//-----------------------------------------------------------------------------
//                             Drawing function
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Main drawing function
///////////////////////////////////////////////////////////////////////////////


void StroboscopeDrawer::draw()
{
    mGraphics->drawColorBar(0.1,0.1,0.8,0.8);
}




