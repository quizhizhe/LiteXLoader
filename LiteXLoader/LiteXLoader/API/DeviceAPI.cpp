#include "pch.h"

#include "APIHelp.h"
#include "DeviceAPI.h"
#include <Kernel/Device.h>
#include <Kernel/Player.h>
#include <string>
using namespace std;
using namespace script;

//////////////////// Class Definition ////////////////////

ClassDefine<DeviceClass> DeviceClassBuilder =
	defineClass<DeviceClass>("LXL_Device")
		.constructor(nullptr)
        .instanceProperty("ip", &DeviceClass::getIP)
		.instanceProperty("avgPing", &DeviceClass::getAvgPing)
		.instanceProperty("avgPacketLoss", &DeviceClass::getAvgPacketLoss)
        .instanceProperty("os", &DeviceClass::getOs)
        .instanceProperty("clientId", &DeviceClass::getClientId)
		.build();


//////////////////// Classes ////////////////////

//生成函数
Local<Object> DeviceClass::newDevice(Player* p)
{
    auto newp = new DeviceClass(p);
    return newp->getScriptObject();
}

//成员函数
void DeviceClass::setPlayer(Player* player)
{
    __try
    {
        id = ((Actor*)player)->getUniqueID();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        isValid = false;
    }
}

Player* DeviceClass::getPlayer()
{
    if (!isValid)
        return nullptr;
    else
        return Raw_GetPlayerByUniqueId(id);
}

Local<Value> DeviceClass::getIP()
{
    try {
        Player* player = getPlayer();
        if (!player)
            return Local<Value>();

        return String::newString(Raw_GetIP(player));
    }
    CATCH("Fail in GetIP!")
}

Local<Value> DeviceClass::getAvgPing()
{
    try {
        Player* player = getPlayer();
        if (!player)
            return Local<Value>();

        return Number::newNumber(Raw_GetAvgPing(player));
    }
    CATCH("Fail in getAvgPing!")
}

Local<Value> DeviceClass::getAvgPacketLoss()
{
    try {
        Player* player = getPlayer();
        if (!player)
            return Local<Value>();

        return  Number::newNumber(Raw_GetAvgPacketloss(player));
    }
    CATCH("Fail in getAvgPacketLoss!")
}

Local<Value> DeviceClass::getOs()
{
    try {
        Player* player = getPlayer();
        if (!player)
            return Local<Value>();

        return String::newString(Raw_GetOs(player));
    }
    CATCH("Fail in getOs!")
}

Local<Value> DeviceClass::getClientId()
{
    try {
        Player* player = getPlayer();
        if (!player)
            return Local<Value>();

        return String::newString(Raw_GetClientId(player));
    }
    CATCH("Fail in getClientId!")
}