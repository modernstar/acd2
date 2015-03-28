#include "Logging.h"
#include "StaticConfiguration.h"
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

std::string trim(std::string);

StaticConfiguration::StaticConfiguration(std::string fname)
{
    char buf[255];
    FILE *f = fopen(fname.c_str(),"r");
    if (f)
    {
        while (fgets((char*)&buf,255,f))
        {

            std::string line = ((char*)&buf);
            size_t pos = line.find(':');
            std::string key = trim(line.substr(0,pos));
            std::string value = trim(line.substr(pos+1));
            
            mValues[key]=value;


        }
        fclose(f);    
    }
}

StaticConfiguration::~StaticConfiguration(){
}

//TODO: we use atoi on top of this sometimes. Should make a method that returns int instead
std::string StaticConfiguration::getValue(std::string key)
{
    if (mValues.find(key)==mValues.end()) return "";

    return mValues[key];
}
