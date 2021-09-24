#include "pch.h"

#include <ScriptX/ScriptX.h>
#include <API/APIHelp.h>
#include <API/EventAPI.h>
#include <Engine/GlobalShareData.h>
#include <Engine/EngineOwnData.h>
#include <Engine/LocalShareData.h>
#include <Engine/RemoteCall.h>
#include <Engine/MessageSystem.h>
#include <Kernel/Data.h>
#include <Kernel/System.h>
#include <Kernel/i18n.h>
#include <windows.h>
#include <string>
#include <exception>
#include <thread>
#include <chrono>
#include <memory>
#include <filesystem>
#include <Configs.h>
#include <Version.h>
using namespace script;
using namespace std;

//主引擎表
std::vector<ScriptEngine*> lxlModules;
// 配置文件
INI_ROOT iniConf;
// 日志等级
int lxlLogLevel = 1;

extern void LoadDepends();
extern void LoadMain();
extern void BindAPIs(ScriptEngine *engine);
extern void LoadDebugEngine();

void Welcome()
{
    cout << R"(     _       _  _         __   __  _                        _             )" << endl
         << R"(    | |     (_)| |        \ \ / / | |                      | |            )" << endl
         << R"(    | |      _ | |_  ___   \ V /  | |      ___    __ _   __| |  ___  _ __ )" << endl
         << R"(    | |     | || __|/ _ \   > <   | |     / _ \  / _` | / _` | / _ \| '__|)" << endl
         << R"(    | |____ | || |_|  __/  / . \  | |____| (_) || (_| || (_| ||  __/| |   )" << endl
         << R"(    |______||_| \__|\___| /_/ \_\ |______|\___/  \__,_| \__,_| \___||_|   )" << endl;

    cout << "\n\n      =========   LiteXLoader Script Plugin Loader   =========\n" << endl;
}

void LoaderInfo()
{
    INFO(std::string("LXL for ") + LXL_MODULE_TYPE + " loaded");
    INFO(std::string("Version ") + to_string(LXL_VERSION_MAJOR) + "." + to_string(LXL_VERSION_MINOR) + "."
        + to_string(LXL_VERSION_REVISION) + (LXL_VERSION_IS_BETA ? " Beta" : ""));
}

void entry()
{
    //设置全局SEH处理
    _set_se_translator(seh_exception::TranslateSEHtoCE);

    //读取配置文件
    Raw_DirCreate(std::filesystem::path(LXL_CONFIG_PATH).remove_filename().u8string());
    iniConf = Raw_IniOpen(LXL_CONFIG_PATH);
    if (!iniConf)
        ERROR("Fail to Load config file of LiteXLoader! Default settings applied.");
    lxlLogLevel = Raw_IniGetInt(iniConf,"Main","LxlLogLevel",1);

    //国际化
    InitI18n(LXL_LANGPACK_DIR + Raw_IniGetString(iniConf, "Main", "Language", "en_US") + ".json");

    //初始化全局数据
    InitLocalShareData();
    InitGlobalShareData();

    //初始化消息系统
    InitMessageSystem();

    //欢迎
    if (localShareData->isFirstInstance)
        Welcome();
    LoaderInfo();

    //初始化经济系统
    Raw_InitEcnonmicSystem(MoneyEventCallback);

    //预加载库
    LoadDepends();
    
    //加载插件
    LoadMain();

    //注册后台调试
    LoadDebugEngine();

    //初始化事件监听
    InitEventListeners();

    //UnlockCmd
    extern bool isUnlockCmdEnabled;
    isUnlockCmdEnabled = Raw_IniGetBool(iniConf, "Modules", "BuiltInUnlockCmd", true);

    Raw_IniClose(iniConf);
}