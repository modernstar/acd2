#ifndef WAKEUPRTPSESSION_H
#define WAKEUPRTPSESSION_H

#define CONF_RTP_PAYLOAD_SIZE 160
#include <ortp/ortp.h>
#include <string>
#include <list>

class RTPSession{
private:
    RtpSession *mSession;
    std::string mPeerIP;
    unsigned int mPeerPort;
    std::string mLocalIP;
    unsigned int mLocalPort;

public:
	RTPSession();
	~RTPSession();

    void setPeerAddress(std::string ip, unsigned int peerPort);
    void setLocalAddress(std::string ip, unsigned int port);    
    void activate();
    unsigned int getLocalPort();
    std::string getLocalIP();

    void play(uint8_t *buffer);

};

#endif

