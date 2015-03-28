#pragma once

#include "RTPSession.h"
#include <pthread.h>
#include <list>

typedef std::list<RTPSession*> RTPSessionList;

class MusicOnHoldServer
{
private:
    pthread_mutex_t mQueueLock;
    pthread_t   mThreadHandle;
    RTPSessionList mRTPSessionList;
    volatile bool mRunning;

public:
    MusicOnHoldServer();
    ~MusicOnHoldServer();

    void addRTPSession(RTPSession *session);
    void removeRTPSession(RTPSession *session);
    void run();
    void start();
    void stop();

};

