#include "MusicOnHoldServer.h"
#include <algorithm>
#include <unistd.h>
#include "SoundFile.h"

void *threadStarter(void *p)
{
    MusicOnHoldServer *f = (MusicOnHoldServer*)p;
    f->run();
}

MusicOnHoldServer::MusicOnHoldServer()
{
    pthread_mutex_init(&this->mQueueLock,0);
}

MusicOnHoldServer::~MusicOnHoldServer()
{
    pthread_mutex_destroy(&this->mQueueLock);
}

void MusicOnHoldServer::addRTPSession(RTPSession *session)
{
    pthread_mutex_lock(&this->mQueueLock);
    this->mRTPSessionList.push_back(session); 
    pthread_mutex_unlock(&this->mQueueLock);
}

void MusicOnHoldServer::removeRTPSession(RTPSession *session)
{
    pthread_mutex_lock(&this->mQueueLock);
    auto it = std::find(this->mRTPSessionList.begin(), this->mRTPSessionList.end(), session); 
    if (it!=this->mRTPSessionList.end()) this->mRTPSessionList.erase(it); 
    pthread_mutex_unlock(&this->mQueueLock);
}

void MusicOnHoldServer::run()
{
    uint8_t *buf = new uint8_t[CONF_RTP_PAYLOAD_SIZE];
    SoundFile music(true);
    music.open("moh.wav");
    

    this->mRunning = true;
    while (this->mRunning)
    {
        music.getSample(CONF_RTP_PAYLOAD_SIZE, (char*)buf);

        pthread_mutex_lock(&this->mQueueLock);
        for (auto& it : this->mRTPSessionList)
        {
            RTPSession* rtp = it;
            rtp->play(buf);
        }
        pthread_mutex_unlock(&this->mQueueLock);
        usleep(20000);
    }

    delete buf;
}

void MusicOnHoldServer::start()
{
    pthread_create(&this->mThreadHandle, 0, threadStarter, (void*)this);
}

void MusicOnHoldServer::stop()
{
    this->mRunning = false;
    int ret = pthread_join(this->mThreadHandle,0);
}

