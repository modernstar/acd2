#ifndef CALL_H
#define CALL_H

#include <resip/dum/AppDialogSet.hxx>
//#include "RTPObserver.h"
#include "RTPSession.h"
#include "json/JSON.h"

class Agent;

class Call: public resip::AppDialogSet//, public IRTPSessionObserver
{
public:
    enum CallState
    {   
        Queued,
        Assigned,
        CallingAgent,
        AgentRinging,
        Terminated
    };

	Call(resip::DialogUsageManager &dum);
	~Call();

//    void addRTPObserver(RTPObserver *obs);

    void setRTPSession(RTPSession *rtpSession);

    RTPSession* getRTPSession();

    bool isIncomming();
    void setIncomming(bool i);

    void onRinging();
    void onCallQueued();
    void onAssigned();
    void assignCall(Call *call);
    void onAssignFailed();
    void onTerminated();
    std::string getQueueName();
    void setQueueName(const std::string& name);

    resip::DialogUsageManager& getDUM();
    CallState getCallState();

    resip::NameAddr getFrom();
    resip::NameAddr getContact();
    const char* getOwner();
    
    std::string getID();

    void toJSON(Dumais::JSON::JSON &json);
    Call *getAssociatedQueuedCall();

private:
    friend class CallManager;

    resip::InviteSessionHandle mInviteSessionHandle;

    Agent *mAgent;
    RTPSession *mRtpSession;
//    std::list<RTPObserver*> mRtpObservers;
    bool mIncomming;
    std::string mDigitQueue;
    std::string mQueueName;
    resip::NameAddr mFrom;
    resip::NameAddr mOwner;
    resip::NameAddr mTo;
    resip::NameAddr mContact;
    Call *mQueuedCall;  //TODO: should use handle instead?
    Call *mAssignedCall;
    CallState mCallState;
    time_t mRingingTimeStamp;
};

#endif

