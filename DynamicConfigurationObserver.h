#pragma once

class DynamicConfigurationObserver
{
public:
    virtual void onAddQueue(const std::string& name) = 0;
};
