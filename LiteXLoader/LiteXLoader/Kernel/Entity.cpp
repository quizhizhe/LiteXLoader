#include "Global.h"
#include "Base.h"
#include "Entity.h"
#include "Player.h"
#include "SymbolHelper.h"
#include "NBT.h"
#include "Utils.h"
#include <string>
#include <vector>
#include <bitset>
using namespace std;

class Spawner;
Actor* Raw_SpawnMob(std::string name, const FloatVec4& pos)
{
    try
    {
        if (name.find("minecraft:") == 0)
            name = name.substr(10);

        char a[168];
        ActorDefinitionIdentifier* ad = SymCall("??0ActorDefinitionIdentifier@@QEAA@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
            ActorDefinitionIdentifier*, ActorDefinitionIdentifier*, string&)((ActorDefinitionIdentifier*)a, name);

        Spawner* sp = SymCall("?getSpawner@Level@@UEBAAEAVSpawner@@XZ", Spawner*, Level*)(mc->getLevel());

        Vec3 vec{ pos.x,pos.y,pos.z };
        Actor* ac = SymCall("?spawnMob@Spawner@@QEAAPEAVMob@@AEAVBlockSource@@AEBUActorDefinitionIdentifier@@PEAVActor@@AEBVVec3@@_N44@Z",
            Mob*, Spawner * _this, BlockSource*, ActorDefinitionIdentifier*, Actor*, Vec3*, bool, bool, bool)
            (sp, Raw_GetBlockSourceByDim(pos.dim), ad, nullptr, &vec, 0, 1, 0);

        return ac;
    }
    catch(...)
    {
        return nullptr;
    }
}

string Raw_GetEntityName(Actor* actor)
{
    string name;
    return SymCall("?getActorName@CommandUtils@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVActor@@@Z",
        std::string&, std::string*, Actor * actor)(&name, actor);
}

std::string Raw_GetEntityTypeName(Actor* actor)
{
    /*string res = SymCall("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
        string, int, int) (Raw_GetEntityTypeId(actor), 1);*/
    if (!actor)
        return "";
    if (Raw_IsPlayer(actor))
        return "minecraft:player";
    else
    {
        HashedString hash = dAccess<HashedString>(actor, 888);      //IDA Actor::Actor
        return hash.getString();
    }
}

int Raw_GetEntityTypeId(Actor* actor)
{
    Player* pl = Raw_ToPlayer(actor);
    if (!pl)
        return actor->getEntityTypeId();
    else
        return SymCall("?getEntityTypeId@Player@@UEBA?AW4ActorType@@XZ", int, Player*)(pl);
}

FloatVec4 Raw_GetEntityPos(Actor* actor)
{
    Vec3 pos;
    SymCall("?getFeetPos@CommandUtils@@YA?AVVec3@@PEBVActor@@@Z",
        Vec3*, Vec3*, Actor*)(&pos, actor);

    return {pos.x,pos.y,pos.z,Raw_GetEntityDimId(actor)};
}

IntVec4 Raw_GetEntityBlockPos(Actor* actor)
{
    auto pos = Raw_GetEntityPos(actor);
    return { (int)pos.x, (int)(pos.y+0.5), (int)pos.z, pos.dim };
}

int Raw_GetHealth(Actor *actor)
{
    return SymCall("?getHealth@Actor@@QEBAHXZ", int, Actor*)(actor);
}

int Raw_GetMaxHealth(Actor *actor)
{
    return SymCall("?getMaxHealth@Actor@@QEBAHXZ", int, Actor*)(actor);
}

bool Raw_GetIsInAir(Actor *actor)
{
    return !(dAccess<bool, 472>(actor)); // IDA DirectActorProxyImpl<IMobMovementProxy>::isOnGround
}

bool Raw_GetIsInWater(Actor* actor)
{
    return SymCall("?isInWater@Actor@@UEBA_NXZ", bool, Actor*)(actor);
}

bool Raw_GetAllEntities(vector<Actor*> &acs, int dimid)
{
    auto lv = mc->getLevel();
    auto dim = Raw_GetDimByLevel(lv, dimid);
    if (!dim)
        return false;
    auto& list = *(std::unordered_map<ActorUniqueID, void*>*)((uintptr_t)dim + 312); //IDA Dimension::registerEntity
    
    //Check Valid
    auto currTick = SymCall("?getCurrentTick@Level@@UEBAAEBUTick@@XZ"
        , Tick*, Level*)(lv)->t;
    for (auto& i : list)
    {
        //auto entity = SymCall("??$tryUnwrap@VActor@@$$V@WeakEntityRef@@QEBAPEAVActor@@XZ",
        //    Actor*, void*)(&i.second);
        auto entity = Raw_GetEntityByUniqueId(i.first);
        if (!entity)
            continue;
        auto lastTick = Raw_GetEntityLastTick(entity);
        if (!lastTick)
            continue;
        if (currTick - lastTick->t == 0 || currTick - lastTick->t == 1)
            acs.push_back(entity);
    }
    return true;
}

std::vector<Actor*> Raw_GetAllEntities()
{
    auto lv = (uintptr_t)mc->getLevel();
    std::vector<Actor*> entityList;
    Raw_GetAllEntities(entityList, 0);
    Raw_GetAllEntities(entityList, 1);
    Raw_GetAllEntities(entityList, 2);
    return entityList;
}

IntVec4 Raw_GetBlockStandingOn(Actor* ac)
{
    BlockPos bp;
    SymCall("?getBlockPosCurrentlyStandingOn@Actor@@QEBA?AVBlockPos@@PEAV1@@Z",
        void*, Actor*, BlockPos*, void*)(ac, &bp, nullptr);
    return { bp.x,bp.y,bp.z,Raw_GetEntityDimId(ac) };
}

bool Raw_TeleportEntity(Actor* actor, const FloatVec4 &pos)
{
    WActor(*actor).teleport({pos.x,pos.y,pos.z},pos.dim);
    return true;
}

bool Raw_KillEntity(Actor* actor)
{
    SymCall("?kill@Mob@@UEAAXXZ", void, void*)(actor);
    return true;
}

bool Raw_IsSimulatedPlayer(Actor* actor)
{
    if (!actor)
        return false;
    auto vtbl = dlsym("??_7SimulatedPlayer@@6B@");
    return *(void**)actor == vtbl;
}

bool Raw_IsPlayer(Actor* actor)
{
    if (!actor)
        return false;
    auto vtbl = dlsym("??_7ServerPlayer@@6B@");
    return *(void**)actor == vtbl || Raw_IsSimulatedPlayer(actor);
}

Player* Raw_ToPlayer(Actor* actor)
{
    return Raw_IsPlayer(actor) ? (Player*)actor : nullptr;
}

bool Raw_IsItemEntity(Actor* actor)
{
    return SymCall("?hasCategory@Actor@@QEBA_NW4ActorCategory@@@Z", bool, Actor*, int)(actor, 1024);    //IDA Player::take
}

ItemStack* Raw_ToItem(Actor* actor)
{
    return Raw_IsItemEntity(actor) ? (ItemStack*)((uintptr_t)actor + 1864) : nullptr;      //IDA Player::take
}

Container* Raw_GetArmor(Actor* ac)
{
    return SymCall("?getArmorContainer@Actor@@QEBAAEBVSimpleContainer@@XZ", Container*, Actor*)(ac);
}

int Raw_GetEntityDimId(Actor* actor)
{
    return WActor(*actor).getDimID();
}

bool Raw_HurtEntity(Actor* actor, int damage)
{
    char a[16];
    ActorDamageSource* ad = SymCall("??0ActorDamageSource@@QEAA@W4ActorDamageCause@@@Z",
        ActorDamageSource*, ActorDamageSource*, int)((ActorDamageSource*)a, 12);    //ActorDamageCause::Void
    return SymCall("?_hurt@Mob@@MEAA_NAEBVActorDamageSource@@H_N1@Z",
        bool, Mob*, ActorDamageSource*, int, bool, bool)((Mob*)actor, ad, damage, true, false);
}

bool Raw_SetOnFire(Actor* actor, int time) {
    SymCall("?setOnFire@Actor@@UEAAXH@Z", void, 
        Actor*, int)(actor, time);
    return true;
}

Actor* Raw_GetEntityByUniqueId(ActorUniqueID id) {
    return SymCall("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z",
        Actor*, Level*, ActorUniqueID, bool)(mc->getLevel(), id, 0);
}

float Raw_GetSpeed(Actor* actor) {
    return SymCall("?getSpeedInMetersPerSecond@Actor@@QEBAMXZ"
        , float, Actor*)(actor);
}

Vec2* Raw_GetDirction(Actor* actor)
{
    return (Vec2*)(actor + 312); // IDA: Actor::getRotation()
}

bool Raw_EntityIsMoving(Actor* ac)
{
    return SymCall("?isMoving@Actor@@QEBA_NXZ", bool, Actor*)(ac);
}

bool Raw_AddTag(Actor* ac, const string& str) {
    return SymCall("?addTag@Actor@@QEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
        bool, Actor*, const string*)(ac, &str);
}

bool Raw_RemoveTag(Actor* ac, const string& str) {
    return SymCall("?removeTag@Actor@@QEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
        bool, Actor*, const string*)(ac, &str);
}

std::vector<std::string> Raw_EntityGetAllTags(Actor* ac)
{
    try
    {
        auto &nbt = Tag::fromActor(ac)->asCompound();
        auto &list = nbt.at("Tags").asList();

        vector<string> res;
        for (auto& tag : list)
        {
            res.emplace_back(tag->asString());
        }
        return res;
    }
    catch (...)
    {
        return {};
    }
}

bool Raw_EntityHasTag(Actor* ac, const string& str)
{
    auto tags = Raw_EntityGetAllTags(ac);
    return find(tags.begin(), tags.end(), str) != tags.end();
}

bool Raw_Explode(FloatVec4 pos, Actor* source, float power, float range, float isDestroy, float isFire)
{
    Vec3 vec{ pos.x,pos.y,pos.z };

    SymCall("?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
        void, Level*, BlockSource*, Actor*, Vec3*, float, bool, bool, float, bool)
        (mc->getLevel(), Raw_GetBlockSourceByDim(pos.dim), source, &vec, power, isFire, isDestroy, range, true);
    return true;
}

void Raw_RefreshActorData(Actor* ac)
{
    SymCall("?_sendDirtyActorData@Actor@@QEAAXXZ", uintptr_t, Actor*)(ac);
}

bool Raw_RefreshItems(Actor* ac)
{
    SymCall("?sendInventory@Mob@@UEAAX_N@Z", void, Mob*)((Mob*)ac);
    bitset<4> bits("1111");
    SymCall("?sendArmor@Mob@@UEAAXAEBV?$bitset@$03@std@@@Z", void, Mob*, bitset<4>*)((Mob*)ac, &bits);
    return true;
}

bool Raw_EntityIsRemoved(Actor* ac)
{
    return SymCall("?isRemoved@Actor@@QEBA_NXZ", bool, Actor*)(ac);
}

Tick* Raw_GetEntityLastTick(Actor* ac)
{
    auto bs = Raw_GetBlockSourceByActor(ac);
    if (!bs)
        return nullptr;
    auto bpos = ((Vec3)ac->getPos()).toBlockPos();
    void* lc = SymCall("?getChunkAt@BlockSource@@QEBAPEAVLevelChunk@@AEBVBlockPos@@@Z",
        void*, BlockSource*, BlockPos*)(bs, &bpos);
    if (!lc)
        return nullptr;
    return SymCall("?getLastTick@LevelChunk@@QEBAAEBUTick@@XZ"
        , Tick*, void*)(lc);
}