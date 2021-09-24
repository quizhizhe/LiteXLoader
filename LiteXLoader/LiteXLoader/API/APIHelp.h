#pragma once
#include <ScriptX/ScriptX.h>
using namespace script;

#include <Kernel/Global.h>
#include <Kernel/ThirdParty.h>
#include <Kernel/i18n.h>
#include <Engine/EngineOwnData.h>
#include <string>
#include <vector>
#include <exception>

// 输出
extern int lxlLogLevel;
#define PREFIX "[LiteXLoader." LXL_MODULE_TYPE "]" 
#define DEBUG(t) { if(lxlLogLevel >= 5) std::cout << PREFIX "[Debug] " << (t) << std::endl; }
#define INFO(t)  { if(lxlLogLevel >= 4) std::cout << PREFIX "[Info] " << (t) << std::endl; }
#define WARN(t)  { if(lxlLogLevel >= 3) std::cout << PREFIX "[Warning] " << (t) << std::endl; }
#define ERROR(t)  { if(lxlLogLevel >= 2) std::cerr << PREFIX "[Error] " << (t) << std::endl; }
#define FATAL(t)  { if(lxlLogLevel >= 1) std::cerr << PREFIX "[FATAL] " << (t) << std::endl; }
#define PRINT(t)  { std::cout << t << std::endl; }
#define ERRPRINT(t)  { if(lxlLogLevel >= 2) std::cerr << t << std::endl; }

// 方便提取类型
#define toStr() asString().toString()
#define toInt() asNumber().toInt32()

// 实例类类型检查
template <typename T>
bool inline IsInstanceOf(Local<Value> v)
{
    return EngineScope::currentEngine()->isInstanceOf<T>(v);
}

// 至少COUNT个参数
#define CHECK_ARGS_COUNT(ARGS,COUNT) \
    if(ARGS.size()<COUNT) \
    { \
        ERROR("Too Few arguments!"); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return Local<Value>(); \
    }

// 检查是否TYPE类型 
#define CHECK_ARG_TYPE(ARG,TYPE) \
    if(ARG.getKind() != TYPE) \
    { \
        ERROR("Wrong type of argument!"); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return Local<Value>(); \
    }

// 截获引擎异常
#define CATCH(LOG) \
    catch(const Exception& e) \
    { \
        ERROR(LOG##"\n"); ERRPRINT(e); \
        ERRPRINT("[Error] In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return Local<Value>(); \
    } \
    catch(const std::exception &e) \
    { \
        ERROR("C++ Uncaught Exception Detected!"); \
        ERRPRINT(e.what()); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return Local<Value>(); \
    } \
    catch(const seh_exception &e) \
    { \
        ERROR("SEH Uncaught Exception Detected!"); \
        ERRPRINT(e.what()); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return Local<Value>(); \
    } \
    catch(...) \
    { \
        ERROR("Uncaught Exception Detected!"); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return Local<Value>(); \
    }

// 至少COUNT个参数_Constructor
#define CHECK_ARGS_COUNT_C(ARGS,COUNT) \
    if(ARGS.size()<COUNT) \
    { \
        ERROR("Too Few arguments!"); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return nullptr; \
    }

// 检查是否TYPE类型_Constructor
#define CHECK_ARG_TYPE_C(ARG,TYPE) \
    if(ARG.getKind() != TYPE) \
    { \
        ERROR("Wrong type of argument!"); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return nullptr; \
    }

// 截获引擎异常_Constructor
#define CATCH_C(LOG) \
    catch(const Exception& e) \
    { \
        ERROR(LOG##"\n"); ERRPRINT(e); \
        ERRPRINT("[Error] In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return nullptr; \
    } \
    catch(const std::exception &e) \
    { \
        ERROR("C++ Uncaught Exception Detected!"); \
        ERRPRINT(e.what()); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return nullptr; \
    } \
    catch(const seh_exception &e) \
    { \
        ERROR("SEH Uncaught Exception Detected!"); \
        ERRPRINT(e.what()); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return nullptr; \
    } \
    catch(...) \
    { \
        ERROR("Uncaught Exception Detected!"); \
        ERROR(std::string("In API: ") + __FUNCTION__); \
        ERROR("In Plugin: " + ENGINE_OWN_DATA()->pluginName); \
        return nullptr; \
    }


// 判断是否为浮点数
bool CheckIsFloat(const Local<Value>& num);

// 序列化
void PrintValue(std::ostream &out, Local<Value> v);
std::string ValueToString(Local<Value> v);

// Json 序列化 反序列化
Local<Value> JsonToValue(std::string jsonStr);
Local<Value> JsonToValue(JSON_VALUE j);
std::string ValueToJson(Local<Value> v,int formatIndent = -1);