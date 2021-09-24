#include "pch.h"

#include "Scoreboard.h"
#include "SymbolHelper.h"
#define H do_hash

bool checkSlotName(const std::string& slot)
{
	switch (H(slot.c_str()))
	{
	case H(Scoreboard::DISPLAY_SLOT_LIST): break;
	case H(Scoreboard::DISPLAY_SLOT_BELOWNAME): break;
	case H(Scoreboard::DISPLAY_SLOT_SIDEBAR): break;
	default: 
		return false;
	}
	return true;
}

Objective* Raw_NewObjective(const string& objname, const string& displayName)
{
	string criteria = "dummy";
	return globalScoreBoard->addObjective(objname, displayName, globalScoreBoard->getCriteria(criteria));
}

bool Raw_SetDisplayObjective(const std::string& objname, const std::string& slot, int sort)
{
	if (checkSlotName(slot))
	{
		auto obj = globalScoreBoard->getObjective(objname);
		if (!obj)
			return false;

		globalScoreBoard->setDisplayObjective(slot, obj, (ObjectiveSortOrder)sort);
		return true;
	}
	return false;
}

Objective* Raw_ClearDisplayObjective(const std::string& slot)
{
	if (checkSlotName(slot))
		return globalScoreBoard->clearDisplayObjective(slot);
	return nullptr;
}

Objective* Raw_GetDisplayObjective(const std::string& slot)
{
	if (checkSlotName(slot))
	{
		auto disp = globalScoreBoard->getDisplayObjective(slot);
		if (disp)
			return disp->getObjective();
	}
	return nullptr;
}

std::optional<int> Raw_AddScore(const std::string& objname, const std::string& id, int score)
{
	auto obj = globalScoreBoard->getObjective(objname);
	if (!obj)
		return std::nullopt;

	auto identity = globalScoreBoard->getScoreboardId(id);
	if (!Raw_ScoreboardIdIsValid(identity))
	{
		identity = SymCall("?createScoreboardId@ServerScoreboard@@UEAAAEBUScoreboardId@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			ScoreboardId*, Scoreboard*, const string&)(globalScoreBoard, id);
	}

	int a1 = 0;
	auto ref = globalScoreBoard->getScoreboardIdentityRef(identity);
	bool res = ref->modifyScoreInObjective(&a1, obj, score, PlayerScoreSetFunction::ADD);
	if (res)
		return a1;
	else
		return std::nullopt;
}

std::optional<int> Raw_SetScore(const std::string& objname, const std::string& id, int score)
{
	auto obj = globalScoreBoard->getObjective(objname);
	if (!obj)
		return std::nullopt;

	auto identity = globalScoreBoard->getScoreboardId(id);
	if (!Raw_ScoreboardIdIsValid(identity))
	{
		identity = SymCall("?createScoreboardId@ServerScoreboard@@UEAAAEBUScoreboardId@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			ScoreboardId*, Scoreboard*, const string&)(globalScoreBoard, id);
	}

	int a1 = 0;
	auto ref = globalScoreBoard->getScoreboardIdentityRef(identity);
	bool res = ref->modifyScoreInObjective(&a1, obj, score, PlayerScoreSetFunction::SET);
	if (res)
		return a1;
	else
		return std::nullopt;
}

std::optional<int> Raw_ReduceScore(const std::string& objname, const std::string& id, int score)
{
	auto obj = globalScoreBoard->getObjective(objname);
	if (!obj)
		return std::nullopt;

	auto identity = globalScoreBoard->getScoreboardId(id);
	if (!Raw_ScoreboardIdIsValid(identity))
	{
		identity = SymCall("?createScoreboardId@ServerScoreboard@@UEAAAEBUScoreboardId@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
			ScoreboardId*, Scoreboard*, const string&)(globalScoreBoard, id);
	}

	int a1 = 0;
	auto ref = globalScoreBoard->getScoreboardIdentityRef(identity);
	bool res = ref->modifyScoreInObjective(&a1, obj, score, PlayerScoreSetFunction::REMOVE);
	if (res)
		return a1;
	else
		return std::nullopt;
}

bool Raw_RemoveFromObjective(const std::string& objname, const std::string& id)
{
	auto identity = globalScoreBoard->getScoreboardId(id); 
	if (!Raw_ScoreboardIdIsValid(identity))
		return true;

	auto obj = globalScoreBoard->getObjective(objname);
	if (!obj)
		return true;

	return globalScoreBoard->getScoreboardIdentityRef(identity)->removeFromObjective(globalScoreBoard, obj);
}

bool Raw_RemoveFromObjective(const std::string& objname, Player* player)
{
	auto identity = globalScoreBoard->getScoreboardId(player);
	if (!Raw_ScoreboardIdIsValid(identity))
		return true;

	auto obj = globalScoreBoard->getObjective(objname);
	if (!obj)
		return true;

	return globalScoreBoard->getScoreboardIdentityRef(identity)->removeFromObjective(globalScoreBoard, obj);
}

int Raw_GetScore(const std::string& objname, const std::string& id)
{
	auto identity = globalScoreBoard->getScoreboardId(id);
	if (!Raw_ScoreboardIdIsValid(identity))
		return 0;

	auto obj = globalScoreBoard->getObjective(objname);
	if (!obj)
		return 0;

	auto scores = globalScoreBoard->getIdScores(identity);
	for (auto& it : scores)
		if (it.getObjective() == obj) 
			return it.getCount();
	return 0;
}

int Raw_GetScore(Player* player, const std::string& key)
{
	Objective* obj = globalScoreBoard->getObjective(key);
	if (!obj)
		return 0;

	auto id = globalScoreBoard->getScoreboardId(player);
	if (!Raw_ScoreboardIdIsValid(id))
		return 0;

	auto score = obj->getPlayerScore(*id);
	return score.getCount();
}

bool Raw_SetScore(Player* player, const std::string& key, int value)
{
	Objective* obj = globalScoreBoard->getObjective(key);
	if (!obj)
		return false;

	auto id = globalScoreBoard->getScoreboardId(player);
	if (!Raw_ScoreboardIdIsValid(id))
	{
		id = SymCall("?createScoreboardId@ServerScoreboard@@UEAAAEBUScoreboardId@@AEBVPlayer@@@Z",
			ScoreboardId*, Scoreboard*, Player*)(globalScoreBoard, player);
	}
	globalScoreBoard->modifyPlayerScore(*id, obj, value, 0);   //Set
	return true;
}

bool Raw_AddScore(Player* player, const std::string& key, int value)
{
	Objective* obj = globalScoreBoard->getObjective(key);
	if (!obj)
		return false;

	auto id = globalScoreBoard->getScoreboardId(player);
	if (!Raw_ScoreboardIdIsValid(id))
	{
		id = SymCall("?createScoreboardId@ServerScoreboard@@UEAAAEBUScoreboardId@@AEBVPlayer@@@Z",
			ScoreboardId*, Scoreboard*, Player*)(globalScoreBoard, player);
	}
	globalScoreBoard->modifyPlayerScore(*id, obj, value, 1);   //Add
	return true;
}

bool Raw_ReduceScore(Player* player, const std::string& key, int value)
{
	Objective* obj = globalScoreBoard->getObjective(key);
	if (!obj)
		return false;

	bool a1 = true;
	bool& pa = a1;
	auto id = globalScoreBoard->getScoreboardId(player);
	if (!Raw_ScoreboardIdIsValid(id))
	{
		id = SymCall("?createScoreboardId@ServerScoreboard@@UEAAAEBUScoreboardId@@AEBVPlayer@@@Z",
			ScoreboardId*, Scoreboard*, Player*)(globalScoreBoard, player);
	}
	globalScoreBoard->modifyPlayerScore(*id, obj, value, 2);   //Reduce
	return true;
}

bool Raw_DeleteScore(Player* player, const std::string& objname)
{
	return Raw_RemoveFromObjective(objname, player);
}

bool Raw_ScoreboardIdIsValid(ScoreboardId* id)
{
	return SymCall("?isValid@ScoreboardId@@QEBA_NXZ", bool, ScoreboardId*)(id);
}