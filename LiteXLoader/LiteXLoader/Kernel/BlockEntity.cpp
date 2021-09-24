#include "pch.h"

#include "Global.h"
#include "BlockEntity.h"
#include "Block.h"
#include "SymbolHelper.h"
using namespace std;

BlockActor* Raw_GetBlockEntity(IntVec4 pos)
{
	BlockPos bp{ pos.x,pos.y,pos.z };
	return SymCall("?getBlockEntity@BlockSource@@QEAAPEAVBlockActor@@AEBVBlockPos@@@Z",
		BlockActor*, BlockSource*, BlockPos*)(Raw_GetBlockSourceByDim(pos.dim), &bp);
}

bool Raw_HasBlockEntity(Block* bl)
{
	return SymCall("?hasBlockEntity@Block@@QEBA_NXZ", bool, Block*)(bl);
}

int Raw_GetBlockEntityType(Block* bl)
{
	return dAccess<unsigned int>(bl, 188);		//IDA Block::getBlockEntityType
}

unsigned int Raw_GetBlockEntityType(BlockActor* ba)
{
	return *SymCall("?getType@BlockActor@@QEBAAEBW4BlockActorType@@XZ", unsigned int*, BlockActor*)(ba);
}

BlockPos Raw_GetBlockEntityPos(BlockActor* ba)
{
	return ba->getPosition();
}

bool Raw_RemoveBlockEntity(IntVec4 pos)
{
	int res;
	BlockPos bp{ pos.x,pos.y,pos.z };
	SymCall("?removeBlockEntity@BlockSource@@QEAA?AV?$shared_ptr@VBlockActor@@@std@@AEBVBlockPos@@@Z",
		int*, BlockSource*, BlockPos*, int*)(Raw_GetBlockSourceByDim(pos.dim), &bp, &res);
	return true;
}
