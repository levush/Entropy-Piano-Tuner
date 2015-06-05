#ifndef RAWDATAWRITER_H
#define RAWDATAWRITER_H

#include <limits>
#include <mutex>

#include "audiobase.h"
#include "circularbuffer.h"

class RawDataWriter
{
public:
private:
    CircularBuffer<AudioBase::PacketDataType> mPacket;
    mutable std::mutex mPacketMutex;

public:
    RawDataWriter();

    void setup(int samplingRate, int mBufferSize);

    AudioBase::PacketType readPacket(size_t size = std::numeric_limits<size_t>::max());

protected:
    int mSamplingRate;
    void clearPacket();
    size_t getFreePacketSize() const;
    void writeData(AudioBase::PacketType d);
};

#endif // RAWDATAWRITER_H
