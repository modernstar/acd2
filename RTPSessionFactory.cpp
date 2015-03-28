#include "Logging.h"
#include "RTPSessionFactory.h"
#include "stdio.h"

RTPSessionFactory::RTPSessionFactory(unsigned int lport, unsigned int hport)
{
    this->mHighPort = hport;
    this->mLowPort = lport;
    ortp_init();
    ortp_scheduler_init();
    ortp_set_log_level_mask(ORTP_MESSAGE|ORTP_WARNING|ORTP_ERROR);
    this->mNextPort = this->mLowPort;

}


RTPSessionFactory::~RTPSessionFactory()
{
    ortp_exit();
}

RTPSession* RTPSessionFactory::createRTPSession()
{
    RTPSession *rtpSession = new RTPSession();
    rtpSession->setLocalAddress(this->mLocalIP,this->mNextPort);
    this->mNextPort+=2;
    if (this->mNextPort>=this->mHighPort)
    {
        this->mNextPort = this->mLowPort;
    }

    return rtpSession;
}

void RTPSessionFactory::setLocalIP(std::string localIP)
{
    this->mLocalIP = localIP;
}
