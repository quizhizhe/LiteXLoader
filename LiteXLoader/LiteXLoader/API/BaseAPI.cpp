#include "BaseAPI.h"
#include "APIHelp.h"
#include <Kernel/Base.h>
#include <Kernel/SymbolHelper.h>
#include "McAPI.h"
#include <cmath>
using namespace script;

//////////////////// Class Definition ////////////////////

ClassDefine<IntPos> IntPosBuilder =
    defineClass<IntPos>("IntPos")
        .constructor(&IntPos::create)
        .instanceProperty("x", &IntPos::getX, &IntPos::setX)
        .instanceProperty("y", &IntPos::getY, &IntPos::setY)
        .instanceProperty("z", &IntPos::getZ, &IntPos::setZ)
        .instanceProperty("dim", &IntPos::getDim)
        .instanceProperty("dimid", &IntPos::getDimId, &IntPos::setDimId)
        .build();

ClassDefine<FloatPos> FloatPosBuilder =
    defineClass<FloatPos>("FloatPos")
        .constructor(&FloatPos::create)
        .instanceProperty("x", &FloatPos::getX, &FloatPos::setX)
        .instanceProperty("y", &FloatPos::getY, &FloatPos::setY)
        .instanceProperty("z", &FloatPos::getZ, &FloatPos::setZ)
        .instanceProperty("dim", &FloatPos::getDim)
        .instanceProperty("dimid", &FloatPos::getDimId, &FloatPos::setDimId)
        .build();

ClassDefine<PitchAngle> AnglePitchBuilder =
    defineClass<PitchAngle>("PitchAngle")
        .constructor(&PitchAngle::create)
        .instanceProperty("pitch", &PitchAngle::getPitch, &PitchAngle::setPitch)
        .instanceProperty("yaw", &PitchAngle::getYaw, &PitchAngle::setYaw)
        .instanceFunction("toFacing", &PitchAngle::toFacing)
        .build();
        

//////////////////// IntPos ////////////////////   

IntPos* IntPos::create(const Arguments& args)
{
    if(args.size() < 3)
        return nullptr;
    try
    {
        IntPos *p = new IntPos(args.thiz());
        p->x = args[0].asNumber().toInt32();
        p->y = args[1].asNumber().toInt32();
        p->z = args[2].asNumber().toInt32();
        p->dim = args[3].asNumber().toInt32();
        return p;
    }
    catch(...)
    {
        return nullptr;
    }
}

Local<Object> IntPos::newPos(int x, int y, int z, int dim)
{
    return EngineScope::currentEngine()->newNativeClass<IntPos>(x,y,z,dim);
}

Local<Object> IntPos::newPos(const BlockPos &b, int dim)
{
    return IntPos::newPos(b.x, b.y, b.z, dim);
}

Local<Object> IntPos::newPos(const IntVec4 &v)
{
    return IntPos::newPos(v.x, v.y, v.z, v.dim);
}

Local<Object> IntPos::newPos(const BlockPos* b, int dim)
{
    return IntPos::newPos(b->x, b->y, b->z, dim);
}

Local<Object> IntPos::newPos(const BlockPos* b, BlockSource* bs)
{
    return IntPos::newPos(b->x, b->y, b->z, Raw_GetBlockDimensionId(bs));
}

IntPos* IntPos::extractPos(Local<Value> v)
{
    if(EngineScope::currentEngine()->isInstanceOf<IntPos>(v))
        return EngineScope::currentEngine()->getNativeInstance<IntPos>(v);
    else
        return nullptr;
}

Local<Value> IntPos::getDim()
{
    return String::newString(DimId2Name(dim));
}

//////////////////// FloatPos ////////////////////

FloatPos* FloatPos::create(const Arguments& args)
{
    if(args.size() < 3)
        return nullptr;
    try
    {
        FloatPos *p = new FloatPos(args.thiz());
        p->x = args[0].asNumber().toFloat();
        p->y = args[1].asNumber().toFloat();
        p->z = args[2].asNumber().toFloat();
        p->dim = args[3].asNumber().toInt32();
        return p;
    }
    catch(...)
    {
        return nullptr;
    }
}

Local<Object> FloatPos::newPos(double x, double y, double z, int dim)
{
    return EngineScope::currentEngine()->newNativeClass<FloatPos>(x,y,z,dim);
}

Local<Object> FloatPos::newPos(const Vec3 &v, int dim)
{
    return FloatPos::newPos(v.x, v.y, v.z, dim);
}

Local<Object> FloatPos::newPos(const FloatVec4 &v)
{
    return FloatPos::newPos(v.x, v.y, v.z, v.dim);
}

FloatPos* FloatPos::extractPos(Local<Value> v)
{
    if(EngineScope::currentEngine()->isInstanceOf<FloatPos>(v))
        return EngineScope::currentEngine()->getNativeInstance<FloatPos>(v);
    else
        return nullptr;
}

Local<Value> FloatPos::getDim()
{
    string name;
    switch (dim)
    {
    case 0:
        name = _TRS("base.getDimName.0");
        break;
    case 1:
        name = _TRS("base.getDimName.1");
        break;
    case 2:
        name = _TRS("base.getDimName.2");
        break;
    default:
        name = _TRS("base.getDimName.unknown");
        break;
    }
    return String::newString(name);
}

//////////////////// PitchAngle ////////////////////

PitchAngle* PitchAngle::create(const Arguments& args)
{
    if (args.size() < 2)
        return nullptr;
    try
    {
        PitchAngle* pa = new PitchAngle(args.thiz());
        pa->pitch = args[0].asNumber().toInt32();
        pa->yaw = args[1].asNumber().toInt32();
        return pa;
    }
    catch (...)
    {
        return nullptr;
    }
}

Local<Value> PitchAngle::toFacing()
{
    int yaw_u = yaw + 180;

    if (yaw_u > 135 && yaw_u < 225)
        return Number::newNumber(0);
    if (yaw_u >= 225 && yaw_u < 315)
        return Number::newNumber(1);
    if ((yaw_u >= 315 && yaw_u <= 359) || (yaw_u >= 1 && yaw_u <= 45))
        return Number::newNumber(2);
    if (yaw_u > 45 && yaw_u <= 135)
        return Number::newNumber(3);

    return Number::newNumber(-1);
}

Local<Object> PitchAngle::newAngle(float pitch, float yaw)
{
    return EngineScope::currentEngine()->newNativeClass<PitchAngle>(pitch, yaw);
}

//////////////////// APIs ////////////////////

Local<Value> McClass::newIntPos(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 4)
    CHECK_ARG_TYPE(args[0], ValueKind::kNumber)
    CHECK_ARG_TYPE(args[1], ValueKind::kNumber)
    CHECK_ARG_TYPE(args[2], ValueKind::kNumber)
    CHECK_ARG_TYPE(args[3], ValueKind::kNumber)

    try
    {
        return IntPos::newPos(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt());
    }
    CATCH("Fail in NewIntPos!")
}

Local<Value> McClass::newFloatPos(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 4)
    CHECK_ARG_TYPE(args[0], ValueKind::kNumber)
    CHECK_ARG_TYPE(args[1], ValueKind::kNumber)
    CHECK_ARG_TYPE(args[2], ValueKind::kNumber)
    CHECK_ARG_TYPE(args[3], ValueKind::kNumber)

    try
    {
        return FloatPos::newPos(args[0].asNumber().toFloat(), args[1].asNumber().toFloat(), args[2].asNumber().toFloat(), args[3].toInt());
    }
    CATCH("Fail in NewFloatPos!")
}

Local<Value> McClass::getBDSVersion(const Arguments& args)
{
    try
    {
        return String::newString(Raw_GetBDSVersion());
    }
    CATCH("Fail in GetBDSVersion!")
}