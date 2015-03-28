#pragma once

class IAcdObserver
{
public:
    virtual void onQueuesChanged() = 0;
};
