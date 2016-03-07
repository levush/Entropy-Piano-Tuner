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

#ifndef PREREQUISITES
#define PREREQUISITES

#include "config.h"

#if defined(QT_CORE_LIB)
#include <QString>
#endif

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <limits>
#include <utility>
#include <complex>

// additional defines for windows (msvc)
#if defined(_WIN32) || defined(_WIN64)
#include <iso646.h>
using uint = unsigned int;
#endif

// export std templates
EPT_EXTERN_TEMPLATE class EPT_EXTERN std::basic_string<wchar_t>;
#if !defined(QT_CORE_LIB)
// export std::string on our own
EPT_EXTERN_TEMPLATE class EPT_EXTERN std::basic_string<char>;
#endif
template class EPT_EXTERN std::vector<double>;
template class EPT_EXTERN std::vector<float>;
template class EPT_EXTERN std::map<std::string, double>;
template class EPT_EXTERN std::map<std::string, int>;
template class EPT_EXTERN std::map<std::string, std::string>;
template class EPT_EXTERN std::map<double, double>;
template class EPT_EXTERN std::map<int, int>;
template struct EPT_EXTERN std::atomic<bool>;
template class EPT_EXTERN std::vector<std::pair<std::string, std::string> >;
template class EPT_EXTERN std::complex<double>;
template class EPT_EXTERN std::vector<std::complex<double>>;
class EPT_EXTERN std::mutex;
class EPT_EXTERN std::thread;

// define snprintf for windows
#ifdef _MSC_VER

#include <stdarg.h>
#include <stdio.h>

#if !defined(snprintf) && defined(_MSC_VER) && _MSC_VER < 1900

inline int snprintf(char* str, size_t size, const char* format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

inline int snprintf(char* str, size_t size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = snprintf(str, size, format, ap);
    va_end(ap);

    return count;
}
#endif // snprintf

#endif // _MSC_VER


#include <stdlib.h>

enum OperationMode      ///  Operation mode of the tuner
{
    MODE_IDLE = 0,      ///< Do nothing
    MODE_RECORDING,     ///< Mode for recording the piano keys
    MODE_CALCULATION,   ///< Mode where the entropy optimization is carried out
    MODE_TUNING,        ///< Mode for manually tuning the piano

    MODE_COUNT,         ///< The count of modes
};

inline int toFlag(OperationMode mode)
{
    return 1 << mode;
}

#endif // PREREQUISITES

