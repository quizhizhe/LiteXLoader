#include "Container.h"
#include "Global.h"
#include "Item.h"
#include "SymbolHelper.h"
using namespace std;

std::string Raw_GetContainerTypeName(Container* container)
{
	char type = dAccess<char>(container, 8);	//IDA Container::Container
	return SymCall("?getContainerTypeName@Container@@SAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ContainerType@@@Z",
		string&, char)(type);
}

bool Raw_AddItem(Container* container, ItemStack* item)
{
	if (!Raw_HasRoomFor(container,item))
		return false;

	container->addItem(*Raw_CloneItem(item));
	return true;
}

bool Raw_AddItemToFirstEmptySlot(Container* container, ItemStack* item)
{
	return container->addItemToFirstEmptySlot(*Raw_CloneItem(item));
}

bool Raw_HasRoomFor(Container* container, ItemStack* item)
{
	return container->hasRoomForItem(*item);
}

bool Raw_RemoveItem(Container* container, int slot, int number)
{
	if (slot > Raw_GetContainerSize(container))
		return false;
	container->removeItem(slot, number);
	return true;
}

ItemStack* Raw_GetSlot(Container* container, int slot)
{
	if (slot > Raw_GetContainerSize(container))
		return nullptr;
	auto list = Raw_GetAllSlots(container);
	return list[slot];
}

std::vector<ItemStack*> Raw_GetAllSlots(Container* container)
{
	return container->getSlots();
}

bool Raw_RemoveAllItems(Container* container)
{
	container->removeAllItems();
	return true;
}

bool Raw_IsEmpty(Container* container)
{
	return container->isEmpty();
}

int Raw_GetContainerSize(Container* container)
{
	using _QWORD = unsigned long long;
	return (*(signed int(__fastcall**)(__int64))(*(_QWORD*)container + 112i64))((unsigned long long)container);		//IDA Container::getSlotCopies
}

bool Raw_HasContainer(FloatVec4 pos)
{
	return Raw_GetContainer(pos) != nullptr;
}

class DropperBlockActor;
Container* Raw_GetContainer(FloatVec4 pos)
{
	Vec3 vec{ pos.x, pos.y, pos.z };

	// This function didn't use 'this' pointer
	Container* container = SymCall("?_getContainerAt@DropperBlockActor@@AEAAPEAVContainer@@AEAVBlockSource@@AEBVVec3@@@Z",
		Container*, DropperBlockActor*, BlockSource*, Vec3*)(nullptr, Raw_GetBlockSourceByDim(pos.dim), &vec);

	return container;
}