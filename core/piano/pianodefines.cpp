#include "pianodefines.h"
#include "core/system/eptexception.h"

namespace piano {

std::string toFileEnding(FileType ft) {
    switch (ft) {
    case FT_CSV: return "csv";
    case FT_EPT: return "ept";
    default: EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Invalid file type");
    }
}

FileType parseFileType(const std::string &fileEnding) {
    if (fileEnding == "csv") {return FT_CSV;}
    if (fileEnding == "ept") {return FT_EPT;}

    return FT_NONE;
}


}  // namespace piano
