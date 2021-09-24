#include "pch.h"

#include "APIHelp.h"
#include <Kernel/ThirdParty.h>
#include <Kernel/System.h>
#include <Engine/TimeTaskSystem.h>
#include "NetworkAPI.h"
#include <string>
#include <vector>
using namespace std;
using namespace script;

//////////////////// Classes ////////////////////

ClassDefine<void> NetworkClassBuilder =
    defineClass("network")
        .function("httpGet", &NetworkClass::httpGet)
        .function("httpPost", &NetworkClass::httpPost)
        .function("httpGetSync", &NetworkClass::httpGetSync)

        //For compatibility
        .function("newWebSocket", &NetworkClass::newWebSocket)
        .build();

ClassDefine<WSClientClass> WSClientClassBuilder =
    defineClass<WSClientClass>("WSClient")
        .constructor(&WSClientClass::constructor)
        .instanceProperty("status", &WSClientClass::getStatus)
        .instanceFunction("connect", &WSClientClass::connect)
        .instanceFunction("send", &WSClientClass::send)
        .instanceFunction("listen", &WSClientClass::listen)
        .instanceFunction("close", &WSClientClass::close)
        .instanceFunction("shutdown", &WSClientClass::shutdown)
        .instanceFunction("errorCode", &WSClientClass::errorCode)

        .property("Open", [] { return Number::newNumber((int)WebSocketClient::Status::Open); })
        .property("Closing", [] { return Number::newNumber((int)WebSocketClient::Status::Closing); })
        .property("Closed", [] { return Number::newNumber((int)WebSocketClient::Status::Closed); })
        .build();


//生成函数
WSClientClass::WSClientClass()
    :ScriptClass(ScriptClass::ConstructFromCpp<WSClientClass>{})
{
    ws.OnTextReceived([ nowList {&listeners[int(WSClientEvents::onTextReceived)]} ]
        (WebSocketClient& client, string msg)
    {
        if (!nowList->empty())
            for (auto& listener : *nowList)
            {
                EngineScope enter(listener.engine);
                NewTimeout(listener.func.get(), { String::newString(msg) }, 1);
            }
    });

    ws.OnBinaryReceived([nowList{ &listeners[int(WSClientEvents::onBinaryReceived)] }]
        (WebSocketClient& client, vector<uint8_t> data)
    {
        if(!nowList->empty())
            for (auto& listener : *nowList)
            {
                EngineScope enter(listener.engine);
                NewTimeout(listener.func.get(), { ByteBuffer::newByteBuffer(data.data(),data.size()) }, 1);
            }
    });

    ws.OnError([nowList{ &listeners[int(WSClientEvents::onError)] }]
        (WebSocketClient& client, string msg)
    {
        if (!nowList->empty())
            for (auto& listener : *nowList)
            {
                EngineScope enter(listener.engine);
                NewTimeout(listener.func.get(), { String::newString(msg) }, 1);
            }
    });

    ws.OnLostConnection([nowList{ &listeners[int(WSClientEvents::onLostConnection)] }]
        (WebSocketClient& client, int code)
    {
        if (!nowList->empty())
            for (auto& listener : *nowList)
            {
                EngineScope enter(listener.engine);
                NewTimeout(listener.func.get(), { Number::newNumber(code) }, 1);
            }
    });
}

WSClientClass* WSClientClass::constructor(const Arguments& args)
{
    return new WSClientClass;
}

//成员函数
void WSClientClass::addListener(const string& event, Local<Function> func)
{
    if (event == "onTextReceived")
        listeners[(int)WSClientEvents::onTextReceived].push_back({ EngineScope::currentEngine(), Global<Function>(func) });
    else if (event == "onBinaryReceived")
        listeners[(int)WSClientEvents::onBinaryReceived].push_back({ EngineScope::currentEngine(), Global<Function>(func) });
    else if (event == "onError")
        listeners[(int)WSClientEvents::onError].push_back({ EngineScope::currentEngine(), Global<Function>(func) });
    else if (event == "onLostConnection")
        listeners[(int)WSClientEvents::onLostConnection].push_back({ EngineScope::currentEngine(), Global<Function>(func) });
    else
    {
        ERROR("WSClient Event \"" + event + "\" No Found!\n");
    }  
}

Local<Value> WSClientClass::getStatus()
{
    try {
        return Number::newNumber((int)ws.GetStatus());
    }
    catch (const std::runtime_error& e)
    {
        return Local<Value>();
    }
    CATCH("Fail in connect!");
}

Local<Value> WSClientClass::connect(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1);
    CHECK_ARG_TYPE(args[0], ValueKind::kString);

    try {
        ws.Connect(args[0].toStr());
        return Boolean::newBoolean(true);
    }
    catch (const std::runtime_error& e)
    {
        return Boolean::newBoolean(false);
    }
    CATCH("Fail in connect!");
}

Local<Value> WSClientClass::send(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1);

    try {
        if (args[0].isString())
            ws.SendText(args[0].toStr());
        else if (args[0].isByteBuffer())
            ws.SendBinary((char*)args[0].asByteBuffer().getRawBytes(), args[0].asByteBuffer().byteLength());
        else
        {
            ERROR("Wrong type of argument in WSClientSend!");
            return Local<Value>();
        }
        return Boolean::newBoolean(true);
    }
    catch (const std::runtime_error& e)
    {
        return Boolean::newBoolean(false);
    }
    CATCH("Fail in send!");
}

Local<Value> WSClientClass::listen(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 2);
    CHECK_ARG_TYPE(args[0], ValueKind::kString);
    CHECK_ARG_TYPE(args[1], ValueKind::kFunction);

    try {
        addListener(args[0].toStr(), args[1].asFunction());
        return Boolean::newBoolean(true);
    }
    catch (const std::runtime_error& e)
    {
        return Boolean::newBoolean(false);
    }
    CATCH("Fail in listen!");
}

Local<Value> WSClientClass::close(const Arguments& args)
{
    try {
        ws.Close();
        return Boolean::newBoolean(true);
    }
    catch (const std::runtime_error& e)
    {
        return Boolean::newBoolean(false);
    }
    CATCH("Fail in close!");
}

Local<Value> WSClientClass::shutdown(const Arguments& args)
{
    try {
        ws.Shutdown();
        return Boolean::newBoolean(true);
    }
    catch (const std::runtime_error& e)
    {
        return Boolean::newBoolean(false);
    }
    CATCH("Fail in shutdown!");
}

Local<Value> WSClientClass::errorCode(const Arguments& args)
{
    try {
        return Number::newNumber(WSAGetLastError());
    }
    catch (const std::runtime_error& e)
    {
        return Local<Value>();
    }
    CATCH("Fail in errorCode!");
}


//////////////////// APIs ////////////////////

Local<Value> NetworkClass::httpGet(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 2);
    CHECK_ARG_TYPE(args[0], ValueKind::kString);
    CHECK_ARG_TYPE(args[1], ValueKind::kFunction);

    try
    {
        Global<Function> callbackFunc{ args[1].asFunction() };

        return Boolean::newBoolean(Raw_HttpGet(args[0].toStr(),
            [callback{ std::move(callbackFunc) }, engine{ EngineScope::currentEngine() }]
            (int status, string body)
        {
            EngineScope scope(engine);
            try
            {
                NewTimeout(callback.get(), { Number::newNumber(status), String::newString(body) }, 1);
            }
            catch (const Exception& e)
            {
                ERROR("HttpGet Callback Failed!");
                ERRPRINT("[Error] In Plugin: " + ENGINE_OWN_DATA()->pluginName);
                ERRPRINT(e);
            }
        }));
    }
    CATCH("Fail in HttpGet");
}

Local<Value> NetworkClass::httpPost(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 4);
    CHECK_ARG_TYPE(args[0], ValueKind::kString);
    CHECK_ARG_TYPE(args[1], ValueKind::kString);
    CHECK_ARG_TYPE(args[2], ValueKind::kString);
    CHECK_ARG_TYPE(args[3], ValueKind::kFunction);

    try
    {
        Global<Function> callbackFunc{ args[3].asFunction() };

        return Boolean::newBoolean(Raw_HttpPost(args[0].toStr(), args[1].toStr(), args[2].toStr(),
            [callback{ std::move(callbackFunc) }, engine{ EngineScope::currentEngine() }]
            (int status, string data)
        {
            EngineScope scope(engine);
            try
            {
                NewTimeout(callback.get(), { Number::newNumber(status), String::newString(data) }, 1);
            }
            catch (const Exception& e)
            {
                ERROR("HttpPost Callback Failed!");
                ERRPRINT("[Error] In Plugin: " + ENGINE_OWN_DATA()->pluginName);
                ERRPRINT(e);
            }
        }));
    }
    CATCH("Fail in HttpPost");
}

Local<Value> NetworkClass::httpGetSync(const Arguments& args)
{
    CHECK_ARGS_COUNT(args, 1);
    CHECK_ARG_TYPE(args[0], ValueKind::kString);

    try {
        int status;
        string result;
        Raw_HttpGetSync(args[0].toStr(), &status, &result);
        Local<Object> res = Object::newObject();
        res.set("status", status);
        res.set("data", result);
        return res;
    }
    CATCH("Fail in HttpGetSync");
}


//For compatibility
Local<Value> NetworkClass::newWebSocket(const Arguments& args)
{
    auto newp = new WSClientClass();
    return newp->getScriptObject();
}

Local<Object> WSClientClass::newWSClient()
{
    auto newp = new WSClientClass();
    return newp->getScriptObject();
}