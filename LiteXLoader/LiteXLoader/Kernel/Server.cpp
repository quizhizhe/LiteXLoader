#include "pch.h"

#include "Global.h"
#include "Server.h"
#include "Global.h"
using namespace std;

bool Raw_SetServerMotd(const std::string &motd)
{
	if (!mc)
		return false;
	ServerNetworkHandler* snh = mc->getServerNetworkHandler();
	if (!snh)
		return false;

	SymCall("?allowIncomingConnections@ServerNetworkHandler@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
		long long, ServerNetworkHandler*, const string&, bool)(snh, motd, true);
    return true;
}