#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <gsl>
#include <windows.h>
#include <LiteLoader/lbpch.h>
#include <LiteLoader/api/basicEvent.h>
#include <LiteLoader/mc/OffsetHelper.h>
#include <LiteLoader/mc/block.h>
#include <LiteLoader/mc/item.h>
#include <LiteLoader/mc/mass.h>
#include <LiteLoader/api/regCommandHelper.h>
#include <LiteLoader/api/types/types.h>
#include <LiteLoader/api/xuidreg/xuidreg.h>
#include <LiteLoader/stl/KVDB.h>
#include <LiteLoader/stl/langPack.h>
//#include <LiteLoader/mc/mass.h>
#include <LiteLoader/httplib.h>
#include <LiteLoader/api/myPacket.h>
#include <LiteLoader/stl/varint.h>
#include <Configs.h>
#include "i18n.h"
#include <string>

typedef unsigned long long QWORD;

class IntVec4
{
public:
	int x,y,z;
    int dim;
};

class FloatVec4
{
public:
	float x,y,z;
    int dim;
    inline IntVec4 toIntVec4() {
        auto px = (int)x;
        auto py = (int)y;
        auto pz = (int)z;
        if (px < 0 && px != x)
            px = px - 1;
        if (py < 0 && py != y)
            py = py - 1;
        if (pz < 0 && pz != z)
            pz = pz - 1;
        return { px, py, pz, dim };
    }
};

class FishingHook;
class ProjectileComponent;

inline std::string DimId2Name(int dimid)
{
    std::string name;
    switch (dimid)
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
    return name;
}

//全局变量
class DataLoadHelper;
extern bool isServerStarted;
extern bool isCmdRegisterEnabled;
extern CommandRegistry* CmdReg;
extern Minecraft* mc;

// 输出
extern int lxlLogLevel;
#define PREFIX "[LiteXLoader." LXL_MODULE_TYPE "]" 
#define DEBUG(t) { if(lxlLogLevel >= 5) std::cout << PREFIX "[Debug] " << (t) << std::endl; }
#define INFO(t)  { if(lxlLogLevel >= 4) std::cout << PREFIX "[Info] " << (t) << std::endl; }
#define WARN(t)  { if(lxlLogLevel >= 3) std::cout << PREFIX "[Warning] " << (t) << std::endl; }
#define ERROR(t)  { if(lxlLogLevel >= 2) std::cerr << PREFIX "[Error] " << (t) << std::endl; }
#define FATAL(t)  { if(lxlLogLevel >= 1) std::cerr << PREFIX "[FATAL] " << (t) << std::endl; }
#define PRINT(t)  { std::cout << (t) << std::endl; }
#define ERRPRINT(t)  { if(lxlLogLevel >= 2) std::cerr << (t) << std::endl; }

// Call
template<typename RTN = void, typename... Args>
RTN inline VirtualCall(void* _this, uintptr_t off, Args... args) {
    return (*(RTN(**)(void*, Args...))(*(uintptr_t*)_this + off))(_this, args...);
}