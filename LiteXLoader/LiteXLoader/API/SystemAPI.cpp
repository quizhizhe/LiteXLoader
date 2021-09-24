#include "pch.h"

#include "SystemAPI.h"
#include "APIHelp.h"
#include <Kernel/System.h>
#include <Engine/TimeTaskSystem.h>
#include <filesystem>
#include <fstream>
using namespace script;
using namespace std::filesystem;

//////////////////// Classes ////////////////////

ClassDefine<void> SystemClassBuilder =
    defineClass("system")
        .function("getTimeStr", &SystemClass::getTimeStr)
        .function("getTimeObj", &SystemClass::getTimeObj)
        .function("randomGuid", &SystemClass::randomGuid)
        .function("cmd", &SystemClass::cmd)
        .function("newProcess", &SystemClass::newProcess)
        .build();


Local<Value> SystemClass::cmd(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 2);
    CHECK_ARG_TYPE(args[0], ValueKind::kString);
    CHECK_ARG_TYPE(args[1], ValueKind::kFunction);
    if (args.size() >= 3)
        CHECK_ARG_TYPE(args[2], ValueKind::kNumber);

    try {
        Global<Function> callbackFunc{ args[1].asFunction() };

        return Boolean::newBoolean(Raw_NewProcess("cmd /c" + args[0].toStr(),
            [callback{ std::move(callbackFunc) }, engine{ EngineScope::currentEngine() }]
        (int exitCode, string output)
        {
            EngineScope scope(engine);
            try
            {
                NewTimeout(callback.get(), { Number::newNumber(exitCode), String::newString(output) }, 1);
            }
            catch (const Exception& e)
            {
                ERROR("SystemCmd Callback Failed!");
                ERRPRINT("[Error] In Plugin: " + ENGINE_OWN_DATA()->pluginName);
                ERRPRINT(e);
            }
        }
        , args.size() >= 3 ? args[2].toInt() : -1));
    }
    CATCH("Fail in SystemCmd");
}

Local<Value> SystemClass::newProcess(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 2);
    CHECK_ARG_TYPE(args[0], ValueKind::kString);
    CHECK_ARG_TYPE(args[1], ValueKind::kFunction);
    if (args.size() >= 3)
        CHECK_ARG_TYPE(args[2], ValueKind::kNumber);

    try {
        Global<Function> callbackFunc{ args[1].asFunction() };

        return Boolean::newBoolean(Raw_NewProcess(args[0].toStr(),
            [callback{ std::move(callbackFunc) }, engine{ EngineScope::currentEngine() }]
        (int exitCode, string output)
        {
            EngineScope scope(engine);
            try
            {
                NewTimeout(callback.get(), { Number::newNumber(exitCode), String::newString(output) }, 1);
            }
            catch (const Exception& e)
            {
                ERROR("SystemNewProcess Callback Failed!");
                ERRPRINT("[Error] In Plugin: " + ENGINE_OWN_DATA()->pluginName);
                ERRPRINT(e);
            }
        }
        , args.size() >= 3 ? args[2].toInt() : -1));
    }
    CATCH("Fail in SystemCmd");
}

Local<Value> SystemClass::getTimeStr(const Arguments& args)
{
    try {
        return String::newString(Raw_GetDateTimeStr());
    }
    CATCH("Fail in GetTimeStr!")
}

Local<Value> SystemClass::getTimeObj(const Arguments& args)
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

Local<Value> SystemClass::randomGuid(const Arguments& args)
{
    return String::newString(Raw_RandomGuid());
}