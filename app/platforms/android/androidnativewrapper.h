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

#ifndef ANDROIDNATIVEWRAPPER_H
#define ANDROIDNATIVEWRAPPER_H

#ifndef __ANDROID__
#error This file may only be included on Android
#endif

#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>

void callAndroidVoidTunerApplicationFunction(const char *functionName);
long int callAndroidLongIntTunerApplicationFunction(const char *functionName);

#endif // ANDROIDNATIVEWRAPPER_H
