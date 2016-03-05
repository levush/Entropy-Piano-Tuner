#ifndef SHAREDLIBRARY_H
#define SHAREDLIBRARY_H

#include <string>
#include <memory>

#ifdef EPT_SHARED_ALGORITHMS

#include "uv.h"

#include "log.h"

class SharedLibrary
{
public:
    SharedLibrary(const std::string &path);
    ~SharedLibrary();

    void close();

    bool sym(const char *name, void **ptr);

    bool is_open() const {return mOpen;}

private:
    bool open(const std::string &path);

private:
    uv_lib_t mLib;
    bool mOpen;
    const std::string mPath;
};

typedef std::shared_ptr<SharedLibrary> SharedLibraryPtr;

#endif

#endif // SHAREDLIBRARY_H
