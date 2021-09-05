#include <ScriptX/ScriptX.h>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <mutex>
using namespace script;


//////////////////// Structs ////////////////////

//导出函数表
struct ExportedFuncData
{
	std::string fromEngineType;
	ScriptEngine* engine;
	Global<Function> func;
};

//远程调用信息
struct RemoteEngineData
{
	unsigned threadId;
};

//远程调用信息
struct MessageSystemData
{
	unsigned threadId;
};

//全局共享数据
struct GlobalDataType
{
	//所有插件名单
	std::vector<std::string> pluginsList;

	//导出函数表
	std::unordered_map<std::string, ExportedFuncData> exportedFuncs;

	//模块消息系统线程集合
	std::unordered_map<std::string, MessageSystemData> moduleMessageSystemsList;
	int messageSystemNextId = 0;

	//模块消息系统等待队列
	std::mutex syncWaitListLock;
	std::unordered_map<int, bool> syncWaitList;

	//插件热管理多线程锁
	std::mutex hotManageLock;

	//fastlog多线程锁
	std::mutex fastlogLock;
};


//////////////////// Externs ////////////////////

//全局共享数据
extern GlobalDataType* globalShareData;


//////////////////// APIs ////////////////////

void InitGlobalShareData();
void AddToGlobalPluginsList(const std::string& name);
void RemoveFromGlobalPluginsList(const std::string& name);