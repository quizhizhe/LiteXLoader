#include "pch.h"

#include "i18n.h"
#include "Global.h"
#include <string>
using namespace std;

LangPack LangP;

void InitI18n(const string &langPack)
{
	LangP.load(langPack);
}