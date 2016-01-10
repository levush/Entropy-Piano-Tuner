#ifndef PIANOFILEIOCSV_H
#define PIANOFILEIOCSV_H

#include "pianofileiointerface.h"

class PianoFileIOCsv : public PianoFileIOInterface
{
public:
    virtual void write(QIODevice *device, const Piano &piano) const override final;
    virtual void read(QIODevice *device, Piano &piano) override final;
};

#endif // PIANOFILEIOCSV_H
