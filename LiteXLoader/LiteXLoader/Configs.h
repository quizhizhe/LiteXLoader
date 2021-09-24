#pragma once
#include <ScriptX/ScriptX.h>

////////////// Configs //////////////

// 后端宏
#define LXL_LANG_JS "Js"
#define LXL_LANG_LUA "Lua"

#if defined(SCRIPTX_LANG_JAVASCRIPT)
	#define LXL_SCRIPT_LANG_TYPE LXL_LANG_JS
	#define LXL_PLUGINS_SUFFIX ".js"
#elif defined(SCRIPTX_LANG_LUA)
	#define LXL_SCRIPT_LANG_TYPE LXL_LANG_LUA
	#define LXL_PLUGINS_SUFFIX ".lua"
#endif

#define LXL_MODULE_TYPE LXL_SCRIPT_LANG_TYPE

// 配置文件
#define LXL_PLUGINS_LOAD_DIR "./plugins"
#define LXL_LANGPACK_DIR "./plugins/LiteXLoader/LangPack/"
#define LXL_CONFIG_PATH "./plugins/LiteXLoader/config.ini"

// 基础库 & 依赖库
#define LXL_DEPENDS_DIR "./plugins/lib"

// 全局调试
#if defined(SCRIPTX_LANG_JAVASCRIPT)
	#define LXL_DEBUG_CMD "jsdebug"
#elif defined(SCRIPTX_LANG_LUA)
	#define LXL_DEBUG_CMD "luadebug"
#endif
#define LXL_DEBUG_ENGINE_NAME "__LXL_DEBUG_ENGINE__"

// 全局通信
#define LXL_GLOBAL_DATA_NAME L"LITEXLOADER_GLOBAL_DATA_SECTION"
#define LXL_REMOTE_CALL_EVENT_NAME L"LITEXLOADER_REMOTE_CALL_EVENT"
#define LXL_MESSAGE_SYSTEM_WAIT_CHECK_INTERVAL 5

//超时
#define LXL_MAXWAIT_REMOTE_LOAD 10*1000
#define LXL_MAXWAIT_REMOTE_CALL 60*1000

//线程池
#define LXL_POOL_THREAD_COUNT 4