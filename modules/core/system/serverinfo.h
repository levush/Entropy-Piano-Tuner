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

#ifndef SERVERINFO_H
#define SERVERINFO_H

#include "prerequisites.h"

namespace serverinfo
{

static const std::string SERVER_DOMAIN                 = "piano-tuner.org";
static const std::string SERVER_ADDRESS                = "http://" + SERVER_DOMAIN;
static const std::string SERVER_NAME                   = "www." + SERVER_DOMAIN;
static const std::string SERVER_DOWNLOADS_ADDRESS      = "http://download." + SERVER_DOMAIN + "/Resources/Public/Downloads";
static const std::string SERVER_DOCUMENTATION_ADDRESS  = "http://download." + SERVER_DOMAIN + "/Resources/Public/Documentation";
static const std::string MAIL_ADDRESS                  = "info@" + SERVER_DOMAIN;

static const std::string VERSION_FILENAME              = "version.xml";
static const std::string MAC_X86_64_DMG                = "EntropyPianoTuner_MacOsX_x86_64.dmg";

extern std::string EPT_EXTERN getDownloadsFileAddress(const std::string &filename);
extern std::string EPT_EXTERN getVersionFileAddress();
extern std::string EPT_EXTERN getManualFileAddress(const std::string &langCode);

}  // namespace serverinfo

#endif // SERVERINFO_H
