#ifndef STATICCONFIGURATION_H
#define STATICCONFIGURATION_H
#include <string>
#include <map>

class StaticConfiguration{
private:
    std::map<std::string,std::string> mValues;    
public:
	StaticConfiguration(std::string filename);
	~StaticConfiguration();

    std::string getValue(std::string key);
};

#endif

