#include "pch.h"

#include "Global.h"
#include "Data.h"
#include "System.h"
#include "ThirdParty.h"
#include <filesystem>
#include "i18n.h"
using namespace std;

DB_ROOT Raw_NewDB(const string &dir)
{
    if (!filesystem::exists(dir))
        if (!filesystem::create_directories(dir))
            return nullptr;
    if(!filesystem::is_directory(dir))
        return nullptr;
    try
    {
        return MakeKVDB(dir).release();
    }
    catch(...)
    {
        return nullptr;
    }
}

bool Raw_DBGet(DB_ROOT &db, const string & key, string &val)
{
    return db->get(key,val);
}

bool Raw_DBSet(DB_ROOT &db, const string & key, const string & val)
{
    db->put(key,val);
    return true;
}

bool Raw_DBDel(DB_ROOT &db, const string & key)
{
    db->del(key);
    return true;
}

vector<string> Raw_DBListKey(DB_ROOT &db)
{
    vector<string> keyList;
    db->iter([&](const string_view& key)
    {
        keyList.push_back(string(key));
        return true;
    });
    return keyList;
}

bool Raw_DBClose(DB_ROOT& db)
{
    delete db;
    return true;
}

JSON_ROOT Raw_JsonOpen(const std::string& path, const std::string& defContent)
{
    JSON_ROOT jsonConf;
    if (!Raw_PathExists(path))
    {
        //创建新的
        Raw_AutoCreateDirs(path);

        if (defContent != "")
        {
            try
            {
                jsonConf = JSON_VALUE::parse(defContent);
            }
            catch (exception& e)
            {
                ERROR("Fail to parse default json content!");
                ERRPRINT(e.what());
                jsonConf = JSON_VALUE::object();
            }
        }
        else
        {
            jsonConf = JSON_VALUE::object();
        }

        ofstream jsonFile(path);
        if (jsonFile.is_open() && defContent != "")
            jsonFile << jsonConf.dump(4);
        jsonFile.close();
    }
    else
    {
        //已存在
        string jsonTexts;
        if (!Raw_FileReadFrom(path, jsonTexts))
            jsonTexts = "";
        try
        {
            jsonConf = JSON_VALUE::parse(jsonTexts);
        }
        catch (exception& e)
        {
            ERROR("Fail to parse json content in file!");
            ERRPRINT(e.what());
            jsonConf = JSON_VALUE::array();
        }
    }
    return jsonConf;
}

INI_ROOT Raw_IniOpen(const string &path, const std::string& defContent)
{
    if (!Raw_PathExists(path))
    {
        //创建新的
        filesystem::create_directories(filesystem::path(path).remove_filename().u8string());

        ofstream iniFile(path);
        if (iniFile.is_open() && defContent != "")
            iniFile << defContent;
        iniFile.close();
    }

    //已存在
    auto root = new SimpleIni;
    root->SetUnicode(true);
    auto res = root->LoadFile(path.c_str());
    if (res<0)
    {
        ERROR("Failed in loading ini file");
        ERRPRINT(res);
        delete root;
        return nullptr;
    }
    else
    {
        root->filePath = path;
        return root;
    }
}

bool Raw_IniClose(INI_ROOT ini)
{
    if(ini)
        delete ini;
    return true;
}

bool Raw_IniSetInt(INI_ROOT ini, const string & sec, const string & key, int value)
{
    if (!ini)
        return false;
    bool isOk = ini->SetLongValue(sec.c_str(), key.c_str(), value) >= 0;
    ini->SaveFile(ini->filePath.c_str());
    return isOk;
}

bool Raw_IniSetFloat(INI_ROOT ini, const string & sec, const string & key, float value)
{
    if (!ini)
        return false;
    bool isOk = ini->SetDoubleValue(sec.c_str(),key.c_str(),value) >= 0;
    ini->SaveFile(ini->filePath.c_str());
    return isOk;
}

bool Raw_IniSetString(INI_ROOT ini, const string & sec, const string & key, const string & value)
{
    if (!ini)
        return false;
    bool isOk = ini->SetValue(sec.c_str(),key.c_str(),value.c_str()) >= 0;
    ini->SaveFile(ini->filePath.c_str());
    return isOk;
}

bool Raw_IniSetBool(INI_ROOT ini, const string & sec, const string & key, bool value)
{
    if (!ini)
        return false;
    bool isOk = ini->SetBoolValue(sec.c_str(),key.c_str(),value) >= 0;
    ini->SaveFile(ini->filePath.c_str());
    return isOk;
}

int Raw_IniGetInt(INI_ROOT ini, const string & sec, const string & key, int def)
{
    if (!ini)
        return def;
    return ini->GetLongValue(sec.c_str(),key.c_str(),def);
}

float Raw_IniGetFloat(INI_ROOT ini, const string & sec, const string & key, float def)
{
    if (!ini)
        return def;
    return (float)ini->GetDoubleValue(sec.c_str(),key.c_str(),def);
}

string Raw_IniGetString(INI_ROOT ini, const string & sec, const string & key, const string & def)
{
    if (!ini)
        return def;
    return ini->GetValue(sec.c_str(),key.c_str(),def.c_str());
}

bool Raw_IniGetBool(INI_ROOT ini, const string & sec, const string & key, bool def)
{
    if (!ini)
        return def;
    return ini->GetBoolValue(sec.c_str(),key.c_str(),def);
}

bool Raw_IniDeleteKey(INI_ROOT ini, const std::string & sec, const std::string & key)
{
    if (!ini)
        return false;
    return ini->Delete(sec.c_str(),key.c_str(),true);
}


////////////// Helper //////////////
typedef money_t (*LLMoneyGet_T)(xuid_t);
typedef string (*LLMoneyGetHist_T)(xuid_t, int);
typedef bool (*LLMoneyTrans_T)(xuid_t, xuid_t, money_t, string const&);
typedef bool (*LLMoneySet_T)(xuid_t, money_t);
typedef bool (*LLMoneyAdd_T)(xuid_t, money_t);
typedef bool (*LLMoneyReduce_T)(xuid_t, money_t);
typedef void (*LLMoneyClearHist_T)(int);

typedef void (*LLMoneyListenBeforeEvent_T)(LLMoneyCallback callback);
typedef void (*LLMoneyListenAfterEvent_T)(LLMoneyCallback callback);

struct dynamicSymbolsMap_type
{
    LLMoneyGet_T LLMoneyGet = nullptr;
    LLMoneySet_T LLMoneySet = nullptr;
    LLMoneyAdd_T LLMoneyAdd = nullptr;
    LLMoneyReduce_T LLMoneyReduce = nullptr;
    LLMoneyTrans_T LLMoneyTrans = nullptr;
    LLMoneyGetHist_T LLMoneyGetHist = nullptr;
    LLMoneyClearHist_T LLMoneyClearHist = nullptr;

    LLMoneyListenBeforeEvent_T LLMoneyListenBeforeEvent = nullptr;
    LLMoneyListenAfterEvent_T LLMoneyListenAfterEvent = nullptr;
} dynamicSymbolsMap;
////////////// Helper //////////////

bool Raw_InitEcnonmicSystem(LLMoneyCallback eventCallback)
{
    auto libs = liteloader::getAllLibs();
    bool libExists = false;
    for (auto& [name, h] : libs)
        if (name.find(L"LLMoney.dll") != wstring::npos)
        {
            libExists = true;
                
            dynamicSymbolsMap.LLMoneyGet = (LLMoneyGet_T)GetProcAddress(h, "LLMoneyGet");
            if(!dynamicSymbolsMap.LLMoneyGet)
                WARN("Fail to load API money.getMoney!");

            dynamicSymbolsMap.LLMoneySet = (LLMoneySet_T)GetProcAddress(h, "LLMoneySet");
            if (!dynamicSymbolsMap.LLMoneySet)
                WARN("Fail to load API money.setMoney!");

            dynamicSymbolsMap.LLMoneyAdd = (LLMoneyAdd_T)GetProcAddress(h, "LLMoneyAdd");
            if (!dynamicSymbolsMap.LLMoneyAdd)
                WARN("Fail to load API money.addMoney!");

            dynamicSymbolsMap.LLMoneyReduce = (LLMoneyReduce_T)GetProcAddress(h, "LLMoneyReduce");
            if (!dynamicSymbolsMap.LLMoneyReduce)
                WARN("Fail to load API money.reduceMoney!");

            dynamicSymbolsMap.LLMoneyTrans = (LLMoneyTrans_T)GetProcAddress(h, "LLMoneyTrans");
            if (!dynamicSymbolsMap.LLMoneyTrans)
                WARN("Fail to load API money.transMoney!");

            dynamicSymbolsMap.LLMoneyGetHist = (LLMoneyGetHist_T)GetProcAddress(h, "LLMoneyGetHist");
            if (!dynamicSymbolsMap.LLMoneyGetHist)
                WARN("Fail to load API money.getTransHist!");

            dynamicSymbolsMap.LLMoneyClearHist = (LLMoneyClearHist_T)GetProcAddress(h, "LLMoneyClearHist");
            if (!dynamicSymbolsMap.LLMoneyClearHist)
                WARN("Fail to load API money.clearHist!");

            dynamicSymbolsMap.LLMoneyListenBeforeEvent = (LLMoneyListenBeforeEvent_T)GetProcAddress(h, "LLMoneyListenBeforeEvent");
            if (!dynamicSymbolsMap.LLMoneyListenBeforeEvent)
            {
                WARN("Fail to load API to listen money event!");
            }
            else
            {
                dynamicSymbolsMap.LLMoneyListenBeforeEvent(eventCallback);
            }

            //dynamicSymbolsMap.LLMoneyListenAfterEvent = (LLMoneyListenAfterEvent_T)GetProcAddress(h, "LLMoneyListenAfterEvent");
            //if (!dynamicSymbolsMap.LLMoneyListenAfterEvent)
            //    WARN("Fail to load API to listen money event!");
        }
    if (!libExists)
    {
        WARN(_TRS("init.llMoney.noFound"));
        return false;
    }
    return true;
}

money_t Raw_GetMoney(xuid_t player)
{
    if (dynamicSymbolsMap.LLMoneyGet)
        return dynamicSymbolsMap.LLMoneyGet(player);
    else
    {
        ERROR("API money.getMoney have not been loaded!");
        return 0;
    }
}

bool Raw_SetMoney(xuid_t player, money_t money)
{
    if (dynamicSymbolsMap.LLMoneySet)
        return dynamicSymbolsMap.LLMoneySet(player,money);
    else
    {
        ERROR("API money.setMoney have not been loaded!");
        return false;
    }
}

bool Raw_AddMoney(xuid_t player, money_t money)
{
    if (dynamicSymbolsMap.LLMoneyAdd)
        return dynamicSymbolsMap.LLMoneyAdd(player,money);
    else
    {
        ERROR("API money.addMoney have not been loaded!");
        return false;
    }
}

bool Raw_ReduceMoney(xuid_t player, money_t money)
{
    if (dynamicSymbolsMap.LLMoneyReduce)
        return dynamicSymbolsMap.LLMoneyReduce(player, money);
    else
    {
        ERROR("API money.reduceMoney have not been loaded!");
        return false;
    }
}

bool Raw_TransMoney(xuid_t player1, xuid_t player2, money_t money, string const& notes)
{
    if (dynamicSymbolsMap.LLMoneyTrans)
        return dynamicSymbolsMap.LLMoneyTrans(player1, player2, money, notes);
    else
    {
        ERROR("API money.transMoney have not been loaded!");
        return false;
    }
}

std::string Raw_GetMoneyHist(xuid_t player, int time)
{
    if (dynamicSymbolsMap.LLMoneyGetHist)
        return dynamicSymbolsMap.LLMoneyGetHist(player, time);
    else
    {
        ERROR("API money.getTransHist have not been loaded!");
        return "";
    }
}

bool Raw_ClearMoneyHist(int time)
{
    if (dynamicSymbolsMap.LLMoneyClearHist)
    {
        dynamicSymbolsMap.LLMoneyClearHist(time);
        return true;
    }
    else
    {
        ERROR("API money.clearHist have not been loaded!");
        return false;
    }
}

string Raw_Xuid2Name(string xuid)
{
    auto name = XIDREG::id2str(stoull(xuid));
    return name.set ? name.val() : "";
}

string Raw_Name2Xuid(string name)
{
    auto xuid = XIDREG::str2id(name);
    return xuid.set ? to_string(xuid.val()) : "";
}

string Raw_toMD5(const string& str)
{
    unsigned char md5[MD5_DIGEST_LENGTH];
    const char map[] = "0123456789abcdef";
    string hexmd5;

    MD5((const unsigned char*)str.c_str(), str.length(), md5);
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        hexmd5 += map[md5[i] / 16];
        hexmd5 += map[md5[i] % 16];
    }

    return hexmd5;
}

string Raw_toSHA1(const string& str)
{
    unsigned char sha1[SHA_DIGEST_LENGTH];
    const char map[] = "0123456789abcdef";
    string hexsha1;

    SHA1((const unsigned char*)str.c_str(), str.length(), sha1);
    for (size_t i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        hexsha1 += map[sha1[i] / 16];
        hexsha1 += map[sha1[i] % 16];
    }

    return hexsha1;
}

string Raw_ToBase64(const string& data)
{
    return base64_encode(data);
}

string Raw_FromBase64(const string& data)
{
    return base64_decode(data);
}