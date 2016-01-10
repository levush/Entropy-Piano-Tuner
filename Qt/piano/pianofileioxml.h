#ifndef PIANOFILEIOXML_H
#define PIANOFILEIOXML_H

#include "pianofileiointerface.h"

class PianoFileIOXml : public PianoFileIOInterface
{
public:
    using FileVersionType = int;
    static const QString FILE_TYPE_NAME;
    static const FileVersionType UNSET_FILE_VERSION;
    static const FileVersionType CURRENT_FILE_VERSION;
    static const FileVersionType MIN_SUPPORTED_FILE_VERSION;

    virtual void write(QIODevice *device, const Piano &piano) const override final;
    virtual void read(QIODevice *device, Piano &piano) override final;

private:
    /// file version as integer
    FileVersionType mFileVersion;
};

#endif // PIANOFILEIOXML_H
