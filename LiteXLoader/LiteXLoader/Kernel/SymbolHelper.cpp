#include "Global.h"
#include "SymbolHelper.h"
using namespace std;


BlockSource* Raw_GetBlockSourceByActor(Actor* actor)
{
	return SymCall("?getRegionConst@Actor@@QEBAAEBVBlockSource@@XZ", BlockSource*, Actor*)(actor);
}

int Raw_GetBlockDimensionId(BlockSource* bs)
{
	int dimid = -1;
	return *SymCall("?getDimensionId@BlockSource@@UEBA?AV?$AutomaticID@VDimension@@H@@XZ",
		int*, BlockSource*, int*)(bs, &dimid);
}

BlockSource* Raw_GetBlockSourceByDim(int dimid)
{
	auto dim = (int*) SymCall("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
		uintptr_t, void*, int)(mc->getLevel(), dimid);
	return dAccess<BlockSource*>(dim, 96);
}

Block* Raw_GetBlockByPos(BlockPos* bp, BlockSource* bs)
{
	return Raw_GetBlockByPos(bp->x, bp->y, bp->z, bs);
}

Block* Raw_GetBlockByPos(int x, int y, int z, BlockSource* bs)
{
	return SymCall("?getBlock@BlockSource@@UEBAAEBVBlock@@HHH@Z",
		Block*, BlockSource*, int, int, int)(bs, x, y, z);
}

Block* Raw_GetBlockByPos(BlockPos *bp, int dimid)
{
	auto bs = Raw_GetBlockSourceByDim(dimid);
	return Raw_GetBlockByPos(bp->x, bp->y, bp->z, bs);
}

Block* Raw_GetBlockByPos(IntVec4* pos)
{
	auto bs = Raw_GetBlockSourceByDim(pos->dim);
	return Raw_GetBlockByPos(pos->x, pos->y, pos->z, bs);
}

Actor* Raw_GetFishingHookOwner(FishingHook* fh)
{
	return SymCall("?getOwner@FishingHook@@QEAAPEAVActor@@XZ",
		Actor*, FishingHook*)(fh);
}

Actor* Raw_GetDamageSourceEntity(ActorDamageSource* ads)
{
	char v83;
	auto v6 = *(void**)(*(__int64(__fastcall**)(void*, char*))(*(uintptr_t*)ads + 64))(ads, &v83);
	return SymCall("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z",
		Actor*, Level*, void*, bool)(mc->getLevel(), v6, 0);
}