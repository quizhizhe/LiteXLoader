#include "pch.h"

#include "Utils.h"
#include <string>
#include <vector>
#include <ctime>
#include <optional>
using namespace std;

vector<string> SplitCmdLine(const string& paras)
{
    if (paras.empty())
        return vector<string>();

    vector<string> res;
    string now, strInQuote = "";
    istringstream strIn(paras);
    while (strIn >> now)
    {
        if (!strInQuote.empty())
        {
            strInQuote = strInQuote + " " + now;
            if (now.back() == '\"')
            {
                strInQuote.pop_back();
                res.push_back(strInQuote.erase(0, 1));
                strInQuote = "";
            }
        }
        else
        {
            if (now.front() == '\"')
            {
                if (now.back() == '\"')
                {
                    now = now.substr(1, now.size() - 2);
                    res.push_back(now);
                }
                else
                    strInQuote = now;
            }
            else
                res.push_back(now);
        }
    }
    if (!strInQuote.empty())
    {
        istringstream leftIn(strInQuote);
        while (leftIn >> now)
            res.push_back(now);
    }
    return res;
}

std::vector<std::string> SplitStrWithPattern(const std::string& str, const std::string& pattern)
{
    std::vector<std::string> resVec;

    if (str.empty())
        return resVec;
    
    std::string strs = str + pattern;

    size_t pos = strs.find(pattern);
    size_t size = strs.size();

    while (pos != std::string::npos)
    {
        std::string x = strs.substr(0, pos);
        resVec.push_back(x);
        strs = strs.substr(pos + 1, size);
        pos = strs.find(pattern);
    }

    return resVec;
}

bool StartsWith(const std::string& str, const std::string& start)
{
    int srclen = str.size();
    int startlen = start.size();
    if (srclen >= startlen)
    {
        string temp = str.substr(0, startlen);
        if (temp == start)
            return true;
    }

    return false;
}

bool EndsWith(const std::string& str, const std::string& end)
{
    int srclen = str.size();
    int endlen = end.size();
    if (srclen >= endlen)
    {
        string temp = str.substr(srclen - endlen, endlen);
        if (temp == end)
            return true;
    }

    return false;
}

void SplitHttpUrl(const std::string& url, string& host, string& path)
{
    host = url;

    bool foundProcotol = host.find('//') != string::npos;

    auto splitPos = host.find('/', foundProcotol ? host.find('//') + 2 : 0);    //查找协议后的第一个/分割host与路径
    if (splitPos == string::npos)
    {
        path = "/";
    }
    else
    {
        path = host.substr(splitPos);
        host = host.substr(0, splitPos);
    }
}

bool IsVersionLess(const std::string& v1, const std::string& v2)
{
    auto vers1 = SplitStrWithPattern(v1, ".");
    auto vers2 = SplitStrWithPattern(v2, ".");
    return IsVersionLess(stoi(vers1[0]), stoi(vers1[1]), stoi(vers1[2]), stoi(vers2[0]), stoi(vers2[1]), stoi(vers2[2]));
}

bool IsVersionLess(int v1a, int v1b, int v1c, int v2a, int v2b, int v2c)
{
    return (v1a < v2a || (v1a == v2a && v1b < v2b)
        || (v1a == v2a && v1b == v2b && v1c < v2c));
}

wchar_t* str2cwstr(string str)
{
    auto len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* buffer = new wchar_t[len + 1];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, len + 1);
    buffer[len] = L'\0';

    return buffer;
}

string wstr2str(wstring wstr)
{
    auto len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    char* buffer = new char[len + 1];
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer, len + 1, NULL, NULL);
    buffer[len] = '\0';

    string result = string(buffer);
    delete[] buffer;
    return result;
}

std::optional<std::string> ReadAllFile(const std::string& filePath, bool isBinary)
{
    std::ifstream fRead;

    std::ios_base::openmode mode = ios_base::in;
    if (isBinary)
        mode |= ios_base::binary;

    fRead.open(filePath, mode);
    if (!fRead.is_open())
    {
        return std::nullopt;
    }
    std::string data((std::istreambuf_iterator<char>(fRead)),
        std::istreambuf_iterator<char>());
    fRead.close();
    return data;
}

unsigned long long GetCurrentTimeStampMS()
{
    SYSTEMTIME currentTime = { 0 };
    GetLocalTime(&currentTime);

    struct tm currTm = {
        currentTime.wSecond,
        currentTime.wMinute,
        currentTime.wHour,
        currentTime.wDay,
        currentTime.wMonth - 1,
        currentTime.wYear - 1900
    };

    unsigned long long nNow = mktime(&currTm) * 1000 + currentTime.wMilliseconds;
    return nNow;
}


///////////// Hacker to get private FILE* /////////////

struct meta_auxiliary {
    using type1 = FILE * std::filebuf::*;
    friend type1 get(meta_auxiliary);
};
template<typename Tag, typename Tag::type1 M>
struct Helper_aux {
    friend typename Tag::type1 get(Tag) {
        return M;
    }
};
template struct Helper_aux<meta_auxiliary, &std::filebuf::_Myfile>;
FILE* hack(std::filebuf* buf) {
    return buf->*get(meta_auxiliary());
}


FILE* GetFILEfromFstream(std::fstream& fs)
{
    return hack(fs.rdbuf());
}

HANDLE GetHANDLEfromFstream(std::fstream& fs)
{
    auto cfile = ::_fileno(GetFILEfromFstream(fs));
    return (HANDLE)::_get_osfhandle(cfile);
}