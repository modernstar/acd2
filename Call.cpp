#include "Logging.h"
#include "Call.h"
#include <stdio.h>

#include "resip/dum/InviteSession.hxx"
#include "resip/dum/ClientInviteSession.hxx"
//#include "Agent.h"

using namespace resip;
using namespace Dumais::JSON;

Call::Call(resip::DialogUsageManager &dum):AppDialogSet(dum)
{
    this->mRtpSession = 0;
    this->mIncomming = false;
    this->mQueuedCall = 0;
    this->mAssignedCall = 0;
}

Call::~Call()
{
    if (this->mRtpSession)
    {
        //TODO: should make sure the MOH Server does not hold a ref to the RtpSession
        delete this->mRtpSession;
        this->mRtpSession = 0;
    }

    if (mQueuedCall)
    {
        mQueuedCall->mAssignedCall = 0;
    }
    if (mAssignedCall)
    {
        mAssignedCall->mQueuedCall = 0;
    }

}

void Call::setIncomming(bool i)
{
    this->mIncomming = i;
}

void Call::setRTPSession(RTPSession *rtpSession)
{
    this->mRtpSession = rtpSession;
}

RTPSession* Call::getRTPSession()
{
    return this->mRtpSession;
}

DialogUsageManager& Call::getDUM()
{
    return this->mDum;
}

bool Call::isIncomming()
{
    return this->mIncomming;
}

resip::NameAddr Call::getFrom()
{
    return mFrom;
}

resip::NameAddr Call::getContact()
{
    return mContact;
}

const char* Call::getOwner()
{
    return mOwner.uri().user().data();

}

Call* Call::getAssociatedQueuedCall()
{
    return mQueuedCall;
}


std::string Call::getID()
{
    return this->getDialogSetId().getCallId().c_str();
}

void Call::onCallQueued()
{
    mCallState = Queued;
}

void Call::onAssigned()
{
    mCallState = Assigned;
}

void Call::assignCall(Call *call)
{
    Logging::log("Call Assigned\r\n");
    call->onAssigned();
    mQueuedCall = call;
    call->mAssignedCall = this;
    mCallState = CallingAgent;
}

void Call::onRinging()
{
    if (mCallState == CallingAgent)
    {
        mCallState = AgentRinging;
    }
}

void Call::onAssignFailed()
{
    if (mCallState==CallingAgent || mCallState==AgentRinging)
    {
        mQueuedCall->onAssignFailed();
        
    } else if (mCallState == Assigned)
    {
        mCallState = Queued;
    }
}

void Call::onTerminated()
{
    // If the queued call terminates while it is assigned, cancel the second call.
    if (this->mCallState==Call::Assigned) // Assigned queued call
    {
        if (mAssignedCall)
        {
            mAssignedCall->end();
        }
    }

    mCallState = Terminated;
}

Call::CallState Call::getCallState()
{
    return this->mCallState;
}

void Call::toJSON(Dumais::JSON::JSON &json)
{
    JSON j = json.addObject();
    j.addValue(getID(),"id");

    std::string s;
    switch (mCallState)
    {
        case Queued:
            s= "queued";
            break;
        case Assigned:
            s= "assigned";
            break;
        case CallingAgent:
            s= "callingagent";
            break;
        case AgentRinging:
            s= "agent ringing";
            break;
        case Terminated:
            s= "terminated";
            break;
    }
    j.addValue(s,"state");
    j.addValue(mFrom.uri().getAor().data(),"from");
    j.addValue( mTo.uri().getAor().data(),"to");
}


std::string Call::getQueueName()
{
    return this->mQueueName;
}

void Call::setQueueName(const std::string& name)
{
    this->mQueueName = name;
}

