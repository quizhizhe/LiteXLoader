#pragma once
#include <string>
#include <vector>

class FloatVec4;
class Actor;
class Container;
Actor* Raw_SpawnMob(std::string name, const FloatVec4& pos);

std::string Raw_GetEntityName(Actor* actor);
std::string Raw_GetEntityTypeName(Actor* actor);
int Raw_GetEntityTypeId(Actor* actor);
FloatVec4 Raw_GetEntityPos(Actor* actor);
IntVec4 Raw_GetEntityBlockPos(Actor* actor);
int Raw_GetHealth(Actor *actor);
int Raw_GetMaxHealth(Actor *actor);
bool Raw_GetIsInAir(Actor *actor);
bool Raw_GetIsInWater(Actor* actor);
std::vector<Actor*> Raw_GetAllEntities();
IntVec4 Raw_GetBlockStandingOn(Actor* ac);

bool Raw_TeleportEntity(Actor* actor, const FloatVec4 &pos);
bool Raw_KillEntity(Actor* actor);
bool Raw_SetOnFire(Actor* actor,int time);
bool Raw_HurtEntity(Actor* actor, int damage);

bool Raw_IsPlayer(Actor* actor);
Player* Raw_ToPlayer(Actor* actor);
Container* Raw_GetArmor(Actor* ac);

int Raw_GetEntityDimId(Actor* actor);
Actor* Raw_GetEntityByUniqueId(ActorUniqueID id);
float Raw_GetSpeed(Actor* ac);
bool Raw_EntityIsMoving(Actor* ac);

bool Raw_AddTag(Actor* ac, const std::string& str);
bool Raw_RemoveTag(Actor* ac, const std::string& str);
std::vector<std::string> Raw_EntityGetAllTags(Actor* ac);
bool Raw_EntityHasTag(Actor* ac, const string& str);

bool Raw_Explode(FloatVec4 pos, Actor* source, float power, float range, float isDestroy, float isFire);

void Raw_RefreshActorData(Actor* ac);
bool Raw_RefreshItems(Actor* ac);
bool Raw_EntityIsRemoved(Actor* ac);
Tick* Raw_GetEntityLastTick(Actor* ac);