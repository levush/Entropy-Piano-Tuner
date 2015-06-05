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

#include "log.h"
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <ctime>
#include <fstream>
#include "../adapters/filemanager.h"


const std::string Log::LOG_NAME("entropy_tuner_log.txt");
std::shared_ptr<Log> Log::mLog;

Log::Log(bool useLogfile) {
    mLog = std::shared_ptr<Log>(this);
    if (useLogfile) {
        FileManager::getSingleton().open(
                    mLogStream,
                    FileManager::getSingleton().getLogFilePath(LOG_NAME));
        writeToLogfile(LEVEL_INFORMATION, "Log file for entropy tuner\n\n", __LINE__, __FILE__, __func__);
    }
}

Log::~Log()
{
    mLogStream.close();
}

void Log::verbose(const char *text, int line, const char *file, const char *function) {
    char buffer[ERROR_BUFFER_SIZE];
    snprintf(buffer, ERROR_BUFFER_SIZE, "%s (at line %d in file %s in function %s)", text, line, file, function);
    mLog->impl_verbose(buffer);
    mLog->writeToLogfile(LEVEL_VERBOSE, text, line, file, function);
}

void Log::debug(const char *text, int line, const char *file, const char *function) {
    char buffer[ERROR_BUFFER_SIZE];
    snprintf(buffer, ERROR_BUFFER_SIZE, "%s (at line %d in file %s in function %s)", text, line, file, function);
    mLog->impl_debug(buffer);
    mLog->writeToLogfile(LEVEL_DEBUG, text, line, file, function);
}

void Log::information(const char *text, int line, const char *file, const char *function) {
    char buffer[ERROR_BUFFER_SIZE];
    snprintf(buffer, ERROR_BUFFER_SIZE, "%s (at line %d in file %s in function %s)", text, line, file, function);
    mLog->impl_information(buffer);
    mLog->writeToLogfile(LEVEL_INFORMATION, text, line, file, function);
}

void Log::warning(const char *text, int line, const char *file, const char *function) {
    char buffer[ERROR_BUFFER_SIZE];
    snprintf(buffer, ERROR_BUFFER_SIZE, "%s (at line %d in file %s in function %s)", text, line, file, function);
    mLog->impl_warning(buffer);
    mLog->writeToLogfile(LEVEL_WARNING, text, line, file, function);
}

void Log::error(const char *text, int line, const char *file, const char *function) {
    char buffer[ERROR_BUFFER_SIZE];
    snprintf(buffer, ERROR_BUFFER_SIZE, "%s (at line %d in file %s in function %s)", text, line, file, function);
    mLog->impl_error(buffer);
    mLog->writeToLogfile(LEVEL_ERROR, text, line, file, function);
}

void Log::impl_verbose(const char *l) {
    std::cout << "Verbose: " << l << std::endl;
}

void Log::impl_debug(const char *l) {
    std::cout << "Debug: " << l << std::endl;
}

void Log::impl_information(const char *l) {
    std::cout << "Information: " << l << std::endl;
}

void Log::impl_warning(const char *l) {
    std::cout << "Warning: " << l << std::endl;
}

void Log::impl_error(const char *l) {
    std::cout << "Error: " << l << std::endl;
}

void Log::writeToLogfile(ELevel level, const char *text, int line, const char *file, const char *function) {
    if (mLogStream.is_open() == false) {
        // no log output
        return;
    }

    // current time
    std::time_t t = std::time(0); //obtain the current time_t value
    tm now = *std::localtime(&t); //convert it to tm
    char tmdescr[20]={0};
    const char fmt[]="%X";
    strftime(tmdescr, sizeof(tmdescr) - 1, fmt, &now);

    // write level to log file
    switch (level) {
    case LEVEL_VERBOSE:
        mLogStream << "V/";
        break;
    case LEVEL_DEBUG:
        mLogStream << "D/";
        break;
    case LEVEL_INFORMATION:
        mLogStream << "I/";
        break;
    case LEVEL_WARNING:
        mLogStream << "W/";
        break;
    case LEVEL_ERROR:
        mLogStream << "E/";
        break;
    }

    // write time to log file:
    mLogStream << tmdescr << ":\t";

    mLogStream << "In file " << file << " in function " << function << " at line " << line << std::endl;

    // write to the log file
    mLogStream << "\t\t" << text << std::endl << std::endl;
    mLogStream.flush();
}
