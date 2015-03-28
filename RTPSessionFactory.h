#ifndef RTPSESSIONFACTORY_H
#define RTPSESSIONFACTORY_H

#include "RTPSession.h"


class RTPSessionFactory{
private:
    unsigned int mNextPort;
    unsigned int mLowPort;
    unsigned int mHighPort;
    std::string mLocalIP;
public:
	RTPSessionFactory(unsigned int lport, unsigned int hport);
	~RTPSessionFactory();

    RTPSession* createRTPSession();
    void setLocalIP(std::string localIP);
};

#endif

