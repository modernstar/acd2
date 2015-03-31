#pragma once
#include "ISound.h"
#include <vector>

class PlayList: public ISound
{
private:
    bool mLoop;
    int mCurrentIndex;
    std::vector<ISound*> mList;
public:
    PlayList(bool loop);
    ~PlayList();

    void clear();
    void addSound(ISound* sound);
    virtual int getSample(int size, char* buf);
    virtual void restart();
    virtual std::string toString();
};

