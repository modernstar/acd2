#include "Logging.h"
#include "Acd.h"

Acd::Acd(CallManager* callManager)
{
    mCallManager = callManager;

    this->mCallManager->addObserver(this);
}

Acd::~Acd()
{
    //TODO: remove ourself from observer list in callManager
}

void Acd::onAddQueue(const std::string& name)
{
    this->mQueues[name] = name;
}

void Acd::notifyQueueChanged()
{
    for (auto& it : mObservers)
    {
        it->onQueuesChanged();
    }
}

void Acd::onCallTerminated(Call *call)
{
    //TODO: It is possible that, after a transfer, the agent hangs up between the REFER and the INVITE. In that
    //      case, we could detect this with the final NOTIFY that the caller would send us. We should
    //      queue the call back and assign another agent to it. The current code would leave the caller
    //      in the limbo
    Logging::log("Removing call (state=%i)\r\n",call->getCallState());
    call->onTerminated();
    auto it = mCallList.find(call->getID());
    if (it != mCallList.end()) mCallList.erase(it);
    this->notifyQueueChanged();
}

void Acd::onCallAnswered(Call *call)
{
    Logging::log("Acd::onCallAnswered()\r\n");
    mCallManager->transfer(call->getAssociatedQueuedCall(),call);
}

void Acd::onCallFailed(Call *call)
{
    call->onAssignFailed();
    Logging::log("Call failed. Putting call back into queue\r\n");
}

void Acd::onRONA(Call *call)
{
    call->onAssignFailed();
    Logging::log("Call failed. Putting call back into queue\r\n");
}


void Acd::onIncommingCallAnswered(Call *call)
{
}

void Acd::onNewOutgoingCall(Call *call)
{
    this->notifyQueueChanged();
}

void Acd::onNewIncommingCall(const std::string& to, Call *call)
{
    Logging::log("Acd::onNewIcommingCall\r\n");
    if (mQueues.find(to) == mQueues.end())
    {
        Logging::log("Acd::onNewIcommingCall: Call rejected. Queue not found\r\n");
        mCallManager->reject(call,404);
    }
    call->setQueueName(to);
}

void Acd::onIncommingCallRinging(Call *call)
{
    Logging::log("Acd::onIcommingCallRinging Incomming call queued\r\n");
    mCallManager->answerIncomming(call);
    call->onCallQueued();
    mCallList[call->getID()] = call->getHandle();
    this->notifyQueueChanged();
}

void Acd::onDTMF(Call *call,std::string digit)
{
}

void Acd::getQueues(Dumais::JSON::JSON &queues)
{
    queues.addList("queues");

    int i = 0;
    for (auto it : mQueues)
    {
        queues["queues"].addObject();
        queues["queues"][i].addValue(it.first,"name"); 
        queues["queues"][i].addList("calls"); 
        int i2 = 0;
        for (auto& it1 : mCallList)
        {
            Call *call = dynamic_cast<Call*>(it1.second.get());
            std::string qn = call->getQueueName();
            if (qn != it.first) continue;

            queues["queues"][i]["calls"].addObject();
            queues["queues"][i]["calls"][i2].addValue(call->getCallState(),"state");
            queues["queues"][i]["calls"][i2].addValue(call->getID(),"callid");
            queues["queues"][i]["calls"][i2].addValue(call->getFrom().uri().user().data(),"from");
            i2++;
        }
        i++;
    }

}

void Acd::addObserver(IAcdObserver* obs)
{
    mObservers.push_back(obs);
}

void Acd::removeObserver(IAcdObserver* obs)
{
    auto it = std::find(this->mObservers.begin(), this->mObservers.end(), obs);
    if (it == this->mObservers.end()) return;

    this->mObservers.erase(it);
}

void Acd::pickup(const std::string& callid, const std::string& ext)
{
    Logging::log("Request to pickup call %s to ext %s\r\n",callid.c_str(), ext.c_str());

    if (mCallList.find(callid) == mCallList.end())
    {
        Logging::log("Call Not Found!\r\n");
        return;
    }
    Call* call = dynamic_cast<Call*>(this->mCallList[callid].get());
    if (call->getCallState() != Call::Queued) return;

    Call *call2 = this->mCallManager->call(ext);    
    if (call2)
    {
        call2->assignCall(call);
    }
}

