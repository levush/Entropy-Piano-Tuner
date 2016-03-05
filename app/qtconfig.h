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

#ifndef QTCONFIG_H
#define QTCONFIG_H

#include "core/config.h"

#include <QtGlobal>
#include <QGuiApplication>
#include <QScreen>

// define mobile/desktop version defines
#if defined Q_OS_BLACKBERRY || defined Q_OS_ANDROID || defined Q_OS_IOS || defined Q_OS_WP
#define Q_OS_MOBILE
#else
#define Q_OS_DESKTOP
#endif

#if CONFIG_DIALOG_SIZE == 2
#   define SHOW_DIALOG(d) {(d)->showMaximized(); (d)->setFixedSize(QGuiApplication::primaryScreen()->size());}
#elif CONFIG_DIALOG_SIZE == 3
#   define SHOW_DIALOG(d) {(d)->showFullscreen(); (d)->setFixedSize(QGuiApplication::primaryScreen()->size());}
#else
#   define SHOW_DIALOG(d) {(d)->showNormal();}
#endif

// icons for menu?
#if __APPLE__
#   define CONFIG_MENU_ICONS 0
#else
#   define CONFIG_MENU_ICONS 1
#endif

// update check
#ifndef CONFIG_ENABLE_UPDATE_TOOL
#   define CONFIG_ENABLE_UPDATE_TOOL     0
#else
// set update tool to 1
#   undef CONFIG_ENABLE_UPDATE_TOOL
#   define CONFIG_ENABLE_UPDATE_TOOL     1
#endif

#endif // QTCONFIG_H

