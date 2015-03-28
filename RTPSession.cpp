#include "Logging.h"
#include "RTPSession.h"


RTPSession::RTPSession()
{
    this->mSession=rtp_session_new(RTP_SESSION_SENDRECV);
    this->mLocalIP = "";
    this->mLocalPort=0;
}

RTPSession::~RTPSession()
{
    if (this->mSession!=0)
    {
        Logging::log("Stopping RTP session\r\n");
        rtp_session_destroy(this->mSession);
        this->mSession = 0;
    }
}

void RTPSession::activate()
{
    rtp_session_set_local_addr(mSession,this->mLocalIP.c_str(),this->mLocalPort,-1);
    rtp_session_set_scheduling_mode(mSession,TRUE);
    rtp_session_set_blocking_mode(mSession,0);
    rtp_session_set_connected_mode(mSession,TRUE);
    rtp_session_set_remote_addr(mSession,this->mPeerIP.c_str(),this->mPeerPort);
    rtp_session_set_payload_type(mSession,0);
}

void RTPSession::setPeerAddress(std::string ip, unsigned int peerPort)
{
    this->mPeerIP = ip;
    this->mPeerPort= peerPort;
}

void RTPSession::setLocalAddress(std::string ip, unsigned int port)
{
    this->mLocalPort = port;
    this->mLocalIP = ip;
}

unsigned int RTPSession::getLocalPort()
{
    return this->mLocalPort;
}

std::string RTPSession::getLocalIP()
{
    return this->mLocalIP;
}


void RTPSession::play(uint8_t *buffer)
{
    unsigned int user_ts = rtp_session_get_current_send_ts(this->mSession);
    rtp_session_send_with_ts(this->mSession,buffer,CONF_RTP_PAYLOAD_SIZE,user_ts);
}

/*void RTPSession::run()
{
    unsigned int user_ts;
    unsigned char buffer[CONF_RTP_PAYLOAD_SIZE];

    ISound *currentSound=0;
    bool queueWasEmpty=true;
    this->mInterruptCurrentSound=false;
    bool resetTime = false;
    while (!this->mAbort)
    {
        if (!currentSound)
        {
            pthread_mutex_lock(&this->mQueueLock);
            if (this->mSoundQueue.size())
            {
                currentSound = this->mSoundQueue.front();
                this->mSoundQueue.pop();
                pthread_mutex_unlock(&this->mQueueLock);
                Logging::log("Playing %s\r\n",currentSound->toString().c_str());
                queueWasEmpty=false;
                this->mInterruptCurrentSound=false;
            } else {
                pthread_mutex_unlock(&this->mQueueLock);
                if (!queueWasEmpty)
                {
                    this->notifyQueueEmpty();
                    queueWasEmpty=true;
                }
            }
        }


        int soundindex = 0;
        bool silenceSuppresion = true;
        if (this->mMusic)
        {
            this->mMusic->getSample(CONF_RTP_PAYLOAD_SIZE,(char*)&buffer);
            silenceSuppresion=false;
        }

        if (currentSound)   // if there is a sound to be played from queue, hide the music
        {
            silenceSuppresion= false;
            // overwrite music with sound
            soundindex=currentSound->getSample(CONF_RTP_PAYLOAD_SIZE,(char*)&buffer);
            //Note: if buffer wasn't filled completely, it contains music anyways.

            if (soundindex<CONF_RTP_PAYLOAD_SIZE || this->mInterruptCurrentSound)
            {
                delete currentSound;
                currentSound = 0;
            }
        }

        if (!silenceSuppresion)
        {
            if (resetTime)
            {
                user_ts = rtp_session_get_current_send_ts(this->mSession);
                resetTime = false;
            }
            rtp_session_send_with_ts(this->mSession,(uint8_t*)&buffer,CONF_RTP_PAYLOAD_SIZE,user_ts);
            user_ts+=CONF_RTP_PAYLOAD_SIZE;
        } else {
            // we started skipping frames, so we will need to resync next time
            resetTime = true;
        }

    }

}*/

