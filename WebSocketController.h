#pragma once

#include "IControlServer.h"
#include "IWebSocketHandler.h"
#include "WebSocketServer.h"
#include "JSON.h"
#include <queue>

class WebSocketController: public IControlServer, public IAcdObserver, public Dumais::WebSocket::IWebSocketHandler
{
private:
    Dumais::WebSocket::WebSocketServer *mWebSocketServer;
    Acd* mAcd;
    std::list<Dumais::WebSocket::WebSocket*> mWebSockets;

public:
    WebSocketController();
    ~WebSocketController();

    virtual void start();
    virtual void stop();
    virtual void work();
    virtual void setACD(Acd* acd);

    virtual void onQueuesChanged();

    void sendQueues();
    void pickup(const std::string& callid,const std::string& ext);

    virtual bool onWebSocketRequest(const std::string& request);
    virtual void onNewConnection(Dumais::WebSocket::WebSocket* ws);
    virtual void onConnectionClosed(Dumais::WebSocket::WebSocket* ws);
    virtual void onMessage(Dumais::WebSocket::WebSocket* ws, Dumais::WebSocket::WebSocketMessage message);

};