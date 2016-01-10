#ifndef PIANOFILEWRITER_H
#define PIANOFILEWRITER_H

#include <QIODevice>

#include "core/piano/piano.h"

class PianoFileIOInterface
{
public:
    PianoFileIOInterface() {}
    virtual ~PianoFileIOInterface() {}

    virtual void write(QIODevice *device, const Piano &piano) const = 0;
    virtual void read(QIODevice *device, Piano &piano) = 0;
};

#endif // PIANOFILEWRITER_H
