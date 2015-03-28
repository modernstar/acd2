#include "WebSocketController.h"
#include "Logging.h"
#include <string.h>
#include "JSON.h"

using namespace Dumais::JSON;


WebSocketController::WebSocketController()
{
    mWebSocketServer = 0;
}

WebSocketController::~WebSocketController()
{
    if (this->mWebSocketServer)
    {
        delete mWebSocketServer;
    }

    if (this->mAcd)
    {
        this->mAcd->removeObserver(this);
    }
}

void WebSocketController::setACD(Acd *acd)
{
    this->mAcd = acd;
    this->mAcd->addObserver(this);
}

void WebSocketController::start()
{
    this->mWebSocketServer = new Dumais::WebSocket::WebSocketServer(8056,100, new Dumais::WebSocket::ConsoleLogger());
    this->mWebSocketServer->setWebSocketHandler(this);
}

void WebSocketController::onQueuesChanged()
{
    Logging::log("WebSocketController::onQueuesChanged\r\n");
    this->sendQueues();
}

void WebSocketController::work()
{
    this->mWebSocketServer->work(10);
}

void WebSocketController::stop()
{
}

void WebSocketController::sendQueues()
{
    JSON j;
    this->mAcd->getQueues(j);

    // this list is only used in one thread
    for (auto& it : this->mWebSockets)
    {
        it->sendText(j.stringify(false));
    }
}

void WebSocketController::pickup(const std::string& callid, const std::string& ext)
{
    this->mAcd->pickup(callid, ext);
}

bool WebSocketController::onWebSocketRequest(const std::string& request)
{
    Logging::log("WebSocket request on %s\r\n",request.c_str());
    if (request == "/") return true;
    return false;
}

void WebSocketController::onNewConnection(Dumais::WebSocket::WebSocket* ws)
{
    this->mWebSockets.push_back(ws);
}

void WebSocketController::onConnectionClosed(Dumais::WebSocket::WebSocket* ws)
{
    auto it = std::find(this->mWebSockets.begin(), this->mWebSockets.end(), ws);
    if (it == this->mWebSockets.end())
    {
        Logging::log("WebSocketController::onConnectionClosed: websocket not found\r\n");
        return;
    }
    
    this->mWebSockets.erase(it);
}

void WebSocketController::onMessage(Dumais::WebSocket::WebSocket* ws, Dumais::WebSocket::WebSocketMessage message)
{
    std::string str;
    str.assign((char*)message.buffer, message.size);
    JSON j;
    j.parse(str);
    if (j["request"].str() == "getqueues")
    {   
        this->sendQueues();
    }
    else if (j["request"].str() == "pickup")
    {
        this->pickup(j["callid"].str(),j["destination"].str());
    }
}

