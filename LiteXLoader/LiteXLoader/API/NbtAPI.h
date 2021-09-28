#pragma once
#include <ScriptX/ScriptX.h>
#include <Kernel/NBT.h>
using namespace script;

extern struct Tag;
extern ClassDefine<void> NbtStaticBuilder;

class NbtBase
{
public:
    Tag* nbt;
    bool canDelete = false;

    //virtual Local<Value> getType(const Arguments& args) = 0;
    Local<Value> toString(const Arguments& args);
    Local<Value> destroy(const Arguments& args);
};


//NBT Static
class NbtStatic : public ScriptClass
{
public:
    static Local<Value> newTag(const Arguments& args);
    static Local<Value> parseSNBT(const Arguments& args);
    static Local<Value> parseBinaryNBT(const Arguments& args);

    template<TagType T>
    static Local<Value> getType()
    {
        return Number::newNumber((int)T);
    }
};
extern ClassDefine<void> NbtStaticBuilder;


//NBT End
class NbtEndClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtEndClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtEndClass(Tag* p, bool canDelete = false);
    ~NbtEndClass() = default;
    static NbtEndClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);

    Local<Value> getType(const Arguments& args);
    Local<Value> set(const Arguments& args);
    Local<Value> get(const Arguments& args);
};
extern ClassDefine<NbtEndClass> NbtEndClassBuilder;


//NBT Byte
class NbtByteClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtByteClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtByteClass(Tag* p, bool canDelete = false);
    NbtByteClass() = default;
    static NbtByteClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);

    Local<Value> getType(const Arguments& args);
    Local<Value> set(const Arguments& args);
    Local<Value> get(const Arguments& args);
};
extern ClassDefine<NbtByteClass> NbtByteClassBuilder;


//NBT Short
class NbtShortClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtShortClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtShortClass(Tag* p, bool canDelete = false);
    ~NbtShortClass() = default;
    static NbtShortClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);

    Local<Value> getType(const Arguments& args);
    Local<Value> set(const Arguments& args);
    Local<Value> get(const Arguments& args);
};
extern ClassDefine<NbtShortClass> NbtShortClassBuilder;


//NBT Int
class NbtIntClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtIntClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtIntClass(Tag* p, bool canDelete = false);
    ~NbtIntClass() = default;
    static NbtIntClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);
    
    Local<Value> getType(const Arguments& args);
    Local<Value> set(const Arguments& args);
    Local<Value> get(const Arguments& args);
};
extern ClassDefine<NbtIntClass> NbtIntClassBuilder;


//NBT Long
class NbtLongClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtLongClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtLongClass(Tag* p, bool canDelete = false);
    ~NbtLongClass() = default;
    static NbtLongClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);

    Local<Value> getType(const Arguments& args);
    Local<Value> set(const Arguments& args);
    Local<Value> get(const Arguments& args);
};
extern ClassDefine<NbtLongClass> NbtLongClassBuilder;


//NBT Float
class NbtFloatClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtFloatClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtFloatClass(Tag* p, bool canDelete = false);
    ~NbtFloatClass() = default;
    static NbtFloatClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);

    Local<Value> getType(const Arguments& args);
    Local<Value> set(const Arguments& args);
    Local<Value> get(const Arguments& args);
};
extern ClassDefine<NbtFloatClass> NbtFloatClassBuilder;


//NBT Double
class NbtDoubleClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtDoubleClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtDoubleClass(Tag* p, bool canDelete = false);
    ~NbtDoubleClass() = default;
    static NbtDoubleClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);

    Local<Value> getType(const Arguments& args);
    Local<Value> set(const Arguments& args);
    Local<Value> get(const Arguments& args);
};
extern ClassDefine<NbtDoubleClass> NbtDoubleClassBuilder;


//NBT String
class NbtStringClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtStringClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtStringClass(Tag* p, bool canDelete = false);
    ~NbtStringClass() = default;
    static NbtStringClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);

    Local<Value> getType(const Arguments& args);
    Local<Value> set(const Arguments& args);
    Local<Value> get(const Arguments& args);
};
extern ClassDefine<NbtStringClass> NbtStringClassBuilder;


//NBT ByteArray
class NbtByteArrayClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtByteArrayClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtByteArrayClass(Tag* p, bool canDelete = false);
    ~NbtByteArrayClass() = default;
    static NbtByteArrayClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);

    Local<Value> getType(const Arguments& args);
    Local<Value> set(const Arguments& args);
    Local<Value> get(const Arguments& args);
};
extern ClassDefine<NbtByteArrayClass> NbtByteArrayClassBuilder;


//NBT List
class NbtListClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtListClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtListClass(Tag* p, bool canDelete = false);
    ~NbtListClass() = default;
    static NbtListClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);
    
    Local<Value> getType(const Arguments& args);
    Local<Value> getSize(const Arguments& args);
    Local<Value> getTypeOf(const Arguments& args);

    Local<Value> setEnd(const Arguments& args);
    Local<Value> setByte(const Arguments& args);
    Local<Value> setInt(const Arguments& args);
    Local<Value> setShort(const Arguments& args);
    Local<Value> setLong(const Arguments& args);
    Local<Value> setFloat(const Arguments& args);
    Local<Value> setDouble(const Arguments& args);
    Local<Value> setString(const Arguments& args);
    Local<Value> setByteArray(const Arguments& args);
    Local<Value> setTag(const Arguments& args);
    Local<Value> addTag(const Arguments& args);
    Local<Value> removeTag(const Arguments& args);

    Local<Value> getData(const Arguments& args);
    Local<Value> getTag(const Arguments& args);

    Local<Value> toArray(const Arguments& args);
};
extern ClassDefine<NbtListClass> NbtListClassBuilder;


//NBT Compound
class NbtCompoundClass : public NbtBase, public ScriptClass
{
public:
    explicit NbtCompoundClass(const Local<Object>& scriptObj, Tag* p, bool canDelete = false);
    explicit NbtCompoundClass(Tag* p, bool canDelete = false);
    ~NbtCompoundClass() = default;
    static NbtCompoundClass* constructor(const Arguments& args);
    static Tag* extract(Local<Value> v);
    static Local<Value> pack(Tag* tag, bool canDelete = false);

    Local<Value> getType(const Arguments& args);
    Local<Value> getKeys(const Arguments& args);
    Local<Value> getTypeOf(const Arguments& args);

    Local<Value> setEnd(const Arguments& args);
    Local<Value> setByte(const Arguments& args);
    Local<Value> setInt(const Arguments& args);
    Local<Value> setShort(const Arguments& args);
    Local<Value> setLong(const Arguments& args);
    Local<Value> setFloat(const Arguments& args);
    Local<Value> setDouble(const Arguments& args);
    Local<Value> setString(const Arguments& args);
    Local<Value> setByteArray(const Arguments& args);
    Local<Value> setTag(const Arguments& args);
    Local<Value> removeTag(const Arguments& args);

    Local<Value> getData(const Arguments& args);
    Local<Value> getTag(const Arguments& args);

    Local<Value> toObject(const Arguments& args);
    Local<Value> toSNBT(const Arguments& args);
    Local<Value> toBinaryNBT(const Arguments& args);
};
extern ClassDefine<NbtCompoundClass> NbtCompoundClassBuilder;


//Helper
bool IsNbtClass(Local<Value> value);
Local<Value> Tag2Value(Tag* nbt, bool autoExpansion = false);
bool TagSetValue(TagType type, Tag* nbt, Local<Value> value = Local<Value>());