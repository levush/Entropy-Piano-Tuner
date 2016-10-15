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

#include <memory>
#include <fstream>
#include <cstdio>
#include "../config.h"
#include "../prerequisites.h"


#define ERROR_BUFFER_SIZE 1024

// Log level range check
#if LOG_LEVEL < 1 || LOG_LEVEL > 5
#   error Log level has to be in [1, 5], see config.h
#endif

#if LOG_LEVEL <= 1
#   define LogV(...) {char b[ERROR_BUFFER_SIZE]; snprintf(b, ERROR_BUFFER_SIZE, __VA_ARGS__); Log::verbose(b, __LINE__, __FILE__, __func__);}
#else
#   define LogV(...)
#endif

#if LOG_LEVEL <= 2
#   define LogD(...) {char b[ERROR_BUFFER_SIZE]; snprintf(b, ERROR_BUFFER_SIZE, __VA_ARGS__); Log::debug(b, __LINE__, __FILE__, __func__);}
#else
#   define LogD(...)
#endif

#if LOG_LEVEL <= 3
#   define LogI(...) {char b[ERROR_BUFFER_SIZE]; snprintf(b, ERROR_BUFFER_SIZE, __VA_ARGS__); Log::information(b, __LINE__, __FILE__, __func__);}
#else
#   define LogI(...)
#endif

#if LOG_LEVEL <= 4
#   define LogW(...) {char b[ERROR_BUFFER_SIZE]; snprintf(b, ERROR_BUFFER_SIZE, __VA_ARGS__); Log::warning(b, __LINE__, __FILE__, __func__);}
#else
#   define LogW(...)
#endif

#if LOG_LEVEL <= 5
#   define LogE(...) {char b[ERROR_BUFFER_SIZE]; snprintf(b, ERROR_BUFFER_SIZE, __VA_ARGS__); Log::error(b, __LINE__, __FILE__, __func__);}
#else
#   define LogE(...)
#endif

class Log;

template class EPT_EXTERN std::shared_ptr<Log>;

class EPT_EXTERN Log
{
public:
    static const std::string LOG_NAME;

private:
    enum ELevel {
        LEVEL_VERBOSE,
        LEVEL_DEBUG,
        LEVEL_INFORMATION,
        LEVEL_WARNING,
        LEVEL_ERROR,
    };

public:
    static void verbose(const char *text, int line, const char *file, const char *function);
    static void debug(const char *text, int line, const char *file, const char *function);
    static void information(const char *text, int line, const char *file, const char *function);
    static void warning(const char *text, int line, const char *file, const char *function);
    static void error(const char *text, int line, const char *file, const char *function);

    Log();
    ~Log();

    void createLogFile();

    static Log &getSingleton() {return *mLog.get();}
    static Log *getSingletonPtr() {return mLog.get();}

protected:
    virtual void impl_verbose(const char *l);
    virtual void impl_debug(const char *l);
    virtual void impl_information(const char *l);
    virtual void impl_warning(const char *l);
    virtual void impl_error(const char *l);

private:
    void writeToLogfile(ELevel level, const char *text, int line, const char *file, const char *function);

private:
    static std::shared_ptr<Log> mLog;

    // use pointer due to warning C4267 on windows
    std::ofstream *mLogStream;

    static const char* simplify (const char* filename);
};

#endif // LOG_H
