#include "Logging.h"
#include "CallObserver.h"
#include "CallManager.h"
#include <resip/stack/SdpContents.hxx>
#include <resip/dum/ServerInviteSession.hxx>
#include <resip/stack/ParserContainer.hxx>
#include <resip/stack/SipFrag.hxx>
#include "AppDialogSetEndCommand.h"
#include "AppDialogSetRONACommand.h"
#include <sstream>

using namespace resip;

CallManager::CallManager(UserAgent *userAgent, RTPSessionFactory *rtpSessionFactory, MusicOnHoldServer *server){
    this->mRtpSessionFactory = rtpSessionFactory;
    this->mMusicOnHoldServer = server;
    this->mUserAgent = userAgent;
    this->mUserAgent->getDUM()->setInviteSessionHandler(this);
    this->mLastTimeCheck =0;

    /*  When sending a refer, a subscription is automatically created (they call that an Implied Subscription)
        so we must specify a handler for such a case. The handler will only be used to receive
        NOTIFY messages for a refer
    */
    this->mUserAgent->getDUM()->addClientSubscriptionHandler("refer", this);

}

CallManager::~CallManager(){
}

void CallManager::release(Call *call)
{
    AppDialogSetEndCommand* cmd = new AppDialogSetEndCommand(call->getHandle());
    this->mUserAgent->getDUM()->post(cmd);
}

Call* CallManager::call(const std::string& to)
{
    if (!this->mUserAgent->isRegistered()) return 0;

    DialogUsageManager* dum = this->mUserAgent->getDUM();
    AccountSettings settings = this->mUserAgent->getAccountSettings();

    std::stringstream sst;
    sst << "sip:";
    sst << to << "@" <<settings.mDomain;
    NameAddr dest(sst.str().c_str());

    SharedPtr<UserProfile> up(dum->getMasterUserProfile());
    RTPSession *rtpSession = this->mRtpSessionFactory->createRTPSession();
    Call *call = new Call(*dum);
    call->setRTPSession(rtpSession);

    std::stringstream ss;
    ss<<"v=0\r\n"
        "o=- 0 0 IN IP4 "<< rtpSession->getLocalIP() <<"\r\n"
        "s="<<this->mUserAgent->getName()<<"\r\n"
        "c=IN IP4 "<< rtpSession->getLocalIP()<<"\r\n"
        "t=0 0\r\n"
        "m=audio "<< rtpSession->getLocalPort()<<" RTP/AVP 0 \r\n" // only support G.711 uLaw
        "a=rtpmap:0 pcmu/8000\r\n";
    Data txt(ss.str().c_str());
    HeaderFieldValue hfv(txt.data(), txt.size());
    Mime type("application", "sdp");
    SdpContents sdp(hfv, type);
    UInt64 currentTime = Timer::getTimeMicroSec();
    sdp.session().origin().getSessionId() = currentTime;
    sdp.session().origin().getVersion() = currentTime;

    SharedPtr<SipMessage> msg = dum->makeInviteSession(dest, up, &sdp, call);
    call->mTo = dest;

    call->mOwner = msg->header(h_From);
    dum->send(msg);

    return call;
}

void CallManager::blindTransfer(Call *source, std::string ext)
{
    Logging::log("Blind transferring to %s\r\n",ext.c_str());

    DialogUsageManager* dum = this->mUserAgent->getDUM();
    AccountSettings settings = this->mUserAgent->getAccountSettings();
    SharedPtr<UserProfile> up(dum->getMasterUserProfile());


    std::stringstream sst;
    sst << "sip:";
    sst << ext << "@" <<settings.mDomain;
    NameAddr to(sst.str().c_str());

    /*  refer with implicit subscription. If we specify "no implicit subscription" (rfc 4488),
        then the recipient will not send NOTIFYs to us.
    */
    source->mInviteSessionHandle.get()->refer(to);
    release(source); //TODO: should wait for notify before killing the call

}

void CallManager::transfer(Call *source, Call *dest)
{
    if (!source || !dest) return;

    Logging::log("Transferring \r\n"); 
   
    DialogUsageManager* dum = this->mUserAgent->getDUM();
    AccountSettings settings = this->mUserAgent->getAccountSettings();
    SharedPtr<UserProfile> up(dum->getMasterUserProfile());

    /*  refer with implicit subscription. If we specify "no implicit subscription" (rfc 4488),
        then the recipient will not send NOTIFYs to us.
    */
    source->mInviteSessionHandle.get()->refer(dest->mTo,dest->mInviteSessionHandle);
}

void CallManager::addObserver(CallObserver *obs)
{
    this->mCallObservers.push_back(obs);
}

void CallManager::onFailure(resip::ClientInviteSessionHandle cis, const resip::SipMessage& msg)
{
    Call *call = (Call*)cis->getAppDialogSet().get();
    if (call)
    {
        const char* to = call->getOwner();
        Logging::log("CallManager::onFailure for %s\r\n",to);
        for (CallObserverList::iterator it = this->mCallObservers.begin();it!=this->mCallObservers.end();it++)
        {
            CallObserver* obs = *it;
            obs->onCallFailed(call);
        }
    }
}


void CallManager::onProvisional(resip::ClientInviteSessionHandle cis, const resip::SipMessage& msg)
{
    Call *call = (Call*)cis->getAppDialogSet().get();

    switch (msg.header(h_StatusLine).statusCode())
    {
    case 100:
        
       // call->onTrying();
        break;
    case 180:
        {
            // This is the ringing event we get when we start transfering. If the destination doesn't
            // answer, we should redirect the call.
            if (call->getCallState()==Call::CallingAgent){
                Logging::log("Starting RONA timer\r\n");
                time(&call->mRingingTimeStamp);
                AppDialogSetRONACommand cmd(call->getHandle());
                this->mUserAgent->getDUM()->getSipStack().post(cmd,mRONA,this->mUserAgent->getDUM());
            }
            call->onRinging();
        }
        break;
    }

}

void CallManager::onConnected(resip::ClientInviteSessionHandle cis, const resip::SipMessage& msg)
{
    Call *call = (Call*)cis->getAppDialogSet().get();

    const char* to = call->getOwner();
    for (CallObserverList::iterator it = this->mCallObservers.begin();it!=this->mCallObservers.end();it++)
    {
        CallObserver* obs = *it;
        obs->onCallAnswered(call);
    }
}

void CallManager::onConnected(resip::InviteSessionHandle is, const resip::SipMessage& msg)
{
    Logging::log("CallManager::onConnected. Incoming call answered\r\n");

    Call *call = (Call*)is->getAppDialogSet().get();
    if (call)
    {
        RTPSession *rtpSession = call->getRTPSession();
        rtpSession->activate();
        this->mMusicOnHoldServer->addRTPSession(rtpSession);
        const char* to = call->getOwner();
        for (CallObserverList::iterator it = this->mCallObservers.begin();it!=this->mCallObservers.end();it++)
        {
            CallObserver* obs = *it;
            obs->onIncommingCallAnswered(call);        
        }
    }
}

void CallManager::onAnswer(resip::InviteSessionHandle is, const resip::SipMessage& msg, const resip::SdpContents& sdp)
{
}

void CallManager::onInfo(resip::InviteSessionHandle is, const resip::SipMessage& msg)
{
    /*Call *call = dynamic_cast<Call*>(is->getAppDialogSet().get());

    Mime dtmf(resip::Data("application"), resip::Data("dtmf-relay"));
    Contents* contents = msg.getContents();
    Mime mime = msg.getContents()->getType();
    if (mime.type() == "application" && mime.subType() == "dtmf-relay")
    {
        Data data = msg.getContents()->getBodyData();
        std::istringstream iss(data.c_str());
        std::string digit;
        std::getline(iss, digit,'=');
        iss >> digit;

        call->mDigitQueue += digit;
        const char* to = call->getOwner();
        for (CallObserverList::iterator it = this->mCallObservers.begin();it!=this->mCallObservers.end();it++)
        {
            std::string subject = it->second;
            CallObserver* obs = it->first;
            if (!strcmp(subject.c_str(),to)) obs->onDTMF(call,digit);
        }

        is->acceptNIT();
    }*/
}

void CallManager::onRefer(resip::InviteSessionHandle, resip::ServerSubscriptionHandle, const resip::SipMessage& msg)
{
    Logging::log("CallManager::onRefer\r\n");
}

void CallManager::onReferAccepted(resip::InviteSessionHandle is, resip::ClientSubscriptionHandle h, const resip::SipMessage& notify)
{
        Call *call = dynamic_cast<Call*>(is->getAppDialogSet().get());

        SipFrag* frag  = dynamic_cast<SipFrag*>(notify.getContents());
        unsigned int code = 0;
        if (frag)
        {
            code = frag->message().header(h_StatusLine).statusCode();
        }

        if (code>=100 && code<200){
            // don't do anything. It's fine.
        } else if (code>=200 && code<300){
            Logging::log("CallManager::onReferAccepted() with 2xx response\r\n");
            release(call);
        } else {
            Logging::log("WARNING: CallManager::onUpdateActive received code %i\r\n",code);
            release(call);
        }


}

void CallManager::onReferRejected(resip::InviteSessionHandle, const resip::SipMessage& msg)
{
    //TODO: should do something about it.
}

void CallManager::onReferNoSub(resip::Handle<resip::InviteSession>, const resip::SipMessage&)
{
    Logging::log("CallManager::onReferNoSub\r\n");
}

void CallManager::onNewSession(resip::ServerInviteSessionHandle sis, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg)
{
    
    Call *call = dynamic_cast<Call*>(sis->getAppDialogSet().get());
    if (call)
    {
        call->mInviteSessionHandle = sis->getSessionHandle();
        call->mFrom = msg.header(h_From);
        call->mOwner = msg.header(h_To);
        call->mTo = msg.header(h_To);
        ParserContainer<NameAddr> contacts = msg.header(h_Contacts);
        if(!contacts.empty())
        {
            call->mContact = contacts.front();
        }

        call->setIncomming(true);

        bool found=false;
        const char* to = call->getOwner();
        for (CallObserverList::iterator it = this->mCallObservers.begin();it!=this->mCallObservers.end();it++)
        {
            CallObserver* obs = *it;
            obs->onNewIncommingCall(to,call);
        }
    }


}

void CallManager::onNewSession(resip::ClientInviteSessionHandle cis, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg)
{
    Logging::log("CallManager::onNewSession (UAC)\r\n");
    Call *call = (Call*)cis->getAppDialogSet().get();
    call->mInviteSessionHandle = cis->getSessionHandle();

    for (CallObserverList::iterator it = this->mCallObservers.begin();it!=this->mCallObservers.end();it++)
    {
        CallObserver* obs = *it;
        obs->onNewOutgoingCall(call);
    }
}

void CallManager::onTerminated(resip::InviteSessionHandle is, resip::InviteSessionHandler::TerminatedReason reason, const resip::SipMessage* msg)
{
    Call *call = (Call*)is->getAppDialogSet().get();
    if (call)
    {
        RTPSession *rtp = call->getRTPSession();
        if (rtp)
        {
            this->mMusicOnHoldServer->removeRTPSession(rtp);
        }

        const char* to = call->getOwner();
        for (CallObserverList::iterator it = this->mCallObservers.begin();it!=this->mCallObservers.end();it++)
        {
            CallObserver* obs = *it;
            obs->onCallTerminated(call);
        }
    }

}


void CallManager::onOffer(resip::InviteSessionHandle is, const resip::SipMessage& msg, const resip::SdpContents& sdp)
{
    Call *call = dynamic_cast<Call*>(is->getAppDialogSet().get());
    if (call)
    {
        
        RTPSession *rtpSession = this->mRtpSessionFactory->createRTPSession();
        call->setRTPSession(rtpSession);

        const char* peerIP = sdp.session().origin().getAddress().c_str();
        unsigned int peerPort = sdp.session().media().front().port();
        rtpSession->setPeerAddress(peerIP, peerPort);
        const char* to = call->getOwner();
        for (CallObserverList::iterator it = this->mCallObservers.begin();it!=this->mCallObservers.end();it++)
        {
            CallObserver* obs = *it;
            obs->onIncommingCallRinging(call);
        }
    }
}

void CallManager::reject(Call* call, int code)
{
    ServerInviteSession *sis = dynamic_cast<ServerInviteSession*>(call->mInviteSessionHandle.get());
    if (sis)
    {
        sis->reject(code);
    }
}

void CallManager::answerIncomming(Call* call)
{
    DialogUsageManager* dum = this->mUserAgent->getDUM();
    AccountSettings settings = this->mUserAgent->getAccountSettings();
    RTPSession *rtpSession = call->getRTPSession();

    std::stringstream ss;
    ss<<"v=0\r\n"
        "o=- 0 0 IN IP4 "<< rtpSession->getLocalIP()<<"\r\n"
        "s="<<this->mUserAgent->getName()<<"\r\n"
        "c=IN IP4 "<< rtpSession->getLocalIP()<<"\r\n"
        "t=0 0\r\n"
        "m=audio "<< rtpSession->getLocalPort()<<" RTP/AVP 0 \r\n" // only support G.711 uLaw
        "a=rtpmap:0 pcmu/8000\r\n";
    Data txt(ss.str().c_str());
    HeaderFieldValue hfv(txt.data(), txt.size());
    Mime type("application", "sdp");
    SdpContents sdp2(hfv, type);
    UInt64 currentTime = Timer::getTimeMicroSec();
    sdp2.session().origin().getSessionId() = currentTime;
    sdp2.session().origin().getVersion() = currentTime;

    ServerInviteSession *sis = dynamic_cast<ServerInviteSession*>(call->mInviteSessionHandle.get());
    if (sis)
    {
        sis->provideAnswer(sdp2);
        Logging::log("Answering incomming call\r\n");
        sis->accept();
    }
}

void CallManager::setRONATimeout(int rona)
{
    mRONA = rona;
}
