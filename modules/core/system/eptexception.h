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

#ifndef EPTEXCEPTION_H
#define EPTEXCEPTION_H

#include <exception>
#include <assert.h>
#include <string>
#include "../prerequisites.h"
#include "../config.h"

// Check for assert mode

// RELEASE_EXCEPTIONS mode
#if EPT_ASSERT_MODE == 1
#   ifdef _DEBUG
#       define EptAssert( a, b ) assert( (a) && (b) )

#   else
#       define EptAssert( a, b ) if( !(a) ) EPT_EXCEPT( EptException::ERR_RT_ASSERTION_FAILED, (b) )

#   endif

// EXCEPTIONS mode
#elif EPT_ASSERT_MODE == 2
#   define EptAssert( a, b ) if( !(a) ) EPT_EXCEPT( EptException::ERR_RT_ASSERTION_FAILED, (b) )

// STANDARD mode
#else
#   define EptAssert( a, b ) assert( (a) && (b) )

#endif

class EPT_EXTERN EptException : public std::exception
{
protected:
    long mLine;
    int mNumber;
    std::string mTypeName;
    std::string mDescription;
    std::string mSource;
    std::string mFile;
    mutable std::string mFullDesc;
public:

    enum ExceptionCodes {
        ERR_CANNOT_WRITE_TO_FILE,
        ERR_CANNOT_READ_FROM_FILE,
        ERR_INVALID_STATE,
        ERR_INVALIDPARAMS,
        ERR_RENDERINGAPI_ERROR,
        ERR_DUPLICATE_ITEM,
        ERR_INTERNAL_ERROR,
        ERR_RT_ASSERTION_FAILED,
        ERR_NOT_IMPLEMENTED
    };

    /// Default constructor.
    EptException( int number, const std::string& description, const std::string& source );

    /// Extended constructor.
    EptException( int number, const std::string& description, const std::string& source, const char* file, long line );

    /// Copy constructor.
    EptException(const EptException&) = default;

    /// Needed for compatibility with std::exception
    ~EptException() throw() {}

    /// Assignment operator.
    EptException& operator = (const EptException&) = default;

    /// Returns a string with the full description of this error.
    virtual const std::string& getFullDescription(void) const;

    /// Gets the error code.
    virtual int getNumber(void) const throw();

    /// Gets the source function.
    virtual const std::string &getSource() const { return mSource; }

    /// Gets source file name.
    virtual const std::string &getFile() const { return mFile; }

    /// Gets line number.
    virtual long getLine() const { return mLine; }

    /// Returns a string with only the 'description' field of this exception.
    virtual const std::string &getDescription(void) const { return mDescription; }

    /// Override std::exception::what
    const char* what() const throw() { return getFullDescription().c_str(); }

private:
    static std::string toString(int code);

};



#ifndef EPT_EXCEPT
#define EPT_EXCEPT(num, desc) throw EptException(num, desc, __func__, __FILE__, __LINE__ );
#endif

#endif // EPTEXCEPTION_H
