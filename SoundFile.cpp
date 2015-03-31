#include "Logging.h"
#include "SoundFile.h"
#include "Logging.h"

#define ENCODING_MICROSOFT_ULAW 0x07 //http://www.sno.phy.queensu.ca/~phil/exiftool/TagNames/RIFF.html

struct riffHeader
{
    uint32_t chunckID;
    uint32_t chunkSize;
    uint32_t format;
    uint32_t subchunkID;
    uint32_t subChuckSize;
    uint16_t audioFormat;
    uint16_t channels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;    
    uint32_t subchunk2ID;
    uint32_t subChuck2size;

};

SoundFile::SoundFile(bool repeat)
{
    this->mRepeat = repeat;
    this->mSoundFile = 0;
}

SoundFile::~SoundFile()
{
    if (this->mSoundFile) fclose(this->mSoundFile);
}

bool SoundFile::open(std::string filename)
{
    this->mSoundFile = fopen(filename.c_str(),"r");
    if (!this->mSoundFile) return false;

    riffHeader header;
    fread((char*)&header,1,44,this->mSoundFile);
    uint16_t format = header.audioFormat;
    uint16_t channels = header.channels;
    int sampleRate = header.sampleRate;
    uint16_t bits = header.bitsPerSample;

    bool supported = false;
    if (format == ENCODING_MICROSOFT_ULAW && channels==1 && bits==8 && sampleRate==8000)
    {
        supported = true;
    }

    if (!supported)
    {
        Logging::log("The file format specified is not supported. We only support u-law encoded RIFF files (8khz, mono, 8bit)\r\n");
        Logging::log("You could encode it like this: sox -V %s -r 8000 -c 1 -t wav -e u-law %s-ulaw.wav\r\n",filename.c_str(),filename.c_str());
        fclose(this->mSoundFile);
        this->mSoundFile = 0;
    }

    this->mFileName = filename;
    return (this->mSoundFile!=0);
}

int SoundFile::getSample(int size, char* buf)
{
    if (!this->mSoundFile) return 0;

    int n = fread(buf,1,size,this->mSoundFile);
       
    if (n<size)
    {
        if (this->mRepeat)
        {
            rewind(this->mSoundFile);
        }
    }
    return n;

}

void SoundFile::restart()
{
    if (!this->mSoundFile) return;
    rewind(this->mSoundFile);
}

std::string SoundFile::toString()
{
    return this->mFileName;
}

