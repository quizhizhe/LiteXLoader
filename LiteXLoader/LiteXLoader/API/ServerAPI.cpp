#include "pch.h"

#include "APIHelp.h"
#include <ScriptX/ScriptX.h>
#include <Kernel/Server.h>
#include "ServerAPI.h"
#include "McAPI.h"
using namespace script;

Local<Value> McClass::setMotd(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    CHECK_ARG_TYPE(args[0],ValueKind::kString)

    try{
        return Boolean::newBoolean(Raw_SetServerMotd(args[0].asString().toString()));
    }
    CATCH("Fail in SetServerMotd!")
}

Local<Value> McClass::crashBDS(const Arguments& args)
{
    *(int*)(uintptr_t)0 = 2;
    return Boolean::newBoolean(true);
}