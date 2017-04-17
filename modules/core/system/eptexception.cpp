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

#include "eptexception.h"
#include <sstream>
#include "../system/log.h"

EptException::EptException(int num, const std::string& desc, const std::string& src) :
    mLine( 0 ),
    mNumber( num ),
    mDescription( desc ),
    mSource( src )
{
    LogE("%s %d %s unknown", desc.c_str(), 0, src.c_str());
}

EptException::EptException(int num, const std::string& desc, const std::string& src, const char* fil, long lin) :
    mLine( lin ),
    mNumber( num ),
    mTypeName(toString(num)),
    mDescription( desc ),
    mSource( src ),
    mFile( fil )
{
    LogE("%s %ld %s %s", getFullDescription().c_str(), mLine, mSource.c_str(), mFile.c_str());
}

const std::string& EptException::getFullDescription(void) const
{
    if (mFullDesc.empty())
    {

        std::stringstream desc;

        desc <<  "EPT EXCEPTION(" << mNumber << ":" << mTypeName << "): "
            << mDescription
            << " in " << mSource;

        if( mLine > 0 )
        {
            desc << " at " << mFile << " (mLine " << mLine << ")";
        }

        mFullDesc = desc.str();
    }

    return mFullDesc;
}

int EptException::getNumber(void) const throw()
{
    return mNumber;
}

std::string EptException::toString(int code) {
    switch (code) {
    case ERR_CANNOT_WRITE_TO_FILE: return "ErrCannotWriteToFile";
    case ERR_CANNOT_READ_FROM_FILE: return "ErrCannotReadFromFile";
    case ERR_INVALID_STATE: return "ErrInvalidState";
    case ERR_INVALIDPARAMS: return "ErrInvalidParams";
    case ERR_RENDERINGAPI_ERROR: return "ErrRenderingApiError";
    case ERR_INTERNAL_ERROR: return "ErrInternalError";
    case ERR_RT_ASSERTION_FAILED: return "ErrRtAssertionFailed";
    case ERR_NOT_IMPLEMENTED:
    default:
        return "ErrNotInplemented";
    }
}
