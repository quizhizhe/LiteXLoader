#define LXL_UPDATE_DOMAIN "https://lxl-upgrade.amd.rocks"
#define LXL_UPDATE_INFO_REMOTE "/LXL/LXL.json"

#define LXL_UPDATE_CHECK_INTERVAL 10*60
#define LXL_UPDATE_CHECK_TIMEOUT 60

#define LXL_UPDATE_PROGRAM "plugins/LiteXLoader/LXLAutoUpdate.dll"
#define LXL_UPDATE_CHECK_PRELOAD "plugins/preload.conf"

#define LXL_UPDATE_CACHE_PATH "plugins/LiteXLoader/Update/"
#define LXL_UPDATE_INFO_RECORD "plugins/LiteXLoader/Update/Update.ini"

#define LXL_UPDATE_OTHER_FILES_RECORD "plugins/LiteXLoader/Versions.ini"

#include <string>
#include <thread>
#include <chrono>
#include <process.h>
#include <filesystem>
#include <Version.h>
#include <Kernel/Base.h>
#include <Kernel/Global.h>
#include <Kernel/ThirdParty.h>
#include <Kernel/Utils.h>
#include <Kernel/Data.h>
#include "AutoUpdate.h"
using namespace std;

enum class DownloadResult {
	Success, FailInit, FailDownload, FailDownloadMd5, FailCheckMd5
};

//Default MD5 File: filename + ".md5"
DownloadResult DownloadAndCheckMd5(const string& url, const string& localPath, bool isBinary, int *errorCode = nullptr, string *md5Rtn = nullptr)
{
	string domain, path;
	SplitHttpUrl(url, domain, path);

	//Init
	httplib::Client cli(domain.c_str());
	cli.set_connection_timeout(LXL_UPDATE_CHECK_TIMEOUT, 0);
	if (!cli.is_valid())
	{
		if (errorCode)
			*errorCode = -1;
		return DownloadResult::FailInit;
	}

	//Download
	auto response = cli.Get(path.c_str());
	if (response && response->status == 200)
	{
		if (filesystem::exists(localPath))
			filesystem::remove(localPath);

		ios_base::openmode mode = ios::out;
		if (isBinary)
			mode |= ios::binary;

		ofstream fout(localPath, mode);
		fout << response->body;
		fout.close();
	}
	else
	{
		if (errorCode)
			*errorCode = response->status;
		return DownloadResult::FailDownload;
	}

	//Download MD5
	string md5;
	response = cli.Get((path + ".md5").c_str());
	if (response && response->status == 200)
	{
		md5 = response->body;
		if (md5.back() == '\n')
			md5.pop_back();
		if (md5.back() == '\r')
			md5.pop_back();
	}
	else
	{
		if (errorCode)
			*errorCode = response->status;
		return DownloadResult::FailDownloadMd5;
	}

	if (md5Rtn)
		*md5Rtn = md5;

	//CheckMD5
	auto content = ReadAllFile(localPath, true);
	if (!content || Raw_toMD5(*content) != md5)
	{
		if (errorCode)
			*errorCode = -1;
		return DownloadResult::FailCheckMd5;
	}
	return DownloadResult::Success;
}

bool CheckAutoUpdate(bool isUpdateManually, bool forceUpdate)
{
	try
	{
		//Init
		httplib::Client cli(LXL_UPDATE_DOMAIN);
		cli.set_connection_timeout(LXL_UPDATE_CHECK_TIMEOUT, 0);

		//Get Json
		auto response = cli.Get(LXL_UPDATE_INFO_REMOTE);
		if (!response || response->status != 200)
		{
			if (isUpdateManually)
			{
				PRINT("LXL自动更新信息拉取失败！");
			}
			else
				DEBUG("LXL自动更新信息拉取失败！");
			return false;
		}

		//Parse
		string info{ response->body };
		JSON_ROOT data = JSON_ROOT::parse(info);

		//Check Other Files config
		if (!filesystem::exists(LXL_UPDATE_OTHER_FILES_RECORD))
		{
			ofstream fout(LXL_UPDATE_OTHER_FILES_RECORD);
			fout.flush();
			fout.close();
		}

		//Process Other Files
		auto iniVersions = Raw_IniOpen(LXL_UPDATE_OTHER_FILES_RECORD);
		for (auto& file : data["OtherFiles"])
		{
			string fileName = file["Name"].get<string>();
			string nowVersion = iniVersions->GetValue(fileName.c_str(), "Version", "0.0.0");
			string newVersion = file["Version"].get<string>();

			if (IsVersionLess(nowVersion, newVersion))
			{
				if (isUpdateManually)
				{
					PRINT("正在更新附属文件：" + fileName);
				}
				else
					DEBUG("正在更新附属文件：" + fileName);

				string url = file["URL"].get<string>();

				string remotePath = url + fileName;
				string localPath = file["Install"].get<string>() + "/" + fileName;

				bool isBinary = file["IsBinary"].get<bool>();

				//Download File
				int errorCode;
				string md5;
				switch (DownloadAndCheckMd5(remotePath, localPath, isBinary, &errorCode, &md5))
				{
				case DownloadResult::Success:
					iniVersions->SetValue(fileName.c_str(), "Version", newVersion.c_str());
					break;
				case DownloadResult::FailInit:
					if (isUpdateManually)
					{
						PRINT("附属文件更新 - 连接初始化失败！");
					}
					else
						DEBUG("附属文件更新 - 连接初始化失败！");
					break;
				case DownloadResult::FailDownload:
					if (isUpdateManually)
					{
						PRINT("附属文件更新 - 下载文件失败！错误码：" + to_string(errorCode));
					}
					else
						DEBUG("附属文件更新 - 下载文件失败！错误码：" + to_string(errorCode));
					break;
				case DownloadResult::FailDownloadMd5:
					if (isUpdateManually)
					{
						PRINT("附属文件更新 - 下载MD5校验文件失败！错误码：" + to_string(errorCode));
					}
					else
						DEBUG("附属文件更新 - 下载MD5校验文件失败！错误码：" + to_string(errorCode));
					break;
				case DownloadResult::FailCheckMd5:
					if (isUpdateManually)
					{
						PRINT("附属文件更新 - MD5校验失败！");
					}
					else
						DEBUG("附属文件更新 - MD5校验失败！");
					break;
				}
			}
		}
		iniVersions->SaveFile(LXL_UPDATE_OTHER_FILES_RECORD, true);
		Raw_IniClose(iniVersions);

		if (isUpdateManually)
		{
			PRINT("");
		}
		else
			DEBUG("");

		//Check BDS Version
		string bds = Raw_GetBDSVersion();
		if (bds.front() == 'v')
			bds.erase(0, 1);
		if (!data[bds].is_object())
		{
			INFO("提示：您的BDS版本不存在于当前主线维护版本中，自动更新将不会推送");
			INFO("如果有需要，请前往LXL相关页面手动更新加载器");
			if (isUpdateManually)
				return false;
			else
				_endthreadex(0);
		}
		else
		{
			//Get Version
			string verRemote = data[bds]["Version"].get<string>();
			auto versRemote = SplitStrWithPattern(verRemote, ".");
			if (versRemote.size() < 3)
			{
				if (isUpdateManually)
				{
					PRINT("版本信息解析失败！格式错误");
				}
				else
					DEBUG("版本信息解析失败！格式错误");
				return false;
			}
			
			//Compare Version
			int a = stoi(versRemote[0]);
			int b = stoi(versRemote[1]);
			int c = stoi(versRemote[2]);

			if(!forceUpdate && !IsVersionLess(LXL_VERSION_MAJOR, LXL_VERSION_MINOR, LXL_VERSION_REVISION, a,b,c))
			{
				if (isUpdateManually)
				{
					PRINT("当前LXL已是最新版本，无需更新。");
				}
				return true;
			}

			//Check existing update
			auto ini = Raw_IniOpen(LXL_UPDATE_INFO_RECORD);
			string existing = ini->GetValue("Info", "Version", "");
			if (!existing.empty())
			{
				auto versExisting = SplitStrWithPattern(existing, ".");
				if (a == stoi(versExisting[0]) && b == stoi(versExisting[1]) && c == stoi(versExisting[2]))
				{
					if (isUpdateManually)
					{
						PRINT("自动更新已下载完毕。请重启服务器升级到新版LXL");
					}
					return true;
				}
					
			}
			Raw_IniClose(ini);

			//Init for Download
			if (isUpdateManually)
			{
				PRINT("检测到新版本，启动更新");
			}
			else
				DEBUG("检测到新版本，启动更新");
			filesystem::remove_all(LXL_UPDATE_CACHE_PATH);
			filesystem::create_directories(LXL_UPDATE_CACHE_PATH);

			//Downloading
			auto iniUpdate = Raw_IniOpen(LXL_UPDATE_INFO_RECORD);
			for (auto& file : data[bds]["Files"])
			{
				string fileName = file["Name"].get<string>();
				if (isUpdateManually)
				{
					PRINT("正在下载更新：" + fileName);
				}
				else
					DEBUG("正在下载更新：" + fileName);

				string url = file["URL"].get<string>();

				string remotePath = url + fileName;
				string localPath = LXL_UPDATE_CACHE_PATH + fileName;
				bool isBinary = file["IsBinary"].get<bool>();

				//Download File
				int errorCode;
				string md5;
				switch (DownloadAndCheckMd5(remotePath,localPath,isBinary,&errorCode,&md5))
				{
				case DownloadResult::Success:
					break;
				case DownloadResult::FailInit:
					if (isUpdateManually)
					{
						PRINT("自动更新 - 连接初始化失败！");
					}
					else
						DEBUG("自动更新 - 连接初始化失败！");
					return false;
					break;
				case DownloadResult::FailDownload:
					if (isUpdateManually)
					{
						PRINT("自动更新 - 下载文件失败！错误码：" + to_string(errorCode));
					}
					else
						DEBUG("自动更新 - 下载文件失败！错误码：" + to_string(errorCode));
					return false;
					break;
				case DownloadResult::FailDownloadMd5:
					if (isUpdateManually)
					{
						PRINT("自动更新 - 下载MD5校验文件失败！错误码：" + to_string(errorCode));
					}
					else
						DEBUG("自动更新 - 下载MD5校验文件失败！错误码：" + to_string(errorCode));
					return false;
					break;
				case DownloadResult::FailCheckMd5:
					if (isUpdateManually)
					{
						PRINT("自动更新 - MD5校验失败！");
					}
					else
						DEBUG("自动更新 - MD5校验失败！");
					return false;
					break;
				}

				//Write to Update.ini
				string install = file["Install"].get<string>();
				iniUpdate->SetValue(fileName.c_str(), "Install", install.c_str());
				iniUpdate->SetValue(fileName.c_str(), "MD5", md5.c_str());
			}

			//Finish Download
			iniUpdate->SetValue("Info", "Version", verRemote.c_str());
			iniUpdate->SaveFile(LXL_UPDATE_INFO_RECORD);

			INFO("LXL自动更新文件下载完毕。新版本：v"+verRemote);
			INFO("在你下次重启服务器的时候，LXL将自动更新为最新版本");
		}
	}
	catch (JSON_ROOT::exception& e) {
		if (isUpdateManually)
		{
			PRINT(string("LXL版本更新信息解析失败！") + e.what());
		}
		else
			DEBUG(string("LXL版本更新信息解析失败！") + e.what());
	}
	catch (const seh_exception& e)
	{
		if (isUpdateManually)
		{
			PRINT(string("SEH Uncaught Exception Detected!\n") + e.what());
			PRINT("In Auto Update system");
		}
		else
		{
			DEBUG(string("SEH Uncaught Exception Detected!\n") + e.what());
			DEBUG("In Auto Update system");
		}
	}
	catch (...)
	{
		if (isUpdateManually)
		{
			PRINT(string("LXL自动更新发生错误！"));
		}
		else
			DEBUG(string("LXL自动更新发生错误！"));
	}
	return false;
}

void AddPreload()
{
	fstream fPreload(LXL_UPDATE_CHECK_PRELOAD, ios::out | ios::in | ios::app);
	fPreload.seekg(0, ios::beg);

	string path;
	while (getline(fPreload, path))
	{
		if (path.back() == '\n')
			path.pop_back();
		if (path.back() == '\r')
			path.pop_back();

		if (path == LXL_UPDATE_PROGRAM)
			return;
	}
	fPreload.clear();
	fPreload << "\n" << LXL_UPDATE_PROGRAM << endl;
}

void ClearUpdateLibrary()
{
	HMODULE hMod = GetModuleHandle(L"LXLAutoUpdate.dll");
	if (hMod != NULL)
	{
		FreeLibrary(hMod);
	}
}

void InitAutoUpdateCheck()
{
	//Check Files
	if (!filesystem::exists(LXL_UPDATE_PROGRAM))
	{
		WARN("LXL自动更新系统未启动！自动更新程序缺失");
		return;
	}
	if (!filesystem::exists(LXL_UPDATE_CACHE_PATH))
	{
		filesystem::create_directories(LXL_UPDATE_CACHE_PATH);
	}
	ClearUpdateLibrary();
	AddPreload();

	std::thread([]()
	{
		_set_se_translator(seh_exception::TranslateSEHtoCE);
		while (true)
		{
			try
			{
				std::this_thread::sleep_for(std::chrono::seconds(LXL_UPDATE_CHECK_INTERVAL));
				CheckAutoUpdate(false);
			}
			catch (const seh_exception& e)
			{
				DEBUG(string("SEH Uncaught Exception Detected!\n")+e.what());
				DEBUG("In Auto Update system");
			}
		}
	}).detach();
}