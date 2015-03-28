#ifndef CALLOBSERVER_H
#define CALLOBSERVER_H

//#include "Call.h"
#include <string>
class Call;

class CallObserver
{
public:
    virtual void onCallTerminated(Call *call)=0;
    virtual void onCallAnswered(Call *call)=0;
    virtual void onCallFailed(Call *call)=0;
    virtual void onRONA(Call *call)=0;
    virtual void onIncommingCallAnswered(Call *call)=0;
    virtual void onNewIncommingCall(const std::string& to, Call *call)=0;
    virtual void onNewOutgoingCall(Call *call)=0;
    virtual void onIncommingCallRinging(Call *call)=0;
    virtual void onDTMF(Call *call,std::string digit)=0;

};


#endif

