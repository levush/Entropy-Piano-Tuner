#include "serverinfo.h"

namespace serverinfo {

std::string getDownloadsFileAddress(const std::string &filename) {
    return SERVER_DOWNLOADS_ADDRESS + "/" + filename;
}

std::string getVersionFileAddress() {
    return getDownloadsFileAddress(VERSION_FILENAME);
}

std::string getManualFileAddress(const std::string &langCode) {
    return SERVER_DOCUMENTATION_ADDRESS + "/manual_" + langCode + ".pdf";
}

}  // namespace serverinfo
