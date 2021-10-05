#include <string>
#include <vector>

class Packet;
class ScorePacketInfo;

Packet* Raw_CreatePacket(int type);

bool Raw_SendPacket(Player *player,void *packet);

int Raw_SendFormPacket(Player* player, const std::string &data);
bool Raw_SendTransferPacket(Player* player, const std::string& address, short port);
bool Raw_SendSetDisplayObjectivePacket(Player* player, const std::string& title, const std::string& name, char sortOrder);
bool Raw_SendSetScorePacket(Player* player, char type, const std::vector<ScorePacketInfo>& data);
bool Raw_SendBossEventPacket(Player* player, std::string name, float percent, int type);
bool Raw_SendCrashClientPacket(Player* player);
bool Raw_SendCommandRequestPacket(Player* player, const string& cmd);
bool Raw_SendTextTalkPacket(Player* player, const string& msg);
bool Raw_BroadcastUpdateBlockPacket(IntVec4 pos);

Player* Raw_GetPlayerFromPacket(NetworkIdentifier* id, Packet* packet);
Player* Raw_GetPlayerFromPacket(ServerNetworkHandler* handler, NetworkIdentifier* id, Packet* packet);