#pragma once
#include <ScriptX/ScriptX.h>
using namespace script;

Local<Value> LxlGetVersion(const Arguments& args);
Local<Value> LxlListPlugins(const Arguments& args);
Local<Value> LxlRequire(const Arguments& args);