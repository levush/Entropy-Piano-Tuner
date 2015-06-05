#ifndef RAWDATAWRITER_H
#define RAWDATAWRITER_H

#include <limits>
#include <mutex>

#include "audiobase.h"
#include "circularbuffer.h"

class RawDataWriter
{
public:
    static const size_t MIN_PACKET_SIZE = 1000;
    static const size_t MIN_FREE_PACKET_SIZE = 100;

private:
    CircularBuffer<AudioBase::PacketDataType> mPacket;
    mutable std::mutex mPacketMutex;

public:
    RawDataWriter(size_t packetSize = MIN_PACKET_SIZE);

    AudioBase::PacketType readPacket(size_t size = std::numeric_limits<size_t>::max());

protected:
    void clearPacket();
    size_t getFreePacketSize() const;
    void writeData(AudioBase::PacketType d);
};

#endif // RAWDATAWRITER_H
