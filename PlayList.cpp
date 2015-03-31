#include "PlayList.h"
#include <string.h>
#include "Logging.h"

PlayList::PlayList(bool loop)
{
    mLoop = loop;
    mCurrentIndex = 0;
}

PlayList::~PlayList()
{
    for (auto& it : mList)
    {
        delete it;
    }
}

void PlayList::addSound(ISound* sound)
{
    this->mList.push_back(sound);
}

void PlayList::restart()
{
    if (this->mList.size() == 0) return;
    this->mCurrentIndex = 0;
    this->mList[this->mCurrentIndex]->restart();
    
}

int PlayList::getSample(int length, char* buf)
{
    if (this->mList.size() == 0)
    {
        memset(buf,0,length);
        return length;
    }
    
    ISound* sound = mList[this->mCurrentIndex];
    
    while (length)
    {
        int n = sound->getSample(length, buf);
        if (n < length)
        {
            mCurrentIndex = (mCurrentIndex+1)%this->mList.size();
            sound = mList[this->mCurrentIndex];
            sound->restart();
            Logging::log("Changing file: (%i/%i) %s\r\n",n,length,sound->toString().c_str());
        }
        if (n >= 0)
        {
            length-=n;
            buf+=n;
        }
    }

    return length;
}

void PlayList::clear()
{
    this->mList.clear();
}

std::string PlayList::toString()
{
    std::string st;
    for (auto& it : mList)
    {
        if (it)
        {
            st += it->toString();
            st += ", ";
        }
    }
    return st;
}

