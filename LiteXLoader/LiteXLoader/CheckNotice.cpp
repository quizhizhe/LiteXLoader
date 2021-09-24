#include "pch.h"

#define LXL_UPDATE_CHECK "https://lxl-upgrade.amd.rocks/LXL/LXL.json"

#include "CheckNotice.h"
#include <Kernel/System.h>
#include <Configs.h>
#include <string>
#include <Kernel/Global.h>
#include <Kernel/ThirdParty.h>

using namespace std;

void ProcessData(int code, string body)
{
	if(code == 200)
	{
		try
		{
			JSON_ROOT data = JSON_ROOT::parse(body);

			//公告信息
			if (data["notice"].is_array())
			{
				for (auto& element : data["notice"])
					INFO(element.get<string>());
			}
		}
		catch (JSON_ROOT::exception& e) {
			DEBUG(string("LXL通知信息解析失败。")+e.what());
		}
	}
	else
	{
		DEBUG(string("LXL获取通知信息失败。错误码：") + to_string(code));
	}
}

void CheckNotice()
{
	if (!Raw_HttpGet(LXL_UPDATE_CHECK, ProcessData))
	{
		DEBUG("LXL获取通知信息失败。");
	}
}