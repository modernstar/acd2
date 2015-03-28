#pragma once
#include "Acd.h"

class IControlServer
{
public:
    virtual void start() = 0;
    virtual void work() = 0;
    virtual void stop() = 0;
    virtual void setACD(Acd* acd) = 0;
};
