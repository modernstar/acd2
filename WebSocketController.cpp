#include "WebSocketController.h"
#include "Logging.h"
#include <string.h>
#include "json/JSON.h"

using namespace Dumais::JSON;


WebSocketController::WebSocketController()
{
    Dumais::Utils::Logging::logger = new Dumais::Utils::ConsoleLogger();
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
    this->mWebSocketServer = new Dumais::WebSocket::WebSocketServer(8056,100);
    this->mWebSocketServer->setOnWebSocketRequest([this](const std::string& request,
        std::map<std::string,std::string> protocols,
        std::string& chosenProtocol)
    {
        Logging::log("WebSocket request on %s\r\n",request.c_str());
        if (request == "/") return true;
        return false;
    });

    this->mWebSocketServer->setOnNewConnection(std::bind(&WebSocketController::onNewConnection,this,std::placeholders::_1));
    this->mWebSocketServer->setOnConnectionClosed(std::bind(&WebSocketController::onConnectionClosed,this,std::placeholders::_1));
    this->mWebSocketServer->setOnMessage(std::bind(&WebSocketController::onMessage,this,std::placeholders::_1,std::placeholders::_2));


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
    Logging::log("WebSocketController::sendQueuesr\n");
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
    Logging::log("WebSocketController::onMessage: %s\r\n",j["request"].str().c_str());
    if (j["request"].str() == "getqueues")
    {   
        this->sendQueues();
    }
    else if (j["request"].str() == "pickup")
    {
        this->pickup(j["callid"].str(),j["destination"].str());
    }
}

