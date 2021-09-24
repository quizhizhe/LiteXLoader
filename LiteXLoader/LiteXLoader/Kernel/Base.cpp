#include "pch.h"

#include "Global.h"
#include "Base.h"
#include <iostream>
using namespace std;

bool Raw_Runcmd(const string &cmd)
{
    return liteloader::runcmd(cmd);
}

std::pair<bool, string> Raw_RuncmdEx(const string &cmd)
{
    return liteloader::runcmdEx(cmd);
}

bool Raw_RegisterCmd(const string &cmd, const string &describe, int cmdLevel)
{
    SymCall("?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
        void, void*, const string&, const char*, char, char, char)
        (CmdReg, cmd, describe.c_str(), cmdLevel, 0, 0x40);
    return true;
}

bool Raw_SendCmdOutput(const std::string& output)
{
    string finalOutput(output);
    finalOutput += "\r\n";

    SymCall("??$_Insert_string@DU?$char_traits@D@std@@_K@std@@YAAEAV?$basic_ostream@DU?$char_traits@D@std@@@0@AEAV10@QEBD_K@Z",
        ostream& , ostream&, const char*, unsigned)
        (cout, finalOutput.c_str(), finalOutput.size());
    return true;
}

string Raw_GetBDSVersion()
{
    string ver;
    SymCall("?getGameVersionString@Common@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
        string&, string&)(ver);
    return ver;
}

Dimension* Raw_GetDimByLevel(Level* lv, int id) {
    return SymCall("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
        Dimension*, void*, int)(lv, id);
}