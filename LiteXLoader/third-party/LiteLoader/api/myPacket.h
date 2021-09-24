#pragma once
#include "loader/Loader.h"
#include <stl/useful.h>
#include <string>
#include <string_view>
#include <ezmc/Packet/MinecraftPacketIds.h>
using std::string_view;
#ifdef EZMC
#include <ezmc/core/BinaryStream.h>
#else
class StreamReadResult;
class ReadOnlyBinaryStream;
class BinaryStream {
public:
	std::string& getRaw() { // BinaryStream::getAndReleaseData
		return *dAccess<std::string*, 96>(this);
	}
};
enum class PacketReliability {
    Relible,
    RelibleOrdered
};

class Packet {
public:
    unsigned          unk2            = 2;                                 // 8
    PacketReliability reliableOrdered = PacketReliability::RelibleOrdered; // 12
    unsigned char     clientSubId     = 0;                                 // 16
    uint64_t          unk24           = 0;                                 // 24
    uint64_t          unk40           = 0;                                 // 32
    uint32_t          incompressible  = 0;                                 // 40
    inline Packet(unsigned compress)
        : incompressible(!compress) {
    }
    inline Packet() {
    }
    inline virtual ~Packet() {
    }
    virtual MinecraftPacketIds getId() const              = 0;
    virtual std::string getName() const                   = 0;
    virtual void        write(BinaryStream&) const        = 0;
    virtual void        dummyread()                       = 0;
    virtual bool        disallowBatching() const          = 0;
};
static_assert(sizeof(Packet) == 48);
#endif

class MyPkt : public Packet {
public:
    string_view view;
    MinecraftPacketIds id;
    bool batching;

    MyPkt(string_view sv = "", MinecraftPacketIds pid = (MinecraftPacketIds) 0, bool batching = true, bool compress = true)
        : view(sv), id(pid), batching(batching)
    {
        incompressible = compress;
    }
    inline virtual ~MyPkt() {
    }
    virtual MinecraftPacketIds getId() const {
        return id;
    }
    virtual std::string getName() const {
        return "MyPkt";
    }
    virtual void write(BinaryStream& bs) const {
        bs.getRaw().append(view);
    }
    void write(WBStream& bs) const {
        bs.write(view.data(), view.size());
    }
    virtual void dummyread() {
    }
    virtual bool disallowBatching() const {
        return !batching;
    }
};
//static_assert(offsetof(MyPkt<MinecraftPacketIds(0)>, view) == 48);