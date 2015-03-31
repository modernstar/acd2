#pragma once

#include "RTPSession.h"
#include <pthread.h>
#include <list>
#include "PlayList.h"


typedef std::list<RTPSession*> RTPSessionList;

class MusicOnHoldServer
{
private:
    pthread_mutex_t mQueueLock;
    pthread_t   mThreadHandle;
    RTPSessionList mRTPSessionList;
    volatile bool mRunning;
    std::string mMOHFolder;
    PlayList *mPlayList;
    void updateMOHFiles();

public:
    MusicOnHoldServer(const std::string& folder);
    ~MusicOnHoldServer();

    void addRTPSession(RTPSession *session);
    void removeRTPSession(RTPSession *session);
    void run();
    void start();
    void stop();

};

