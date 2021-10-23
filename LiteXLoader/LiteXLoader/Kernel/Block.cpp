#include "Global.h"
#include "Block.h"
#include "Packet.h"
#include "Player.h"
#include "SymbolHelper.h"
using namespace std;

string Raw_GetBlockName(Block* block)
{
    HashedString* hashedstr = SymCall("?getName@Block@@QEBAAEBVHashedString@@XZ",
        HashedString*, void*)(block);

    return hashedstr->getString();
}

std::string Raw_GetBlockType(Block* block)
{
    BlockLegacy* blockLegacy = offBlock::getLegacyBlock(block);
    return offBlockLegacy::getFullName(blockLegacy);
}

int Raw_GetBlockId(Block* block)
{
    BlockLegacy* blockLegacy = offBlock::getLegacyBlock(block);
    return blockLegacy->getBlockItemId();
}

Block* Raw_GetBlockFromBlockLegacy(BlockLegacy* blk, unsigned short tileData) {
    auto block = SymCall("?getStateFromLegacyData@BlockLegacy@@UEBAAEBVBlock@@G@Z", Block*, BlockLegacy*, unsigned short)(blk, tileData);
    // 某些方块在 tileData 太大时会变成其他方块，原版 /setblock 指令就存在这个问题（也有可能是被设计成这样的？）
    if (block && offBlock::getLegacyBlock(block) == blk)
        return block;
    return SymCall("?getRenderBlock@BlockLegacy@@UEBAAEBVBlock@@XZ", Block*, BlockLegacy*)(blk);
}

unsigned short Raw_GetTileData(Block* bl)
{
    // 等待大佬改进
    auto tileData = dAccess<unsigned short, 8>(bl);
    auto blk = offBlock::getLegacyBlock(bl);
    if (Raw_GetBlockFromBlockLegacy(blk, tileData) == bl)
        return tileData;
    for (unsigned short i = 0; i < 16; ++i) {
        if (i == tileData)
            continue;
        if (Raw_GetBlockFromBlockLegacy(blk, i) == bl)
            return i;
    }
    ERROR("Error in Raw_GetTileData");
    return 0;
}

struct BlockPalette;
Block* Raw_NewBlockFromNameAndTileData(string name, unsigned short tileData)
{
    BlockPalette* generator = SymCall("?getBlockPalette@Level@@UEBAAEBVBlockPalette@@XZ", BlockPalette*, Level*)(mc->getLevel());
    BlockLegacy* blk = SymCall("?getBlockLegacy@BlockPalette@@QEBAPEBVBlockLegacy@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
        BlockLegacy*, void*, string*)(generator, &name);
    if (!blk)
        return nullptr;
    return Raw_GetBlockFromBlockLegacy(blk, tileData);    //SetBlockCommand::execute
}

Block* Raw_NewBlockFromNbt(Tag* tag)
{
    pair<int, Block*> result; // pair<enum BlockSerializationUtils::NBTState, Block*>
    SymCall("?tryGetBlockFromNBT@BlockSerializationUtils@@YA?AU?$pair@W4NBTState@BlockSerializationUtils@@PEBVBlock@@@std@@AEBVCompoundTag@@PEAUNbtToBlockCache@1@@Z",
        void*, void*, Tag*, int64_t)(&result, tag, 0);
    return result.second;
}

bool Raw_SetBlockByBlock(IntVec4 pos, Block* block)
{
    BlockSource* bs = Raw_GetBlockSourceByDim(pos.dim);

    if (!SymCall("?setBlock@BlockSource@@QEAA_NAEBVBlockPos@@AEBVBlock@@HPEBUActorBlockSyncMessage@@@Z", 
        bool, BlockSource*, BlockPos, Block*, int, void*) (bs, { pos.x, pos.y, pos.z }, block, 0, nullptr))
        return false;

    auto pls = Raw_GetOnlinePlayers();
    for (auto& pl : pls)
        Raw_ResendBlocksAroundPlayer(pl,pos);
    return true;
}

bool Raw_SetBlockByNameAndTileData(IntVec4 pos, const string& name, unsigned short tileData)
{
    Block* newBlock = Raw_NewBlockFromNameAndTileData(name, tileData);
    if (!newBlock)
        return false;
    return Raw_SetBlockByBlock(pos, newBlock);
}

bool Raw_SetBlockByNbt(IntVec4 pos, Tag* nbt)
{
    Block* newBlock = Raw_NewBlockFromNbt(nbt);
    if (!newBlock)
        return false;
    return Raw_SetBlockByBlock(pos, newBlock);
}

bool Raw_SpawnParticle(FloatVec4 pos, const string& type)
{
    string name = type;
    Level* level = mc->getLevel();
    Dimension *dim = SymCall("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
        Dimension*, void*, AutomaticID<Dimension, int>)(level, AutomaticID<Dimension, int>(pos.dim));

    SymCall("?spawnParticleEffect@Level@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVVec3@@PEAVDimension@@@Z",
        void, Level*, string&, const Vec3&, void*)
        (level, name, { pos.x,pos.y,pos.z }, dim);

    return true;
}

bool Raw_ResendBlocksAroundPlayer(Player* pl, IntVec4 pos)
{
    BlockPos bp{ pos.x,pos.y,pos.z };
    SymCall("?resendBlocksAroundArea@ItemUseInventoryTransaction@@QEBAXAEAVPlayer@@AEBVBlockPos@@E@Z",
        void, void*, Player*, BlockPos*, char)(nullptr, pl, &bp, 0);     // this* has not been used
    return true;
}