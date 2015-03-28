#pragma once

#include <string>
#include "CallManager.h"
#include "CallObserver.h"
#include <list>
#include <resip/dum/AppDialogSet.hxx>
#include "DynamicConfigurationObserver.h"
#include "JSON.h"
#include "IAcdObserver.h"

class Acd: public CallObserver, public DynamicConfigurationObserver{
private:
    std::map<std::string,std::string> mQueues;
    void notifyQueueChanged();

protected:
    CallManager *mCallManager;
    std::map<std::string,resip::AppDialogSetHandle> mCallList;
    std::list<IAcdObserver*> mObservers;
//    Call *getFirstCall();    

public:
	Acd(CallManager *callManager);
	~Acd();

    virtual void onAddQueue(const std::string& name);

    void addObserver(IAcdObserver* obs);
    void removeObserver(IAcdObserver* obs);

    void onCallTerminated(Call *call);
    void onCallAnswered(Call *call);
    void onCallFailed(Call *call);
    void onRONA(Call *call);
    void onIncommingCallAnswered(Call *call);
    void onNewIncommingCall(const std::string& to, Call *call);
    void onNewOutgoingCall(Call *call);
    void onIncommingCallRinging(Call *call);
    void onDTMF(Call *call,std::string digit);

    void getQueues(Dumais::JSON::JSON &queues);
    void pickup(const std::string& callid, const std::string& ext);

};


