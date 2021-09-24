#include "pch.h"

#include "APIHelp.h"
#include "LoggerAPI.h"
#include "PlayerAPI.h"
#include <Kernel/System.h>
#include <Kernel/Player.h>
#include <Engine/EngineOwnData.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
using namespace script;

#define LOGGER_CHECK_DEBUG(level) (level >= 5)
#define LOGGER_CHECK_INFO(level) (level >= 4)
#define LOGGER_CHECK_WARN(level) (level >= 3)
#define LOGGER_CHECK_ERROR(level) (level >= 2)
#define LOGGER_CHECK_FATAL(level) (level >= 1)


//////////////////// Classes ////////////////////

ClassDefine<void> LoggerClassBuilder =
    defineClass("logger")
        .function("log", &LoggerClass::log)
        .function("debug", &LoggerClass::debug)
        .function("info", &LoggerClass::info)
        .function("warn", &LoggerClass::warn)
        .function("warning", &LoggerClass::warn)
        .function("error", &LoggerClass::error)
        .function("fatal", &LoggerClass::fatal)

        .function("setTitle", &LoggerClass::setTitle)
        .function("setConsole", &LoggerClass::setConsole)
        .function("setFile", &LoggerClass::setFile)
        .function("setPlayer", &LoggerClass::setPlayer)
        .function("setLogLevel", &LoggerClass::setLogLevel)
        .build();


////////////////// Helper //////////////////
string inline GetTimeStrHelper()
{
    return Raw_GetDateTimeStr();
}

string& StrReplace(string& str, const string& to_replaced, const string& new_str)
{
    for (string::size_type pos(0); pos != string::npos; pos += new_str.length())
    {
        pos = str.find(to_replaced, pos);
        if (pos != string::npos)
            str.replace(pos, to_replaced.length(), new_str);
        else
            break;
    }
    return str;
}
////////////////// Helper //////////////////

void inline LogDataHelper(ostream *outStream, const Arguments& args)
{
    for(int i = 0; i < args.size(); ++i)
        PrintValue(*outStream,args[i]);
    (*outStream) << endl;
}

void LogToEachTarget(shared_ptr<EngineOwnData> globalConf,
    const string &preString, const Arguments& args, int level)
{
    if(globalConf->toConsole && globalConf->consoleLogLevel >= level)
    {
        if(!preString.empty())
            cout << preString;
        LogDataHelper(&cout, args);
    }
    if(globalConf->fout && globalConf->fileLogLevel >= level)
    {
        if(!preString.empty())
            globalConf->fout << preString;
        LogDataHelper(&(globalConf->fout), args);
    }
    if(globalConf->player && globalConf->playerLogLevel >= level && Raw_IsPlayerValid(globalConf->player))
    {
        ostringstream ostr;
        LogDataHelper(&ostr, args);

        string sendStr{ preString + ostr.str() };
        StrReplace(sendStr, "[", "<");
        StrReplace(sendStr, "]", ">");
        Raw_Tell(globalConf->player, sendStr);
    }
}

Local<Value> LoggerClass::log(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)

    try{
        auto globalConf = ENGINE_OWN_DATA();
        LogToEachTarget(globalConf, "", args, -2147483647);
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in LoggerLog!")
}

Local<Value> LoggerClass::debug(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)

    try {
        auto globalConf = ENGINE_OWN_DATA();
        if (LOGGER_CHECK_DEBUG(globalConf->maxLogLevel))
        {
            string preString{ globalConf->title };
            if (!preString.empty())
                preString = "[" + preString + "]";
            preString = preString + "[" + GetTimeStrHelper() + " Debug] ";

            LogToEachTarget(globalConf, preString, args, 5);
        }
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in LoggerDebug!")
}

Local<Value> LoggerClass::info(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    
    try {
        auto globalConf = ENGINE_OWN_DATA();
        if (LOGGER_CHECK_INFO(globalConf->maxLogLevel))
        {
            string preString{ globalConf->title };
            if (!preString.empty())
                preString = "[" + preString + "]";
            preString = preString + "[" + GetTimeStrHelper() + " Info] ";

            LogToEachTarget(globalConf, preString, args, 4);
        }
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in LoggerInfo!")
}

Local<Value> LoggerClass::warn(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    
    try {
        auto globalConf = ENGINE_OWN_DATA();
        if (LOGGER_CHECK_WARN(globalConf->maxLogLevel))
        {
            string preString{ globalConf->title };
            if (!preString.empty())
                preString = "[" + preString + "]";
            preString = preString + "[" + GetTimeStrHelper() + " Warning] ";

            LogToEachTarget(globalConf, preString, args, 3);
        }
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in LoggerWarn!")
}

Local<Value> LoggerClass::error(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    
    try {
        auto globalConf = ENGINE_OWN_DATA();
        if (LOGGER_CHECK_ERROR(globalConf->maxLogLevel))
        {
            string preString{ globalConf->title };
            if (!preString.empty())
                preString = "[" + preString + "]";
            preString = preString + "[" + GetTimeStrHelper() + " Error] ";

            LogToEachTarget(globalConf, preString, args, 2);
        }
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in LoggerError!")
}

Local<Value> LoggerClass::fatal(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    
    try {
        auto globalConf = ENGINE_OWN_DATA();
        if (LOGGER_CHECK_FATAL(globalConf->maxLogLevel))
        {
            string preString{ globalConf->title };
            if (!preString.empty())
                preString = "[" + preString + "]";
            preString = preString + "[" + GetTimeStrHelper() + " FATAL] ";

            LogToEachTarget(globalConf, preString, args, 1);
        }
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in LoggerFatal!")
}

Local<Value> LoggerClass::setTitle(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    CHECK_ARG_TYPE(args[0],ValueKind::kString)

    try {
        ENGINE_OWN_DATA()->title = args[0].asString().toString();
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in LoggerSetTitle!")
}

///////////////// Helper /////////////////
void UpdateMaxLogLevel()
{
    auto data = ENGINE_OWN_DATA();
    data->maxLogLevel = data->consoleLogLevel;
    if (data->maxLogLevel < data->fileLogLevel)
        data->maxLogLevel = data->fileLogLevel;
    if (data->maxLogLevel < data->playerLogLevel)
        data->maxLogLevel = data->playerLogLevel;
}
///////////////// Helper /////////////////

Local<Value> LoggerClass::setConsole(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    CHECK_ARG_TYPE(args[0],ValueKind::kBoolean)
    if(args.size() >= 2)
        CHECK_ARG_TYPE(args[1], ValueKind::kNumber)

    try {
        ENGINE_OWN_DATA()->toConsole = args[0].asBoolean().value();
        if (args.size() >= 2)
        {
            ENGINE_OWN_DATA()->consoleLogLevel = args[1].toInt();
            UpdateMaxLogLevel();
        }
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in LoggerSetConsole!")
}

Local<Value> LoggerClass::setFile(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    CHECK_ARG_TYPE(args[0],ValueKind::kString)
    if (args.size() >= 2)
        CHECK_ARG_TYPE(args[1], ValueKind::kNumber)

    try {
        string newFile = args[0].asString().toString();
        Raw_AutoCreateDirs(newFile);

        ofstream *fout = &(ENGINE_OWN_DATA()->fout);
        if(fout->is_open())
            fout->close();
        fout->clear();
        fout->open(newFile,ios::app);

        if (args.size() >= 2)
        {
            ENGINE_OWN_DATA()->fileLogLevel = args[1].toInt();
            UpdateMaxLogLevel();
        }
        return Boolean::newBoolean(ENGINE_OWN_DATA()->fout.is_open());
    }
    CATCH("Fail in LoggerSetFile!")
}

Local<Value> LoggerClass::setPlayer(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    if (args.size() >= 2)
        CHECK_ARG_TYPE(args[1], ValueKind::kNumber)

    try {
        Player *p = PlayerClass::extract(args[0]);
        ENGINE_OWN_DATA()->player = p;

        if (args.size() >= 2)
        {
            ENGINE_OWN_DATA()->playerLogLevel = args[1].toInt();
            UpdateMaxLogLevel();
        }
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in LoggerSetPlayer!")
}

Local<Value> LoggerClass::setLogLevel(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1)
    CHECK_ARG_TYPE(args[0], ValueKind::kNumber)

    try {
        auto conf = ENGINE_OWN_DATA();
        conf->maxLogLevel = conf->consoleLogLevel = conf->fileLogLevel = conf->playerLogLevel = args[0].toInt();
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in SetLogLevel!")
}
