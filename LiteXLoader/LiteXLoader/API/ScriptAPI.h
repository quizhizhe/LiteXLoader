#pragma once
#include <ScriptX/ScriptX.h>
using namespace script;

//////////////////// APIs ////////////////////

Local<Value> Log(const Arguments& args);
Local<Value> ColorLog(const Arguments& args);
Local<Value> FastLog(const Arguments& args);

Local<Value> GetTimeStr(const Arguments& args);
Local<Value> GetTimeObj(const Arguments& args);
Local<Value> RandomGuid(const Arguments& args);

Local<Value> SetTimeout(const Arguments& args);
Local<Value> SetInterval(const Arguments& args);
Local<Value> ClearInterval(const Arguments& args);