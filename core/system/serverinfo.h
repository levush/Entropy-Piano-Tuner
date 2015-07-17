#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <string>

namespace serverinfo
{

static const std::string SERVER_DOMAIN                 = "piano-tuner.org";
static const std::string SERVER_ADDRESS                = "http://" + SERVER_DOMAIN;
static const std::string SERVER_NAME                   = "www." + SERVER_DOMAIN;
static const std::string SERVER_DOWNLOADS_ADDRESS      = "http://donwload." + SERVER_DOMAIN + "/Resources/Public/Downloads";
static const std::string SERVER_DOCUMENTATION_ADDRESS  = "http://download." + SERVER_DOMAIN + "/Resources/Public/Documentation";
static const std::string MAIL_ADDRESS                  = "info@" + SERVER_DOMAIN;

static const std::string VERSION_FILENAME              = "version.xml";
static const std::string MAC_X86_64_DMG                = "EntropyPianoTuner_MacOsX_x86_64.dmg";

std::string getDownloadsFileAddress(const std::string &filename);
std::string getVersionFileAddress();
std::string getManualFileAddress(const std::string &langCode);

}  // namespace serverinfo

#endif // SERVERINFO_H
