#include "ScriptAPI.h"
#include "APIHelp.h"
#include <Kernel/System.h>
#include <Engine/EngineOwnData.h>
#include <windows.h>
#include <chrono>
#include <map>
#include <thread>
#include <memory>
#define H do_hash
using namespace std;

//////////////////// APIs ////////////////////

Local<Value> Log(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

    try {
        for (int i = 0; i < args.size(); ++i)
            PrintValue(std::cout, args[i]);
        std::cout << std::endl;
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in Log!")
}

Local<Value> ColorLog(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)

    try {
        char color = 15;
        switch (H(args[0].asString().toString().c_str()))
        {
            case H("dk_blue")  : color = 1;  break;
            case H("dk_green") : color = 2;  break;
            case H("bt_blue")  : color = 3;  break;
            case H("dk_red")   : color = 4;  break;
            case H("purple")   : color = 5;  break;
            case H("dk_yellow"): color = 6;  break;
            case H("grey")     : color = 7;  break;
            case H("sky_blue") : color = 9;  break;
            case H("blue")     : color = 9;  break;
            case H("green")    : color = 10; break;
            case H("cyan")     : color = 11; break;
            case H("red")      : color = 12; break;
            case H("pink")     : color = 13; break;
            case H("yellow")   : color = 14; break;
            case H("white")    : color = 15; break;
            default: ERROR("Invalid color!");break;
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        for (int i = 1; i < args.size(); ++i)
            PrintValue(std::cout, args[i]);
        std::cout << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in Log!")
}

Local<Value> GetTimeStr(const Arguments& args)
{
    try {
        return String::newString(Raw_GetDateTimeStr());
    }
    CATCH("Fail in GetTimeStr!")
}

Local<Value> GetTimeObj(const Arguments& args)
{
    try {
        SYSTEMTIME st;
        GetLocalTime(&st);
        Local<Object> res = Object::newObject();
        res.set("Y", Number::newNumber((int)st.wYear));
        res.set("M", Number::newNumber((int)st.wMonth));
        res.set("D", Number::newNumber((int)st.wDay));
        res.set("h", Number::newNumber((int)st.wHour));
        res.set("m", Number::newNumber((int)st.wMinute));
        res.set("s", Number::newNumber((int)st.wSecond));
        res.set("ms", Number::newNumber((int)st.wMilliseconds));
        return res;
    }
    CATCH("Fail in GetTimeNow!")
}

Local<Value> RandomGuid(const Arguments& args)
{
    return String::newString(Raw_RandomGuid());
}


/////////////// Helper ///////////////
int timeTaskId = 0;
std::unordered_map<int, bool> timeTaskMap;

#define TIMETASK_INTERVAL data0
#define TIMETASK_TASKID data1
#define TIMETASK_IS_FUNC data2
#define TIMETASK_FUNC data3

void HandleTimeTaskMessage(utils::Message& msg);
void CleanUpTimeTaskMessage(utils::Message& msg);


void NewTimeTask(int timeTaskId, int timeout, bool isInterval, bool isFunc, Local<Value> func)
{
    utils::Message timeTask(HandleTimeTaskMessage, CleanUpTimeTaskMessage);
    timeTask.TIMETASK_INTERVAL = isInterval ? timeout : 0;
    timeTask.TIMETASK_IS_FUNC = isFunc;
    timeTask.TIMETASK_FUNC = (uintptr_t) new Global<Value>(func);
    timeTask.TIMETASK_TASKID = timeTaskId;

    EngineScope::currentEngine()->messageQueue()->postMessage(timeTask, std::chrono::milliseconds(timeout));
}

void HandleTimeTaskMessage(utils::Message& msg)
{
    int nextInterval = msg.TIMETASK_INTERVAL;
    bool isInterval = (nextInterval != 0);
    int id = msg.TIMETASK_TASKID;

    bool isFunc = (bool) msg.TIMETASK_IS_FUNC;
    Global<Value>* func = (Global<Value>*)(msg.TIMETASK_FUNC);

    if (timeTaskMap[id])
    {
        try
        {
            if (isFunc)
                func->get().asFunction().call();
            else
                EngineScope::currentEngine()->eval(func->get().toStr());
        }
        catch (const Exception& e)
        {
            ERROR(string("Error occurred in ") + (isInterval ? "setInterval" : "setTimeout"));
            ERRPRINT(e);
        }

        if (isInterval)
        {
            NewTimeTask(id, nextInterval, true, isFunc, func->get());
        }
        else
        {
            timeTaskMap[id] = false;
        }
    }
}

void CleanUpTimeTaskMessage(utils::Message& msg)
{
    delete ((Global<Value>*)(msg.TIMETASK_FUNC));
}


//////////////////// APIs ////////////////////

Local<Value> SetTimeout(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 2)
    CHECK_ARG_TYPE(args[1], ValueKind::kNumber)

    try {
        bool isFunc = args[0].getKind() == ValueKind::kFunction;
        if (!isFunc && args[0].getKind() != ValueKind::kString)
        {
            ERROR("Unknown type of time task!");
            return Local<Value>();
        }

        int timeout = args[1].toInt();
        if (timeout <= 0)
            timeout = 1;

        timeTaskMap[++timeTaskId] = true;
        NewTimeTask(timeTaskId, timeout, false, isFunc, args[0]);

        return Number::newNumber(timeTaskId);
    }
    CATCH("Fail in SetTimeout!")
}

Local<Value> SetInterval(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 2)
    CHECK_ARG_TYPE(args[1], ValueKind::kNumber)

    try {
        bool isFunc = args[0].getKind() == ValueKind::kFunction;
        if (!isFunc && args[0].getKind() != ValueKind::kString)
        {
            ERROR("Unknown type of time task!");
            return Local<Value>();
        }

        Global<Value> func{ args[0] };
        int timeout = args[1].toInt();
        if (timeout <= 0)
            timeout = 1;

        timeTaskMap[++timeTaskId] = true;
        NewTimeTask(timeTaskId, timeout, true, isFunc, args[0]);

        return Number::newNumber(timeTaskId);
    }
    CATCH("Fail in SetInterval!")
}

// ClearInterval
Local<Value> ClearInterval(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)
    CHECK_ARG_TYPE(args[0], ValueKind::kNumber)

    try {
        timeTaskMap[args[0].toInt()] = false;
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in ClearInterval!")
}