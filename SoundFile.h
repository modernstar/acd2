#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#include "ISound.h"
#include <string>

class SoundFile: public ISound{
private:
    FILE *mSoundFile;   
    std::string mFileName;
    bool mRepeat;
public:
	SoundFile(bool repeat=false);
	~SoundFile();

    bool open(std::string filename);
    int getSample(int size, char* buf);
    std::string toString();
};

#endif

