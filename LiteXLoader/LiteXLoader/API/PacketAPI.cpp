#include "pch.h"

#include "PacketAPI.h"
#include <LiteXLoader/API/APIHelp.h>

ClassDefine<PacketClass> PacketClassBuilder =
    defineClass<PacketClass>("Packet")
        .constructor(nullptr)
        .function("makePacket",&PacketClass::makePacket)
        .instanceFunction("getPacketId",&PacketClass::getPacketId)
        .instanceFunction("writeInt",&PacketClass::writeInt)
        .instanceFunction("writeUInt", &PacketClass::writeUInt)
        .instanceFunction("writeULong",& PacketClass::writeULong)
        .instanceFunction("writeUShort",&PacketClass::writeUShort)
        .instanceFunction("writeFloat", &PacketClass::writeFloat)
        .instanceFunction("writeDouble", &PacketClass::writeDouble)
        .instanceFunction("writeString", &PacketClass::writeString)
        .instanceFunction("writeBool", &PacketClass::writeBool)
        .build();


PacketClass::PacketClass(int id) :ScriptClass(ScriptClass::ConstructFromCpp<PacketClass>{})
{
    this->Packetid = id;
}

WBStream PacketClass::extractPacket(Local<Value> v)
{
    if (EngineScope::currentEngine()->isInstanceOf<PacketClass>(v))
        return EngineScope::currentEngine()->getNativeInstance<PacketClass>(v)->wb;
}

Local<Object> PacketClass::newPacket(int id)
{
    PacketClass* pkt = new PacketClass(id);
    return pkt->getScriptObject();
}

int PacketClass::_getPacketid(Local<Value> v)
{
    if (EngineScope::currentEngine()->isInstanceOf<PacketClass>(v))
        return EngineScope::currentEngine()->getNativeInstance<PacketClass>(v)->Packetid;
}

Local<Value> PacketClass::makePacket(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

    try {
        auto id = args[0].asNumber().toInt32();
        return PacketClass::newPacket(id);
    }
    CATCH("Fail in PacketWrite!")
}

Local<Value> PacketClass::getPacketId(const Arguments& args)
{
    try {
        return Number::newNumber(this->Packetid);
    }
    CATCH("Fail in PacketWrite!")
}

Local<Value> PacketClass::writeUShort(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

        try {
        auto v = args[0].asNumber().toInt32();
        wb.apply(VarUShort(v));
        return this->getScriptObject();
    }
    CATCH("Fail in PacketWrite!")
}

Local<Value> PacketClass::writeInt(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

    try {
        auto v = args[0].asNumber().toInt32();
        wb.apply(VarInts(v));
        return this->getScriptObject();
    }
    CATCH("Fail in PacketWrite!")
}

Local<Value> PacketClass::writeUInt(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

     try {
        auto v = args[0].asNumber().toInt32();
        wb.apply(VarUInt(v));
        return this->getScriptObject();
     }
    CATCH("Fail in PacketWrite!")
}

Local<Value> PacketClass::writeULong(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

        try {
        auto v = args[0].asNumber().toInt64();
        wb.apply(VarULong(v));
        return this->getScriptObject();
    }
    CATCH("Fail in PacketWrite!")
}

Local<Value> PacketClass::writeString(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

    try {
        auto v = args[0].asString().toString();
        wb.apply(MCString(v));
        return this->getScriptObject();
    }
    CATCH("Fail in PacketWrite!")
}

Local<Value> PacketClass::writeFloat(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

        try {
        auto v = args[0].asNumber().toFloat();
        wb.apply(v);
        return this->getScriptObject();
    }
    CATCH("Fail in PacketWrite!")
}

Local<Value> PacketClass::writeDouble(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

        try {
        auto v = args[0].asNumber().toDouble();
        wb.apply(v);
        return this->getScriptObject();
    }
    CATCH("Fail in PacketWrite!")
}

Local<Value> PacketClass::writeBool(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

        try {
        auto v = args[0].asBoolean().value();
        wb.apply((char)v);
        return this->getScriptObject();
    }
    CATCH("Fail in PacketWrite!")
}