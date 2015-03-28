#ifndef DYNAMICCONFIGURATION_H
#define DYNAMICCONFIGURATION_H
#include <string>
#include <vector>
#include "DynamicConfigurationObserver.h"

class DynamicConfiguration{
private:
    std::vector<DynamicConfigurationObserver*> mObserverList;
public:
	DynamicConfiguration();
	~DynamicConfiguration();

    void addObserver(DynamicConfigurationObserver* obs);
    void removeObserver(DynamicConfigurationObserver* obs);

    void loadScript(std::string filename);
private:
    void processCommand(std::string cmd);
};

#endif

