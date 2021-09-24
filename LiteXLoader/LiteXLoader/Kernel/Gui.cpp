#include "pch.h"

#include "Global.h"
#include "Gui.h"
#include "Data.h"
#include "Packet.h"
using namespace std;

int Raw_SendSimpleForm(Player* player,const string &title,const string &content,const vector<string> &buttons, const std::vector<std::string>& images)
{
    string model = u8R"({"title":"%s","content":"%s","buttons":%s,"type":"form"})";
    model = model.replace(model.find("%s"),2, title);
    model = model.replace(model.find("%s"),2, content);

    JSON_ROOT buttonText;
    for (int i = 0; i < buttons.size(); ++i)
    {
        JSON_VALUE oneButton;
        oneButton["text"] = buttons[i];
        if (!images[i].empty())
        {
            JSON_VALUE image;
            image["type"] = images[i].find("textures/") == 0 ? "path" : "url";
            image["data"] = images[i];
            oneButton["image"] = image;
        }
        buttonText.push_back(oneButton);
    }
    model = model.replace(model.find("%s"),2, buttonText.dump());

    return Raw_SendFormPacket(player,model);
}

int Raw_SendModalForm(Player* player,const string &title,const string &content,const string &button1,const string &button2)
{
    string model = R"({"title":"%s","content":"%s","button1":"%s","button2":"%s","type":"modal"})";
    model = model.replace(model.find("%s"),2, title);
    model = model.replace(model.find("%s"),2, content);
    model = model.replace(model.find("%s"),2, button1);
    model = model.replace(model.find("%s"),2, button2);

    return Raw_SendFormPacket(player,model);
}

int Raw_SendCustomForm(Player* player,const std::string &data)
{
    return Raw_SendFormPacket(player,data);
}

int Raw_SendRawForm(Player* player, const std::string& data)
{
    return Raw_SendFormPacket(player, data);
}