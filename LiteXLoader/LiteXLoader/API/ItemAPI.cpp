#include "APIHelp.h"
#include "BaseAPI.h"
#include "ItemAPI.h"
#include "EntityAPI.h"
#include <Kernel/Item.h>
#include "NbtAPI.h"
#include <Kernel/NBT.h>
#include <vector>
#include <string>
using namespace script;

//////////////////// Class Definition ////////////////////

ClassDefine<ItemClass> ItemClassBuilder =
    defineClass<ItemClass>("LXL_Item")
        .constructor(nullptr)
        .instanceFunction("getRawPtr", &ItemClass::getRawPtr)

        .instanceProperty("name", &ItemClass::getName)
        .instanceProperty("type", &ItemClass::getType)
        .instanceProperty("id", &ItemClass::getId)
        .instanceProperty("count", &ItemClass::getCount)
        .instanceProperty("aux", &ItemClass::getAux)

        .instanceFunction("isNull", &ItemClass::isNull)
        .instanceFunction("setLore", &ItemClass::setLore)
        .instanceFunction("setNbt", &ItemClass::setNbt)
        .instanceFunction("getNbt", &ItemClass::getNbt)

        //For Compatibility
        .instanceFunction("setTag", &ItemClass::setNbt)
        .instanceFunction("getTag", &ItemClass::getNbt)
        .build();


//////////////////// Classes ////////////////////

ItemClass::ItemClass(ItemStack *p)
    :ScriptClass(ScriptClass::ConstructFromCpp<ItemClass>{}),item(p)
{
    preloadData();
}

//生成函数
Local<Object> ItemClass::newItem(ItemStack *p)
{
    auto newp = new ItemClass(p);
    return newp->getScriptObject();
}
ItemStack* ItemClass::extractItem(Local<Value> v)
{
    if(EngineScope::currentEngine()->isInstanceOf<ItemClass>(v))
        return EngineScope::currentEngine()->getNativeInstance<ItemClass>(v)->get();
    else
        return nullptr;
}

//成员函数
void ItemClass::preloadData()
{
    name = Raw_GetCustomName(item);
    if (name.empty())
        name = Raw_GetItemName(item);

    type = Raw_GetItemTypeName(item);
    id = Raw_GetItemId(item);
    count = Raw_GetCount(item);
    aux = Raw_GetItemAux(item);
}

Local<Value> ItemClass::getName()
{ 
    try{
        //已预加载
        return String::newString(name);
    }
    CATCH("Fail in GetItemName!")
}

Local<Value> ItemClass::getType()
{
    try{
        //已预加载
        return String::newString(type);
    }
    CATCH("Fail in GetType!")
}

Local<Value> ItemClass::getId()
{
    try {
        //已预加载
        return Number::newNumber(id);
    }
    CATCH("Fail in GetType!")
}

Local<Value> ItemClass::getCount()
{
    try{
        //已预加载
        return Number::newNumber(count);
    }
    CATCH("Fail in GetCount!")
}

Local<Value> ItemClass::getAux()
{
    try{
        //已预加载
        return Number::newNumber(aux);
    }
    CATCH("Fail in GetAux!")
}

Local<Value> ItemClass::getRawPtr(const Arguments& args)
{
    try {
        return Number::newNumber((intptr_t)item);
    }
    CATCH("Fail in getRawPtr!")
}

Local<Value> ItemClass::isNull(const Arguments& args)
{
    try{
        return Boolean::newBoolean(Raw_IsNull(item));
    }
    CATCH("Fail in IsNull!")
}

Local<Value> ItemClass::setLore(const Arguments& args)
{
    CHECK_ARGS_COUNT(args,1)
    CHECK_ARG_TYPE(args[0],ValueKind::kArray)

    try{
        auto arr = args[0].asArray();
        std::vector<std::string> lores;
        for(int i=0;i<arr.size();++i)
        {
            auto value = arr.get(i);
            if(value.getKind() == ValueKind::kString)
                lores.push_back(value.asString().toString());
        }
        if(lores.empty())
            return Boolean::newBoolean(false);
        
        Raw_SetLore(item, lores);
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in SetLore!")
}

Local<Value> ItemClass::getNbt(const Arguments& args)
{
    try {
        return NbtCompound::newNBT(Tag::fromItem(item));
    }
    CATCH("Fail in getNbt!")
}

Local<Value> ItemClass::setNbt(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1);

    try {
        auto nbt = NbtCompound::extractNBT(args[0]);
        if (!nbt)
            return Local<Value>();    //Null

        nbt->setItem(item);
        return Boolean::newBoolean(true);
    }
    CATCH("Fail in setNbt!")
}

Local<Value> SpawnItem(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 2);

    try {
        FloatVec4 pos;
        if (args.size() == 2)
        {
            // FloatPos
            auto posObj = FloatPos::extractPos(args[1]);
            if (posObj)
            {
                if (posObj->dim < 0)
                    return Local<Value>();
                else
                    pos = *posObj;
            }
            else
            {
                ERROR("Wrong type of argument in SpawnItem!");
                return Local<Value>();
            }
        }
        else if (args.size() == 5)
        {
            // Number Pos
            CHECK_ARG_TYPE(args[1], ValueKind::kNumber);
            CHECK_ARG_TYPE(args[2], ValueKind::kNumber);
            CHECK_ARG_TYPE(args[3], ValueKind::kNumber);
            CHECK_ARG_TYPE(args[4], ValueKind::kNumber);
            pos = { args[1].asNumber().toFloat(), args[2].asNumber().toFloat(), args[3].asNumber().toFloat(), args[4].toInt() };
        }
        else
        {
            ERROR("Wrong number of arguments in SpawnItem!");
            return Local<Value>();
        }

        Actor* entity = nullptr;

        ItemStack* it = ItemClass::extractItem(args[0]);
        if (it)
        {
            //By Item
            entity = Raw_SpawnItemByItemStack(it, pos);
        }
        else
        {
            Tag* nbt = NbtCompound::extractNBT(args[0]);
            if (nbt)
            {
                //By NBT
                entity = Raw_SpawnItemByNBT(nbt, pos);
            }
            else
            {
                ERROR("Wrong type of argument in SpawnItem!");
                return Local<Value>();
            }
        }

        if (!entity)
            return Local<Value>();    //Null
        else
            return EntityClass::newEntity(entity);
    }
    CATCH("Fail in SpawnItem!");
}