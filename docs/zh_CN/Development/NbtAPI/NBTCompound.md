## 📒 NbtCompound - 标签类型

一个`NbtCompound `代表一个完整的标签，里面储存了一系列键值对，类似于对象 / 表这样的数据结构。  
通过`NbtCompound`对象的接口，才可以进行 NBT 标签所独有的一些操作。    
对于某个一个`NbtCompound`类型的对象`comp`，有如下这些接口

#### 获取所有的键

`comp.getKeys()`

- 返回值：Compound 中所有的键
- 返回值类型：`Array<String,String,...>`

<br>

#### 获取键对应的值的数据类型

`comp.getTypeOf(key)`

- 参数：
  - key: `String`  
    要查询的键名
- 返回值：对应的值的数据类型
- 返回值类型：`Enum`
  - 如果要读取的NBT不存在，将返回`Null`

可能的返回值有：`NBT.End` `NBT.Byte` `NBT.Short` `NBT.Int` `NBT.Long`   
`NBT.Float` `NBT.Double` `NBT.ByteArray` `NBT.String`  
`NBT.List` `NBT.Compound`

<br>

#### 设置键对应的NBT对象

`comp.setTag(key,tag)`

- 参数：
  - key: `String`   
    要操作的键名
  - tag: `NbtValue` 或 `NbtList` 或 `NbtCompound`  
    要写入的 NBT 对象（它承载着具体的NBT数据）  
    写入数据类型必须和键对应的值储存的数据类型一致，键名可以不存在
- 返回值：是否成功写入
- 返回值类型：`Boolean`

<br>

#### 读取键对应的NBT对象

`comp.getTag(key)`

- 参数：
  - key: `String`   
    要操作的键名
- 返回值：键对应的NBT对象
- 返回值类型：`NbtValue` 或 `NbtList` 或 `NbtCompound` 
  - 如果要读取的NBT不存在，将返回`Null`

<br>

#### 删除键对应的NBT对象

`list.removeTag(key)`

- 参数：
  - key: `String`  
    要操作的键名。键名必须已经存在
- 返回值：处理完毕的NBT对象（便于连锁进行其他操作）
- 返回值类型：`NbtCompound`

<br>

## 一些协助 NBT 对象操作的方便函数

很明显，在修改NBT的某些值的时候，纯粹使用NBT对象会显得麻烦而语法复杂。  
因此，这里还提供了一些简化对象操作的方便函数，通过直接操作指定位置的数据，而避免通过NBT对象中转，可以一定程度上降低代码量

#### 设置键对应的值的具体数据

`comp.setEnd(key)`  
`comp.setByte(key,data)`  
`comp.setShort(key,data)`  
`comp.setInt(key,data)`  
`comp.setLong(key,data)`  
`comp.setFloat(key,data)`  
`comp.setDouble(key,data)`  
`comp.setString(key,data)`    

- 参数：
  - key: `String`   
    要操作的键名
  - data: `上述类型`  
    要写入的具体数据  
    写入数据类型必须和键对应的值储存的数据类型一致，键名可以不存在
- 返回值：写入完毕的NBT对象（便于连锁进行其他操作）
- 返回值类型：`NbtCompound`

<br>

#### 读取键对应的值的具体数据

`comp.getData(key)`  

- 参数：
  - key: `String`   
    要操作的键名
- 返回值：键对应的值的具体数据
- 返回值类型：`任意类型`，以储存的数据类型为准
  - 如果目标位置储存的是`List`类型 NBT，将返回一个`NbtList`对象；如果目标位置储存的是`Compound`类型 NBT，将返回一个`NbtCompound`对象
  - 如果要读取的NBT不存在，将返回`Null`

<br>

#### 将Compound类型转换为Object

`comp.toObject()`

- 返回值：对应的对象 / 表
- 返回值类型：`Object`

将Compound的内容转换为LXL对象 / 表，把数据项都转换为LXL数据类型储存于对象 / 表的对应key中，方便读取和处理  
如果Compound某一项储存的是`List`或者`Compound`类型的 NBT，将在对应位置递归展开为`Array`或`Object`

<br>

