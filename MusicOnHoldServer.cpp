#include "MusicOnHoldServer.h"
#include <algorithm>
#include <unistd.h>
#include "SoundFile.h"
#include <sys/types.h>
#include <dirent.h>
#include "Logging.h"

void *threadStarter(void *p)
{
    MusicOnHoldServer *f = (MusicOnHoldServer*)p;
    f->run();
}

MusicOnHoldServer::MusicOnHoldServer(const std::string& folder)
{
    this->mMOHFolder = folder;
    mPlayList = new PlayList(true);
    pthread_mutex_init(&this->mQueueLock,0);
    this->updateMOHFiles();
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

    this->mRunning = true;
    while (this->mRunning)
    {
        this->mPlayList->getSample(CONF_RTP_PAYLOAD_SIZE, (char*)buf);

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

void MusicOnHoldServer::updateMOHFiles()
{
    this->mPlayList->clear();
    DIR *dir = opendir(this->mMOHFolder.c_str());
    if(!dir) return;

    dirent *de;
    while (de = readdir(dir))
    {
        std::string fname = de->d_name;
        if (fname == "." || fname == "..") continue;

        SoundFile *sf = new SoundFile(false);
        if (sf->open(this->mMOHFolder+"/"+fname))
        {
            Logging::log("MOH: %s\r\n",fname.c_str());
            this->mPlayList->addSound(sf);
        }
        else
        {
            delete sf;
        }
    }
    closedir(dir);
}
