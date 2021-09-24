#include "pch.h"

#include "Global.h"
#include "Item.h"
#include "NBT.h"
#include "SymbolHelper.h"
#include <string>
#include <vector>
using namespace std;

ItemStack* Raw_NewItem()
{
    try
    {
        ItemStack* a = (ItemStack*) new char[272];
        ItemStack* item = SymCall("??0ItemStack@@QEAA@XZ", ItemStack*, ItemStack*)(a);
        return item;
    }
    catch (...)
    {
        return nullptr;
    }
}

ItemStack* Raw_NewItem(std::string type, int count)
{
    Tag* nbt = Tag::createTag(TagType::Compound);
    nbt->putByte("WasPickedUp", 0);
    nbt->putShort("Damage", 0);
    nbt->putString("Name", type);
    nbt->putByte("Count", count);

    return Raw_NewItem(nbt);
}

ItemStack* Raw_NewItem(Tag* tag)
{
    ItemStack* item = Raw_NewItem();
    if (!item)
        return nullptr;
    tag->setItem(item);

    return item;
}

ItemStack* Raw_CloneItem(ItemStack* item)
{
    ItemStack* it = Raw_NewItem();
    if (!item)
        return nullptr;
    return SymCall("?clone@ItemStack@@QEBA?AV1@XZ", ItemStack*, ItemStack*, ItemStack*)(item,it);
}

class Spawner;
Actor* Raw_SpawnItemByItemStack(ItemStack* item, const FloatVec4& pos)
{
    try
    {
        Spawner* sp = SymCall("?getSpawner@Level@@UEBAAEAVSpawner@@XZ", Spawner*, Level*)(mc->getLevel());

        Vec3 vec{ pos.x,pos.y,pos.z };
        Actor* ac = SymCall("?spawnItem@Spawner@@QEAAPEAVItemActor@@AEAVBlockSource@@AEBVItemStack@@PEAVActor@@AEBVVec3@@H@Z",
            Actor*, Spawner * _this, BlockSource*, ItemStack*, Actor*, Vec3*, int)
            (sp, Raw_GetBlockSourceByDim(pos.dim), item, nullptr, &vec, 0);

        return ac;
    }
    catch (...)
    {
        return nullptr;
    }
}

string Raw_GetItemName(ItemStack* item)
{
    if (Raw_IsNull(item))
        return "";
    return item->getName();
}

string Raw_GetCustomName(ItemStack* item)
{
    if (Raw_IsNull(item))
        return "";
    return item->getCustomName();
}

std::string Raw_GetItemTypeName(ItemStack* item)
{
    if (Raw_IsNull(item))
        return "";
    string fullName;
    SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		void, Item const*, string*)(item->getItem(), &fullName);
    return fullName; 
}

int Raw_GetItemId(ItemStack* item)
{
    return item->getId();
}

int Raw_GetItemAux(ItemStack* item)
{
    if (Raw_IsNull(item))
        return 0;
    return item->getAuxValue();
}

int Raw_GetCount(ItemStack* item)
{
    if (Raw_IsNull(item))
        return 0;
    return offItemStack::getCount(item);
}

bool Raw_SetItem(ItemStack* oldItem, ItemStack* newItem)
{
    Tag* nbt = Tag::fromItem(newItem);
    nbt->setItem(oldItem);
    return true;
}

bool Raw_IsNull(ItemStack* item)
{
    return item->isNull();
}

bool Raw_SetNull(ItemStack* item)
{
    SymCall("?setNull@ItemStack@@UEAAXXZ", void, ItemStack*)(item);
    return true;
}

bool Raw_SetItemAux(ItemStack* item, int aux)
{
    SymCall("?setAuxValue@ItemStackBase@@QEAAXF@Z", void, ItemStack*, short)(item, aux);
    return true;
}

bool Raw_SetLore(ItemStack* item, vector<string> lores)
{
    if (Raw_IsNull(item))
        return false;

    SymCall("?setCustomLore@ItemStackBase@@QEAAXAEBV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@@Z",
		void, void*, vector<string>)(item, lores);
    return true;
}