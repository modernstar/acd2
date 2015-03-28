#include "DynamicConfiguration.h"
#include "Logging.h"
#include <sstream>
#include <stdio.h>
#include <algorithm>

DynamicConfiguration::DynamicConfiguration()
{
}

DynamicConfiguration::~DynamicConfiguration(){
}

void DynamicConfiguration::addObserver(DynamicConfigurationObserver* obs)
{
   mObserverList.push_back(obs); 
}

void DynamicConfiguration::removeObserver(DynamicConfigurationObserver* obs)
{
    auto it = std::find(mObserverList.begin(), mObserverList.end(), obs);
    if (it == mObserverList.end()) return;

    mObserverList.erase(it);
}


void DynamicConfiguration::loadScript(std::string filename)
{
    char buf[255];
    FILE *f = fopen(filename.c_str(),"r");
    if (f)
    {
        while (fgets((char*)&buf,255,f))
        {
            std::string line = ((char*)&buf);
            this->processCommand(line);
        }
    }
}
void DynamicConfiguration::processCommand(std::string cmd)
{
    std::istringstream iss(cmd, std::istringstream::in);
    std::string word;
    std::string words[10];
    int i=0;
    while( iss >> word && i<10)     
    {
        words[i]=word;
        i++;
    }
    
    if (words[0]=="queue")
    {
        Logging::log("Creating queue '%s'\r\n",words[1].c_str());
        for (auto& obs : mObserverList)
        {
            obs->onAddQueue(words[1]);
        }
    }
    
}

