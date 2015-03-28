#ifndef CALLMANAGER_H
#define CALLMANAGER_H
#include "Call.h"
#include "UserAgent.h"
#include <string>
#include "resip/stack/SipMessage.hxx"
#include "resip/dum/ClientInviteSession.hxx"
#include "resip/dum/InviteSessionHandler.hxx"
#include "resip/dum/ServerInviteSession.hxx"
#include "resip/dum/SubscriptionHandler.hxx"
#include "resip/dum/ClientSubscription.hxx"
#include "RTPSessionFactory.h"
#include "MusicOnHoldServer.h"

class CallObserver;
typedef std::list<CallObserver*> CallObserverList;

class CallManager:  public resip::InviteSessionHandler, 
                    public resip::ClientSubscriptionHandler
{
public:
	CallManager(UserAgent *userAgent, RTPSessionFactory *rtpSessionFactory, MusicOnHoldServer *server);
	~CallManager();

    Call* call(const std::string& dest);
    void  answerIncomming(Call* call);
    void  transfer(Call *source, Call *dest);    
    void  blindTransfer(Call *source, std::string);
    void  reject(Call *source, int);

    // refer client subscription
    // These handlers should be used to track transfered call but we don't use it right now
    virtual void onUpdatePending (resip::ClientSubscriptionHandle, const resip::SipMessage &notify, bool outOfOrder){}
    virtual void onUpdateActive (resip::ClientSubscriptionHandle, const resip::SipMessage &notify, bool outOfOrder){}
    virtual void onUpdateExtension (resip::ClientSubscriptionHandle, const resip::SipMessage &notify, bool outOfOrder){}
    virtual int onRequestRetry (resip::ClientSubscriptionHandle, int retrySeconds, const resip::SipMessage &notify){}
    virtual void onTerminated (resip::ClientSubscriptionHandle, const resip::SipMessage *msg){}
    virtual void onNewSubscription (resip::ClientSubscriptionHandle, const resip::SipMessage &notify){}
    virtual void onReadyToSend (resip::ClientSubscriptionHandle, resip::SipMessage &msg){}
    virtual void onNotifyNotReceived (resip::ClientSubscriptionHandle){}
    virtual void onFlowTerminated (resip::ClientSubscriptionHandle){}

    // invite session handler
    virtual void onSuccess(resip::ClientRegistrationHandle h, const resip::SipMessage& response) {}
    virtual void onFailure(resip::ClientRegistrationHandle, const resip::SipMessage& msg) {}
    virtual void onMessage(resip::Handle<resip::InviteSession>, const resip::SipMessage& msg) {}
    virtual void onMessageSuccess(resip::Handle<resip::InviteSession>, const resip::SipMessage&) {}
    virtual void onMessageFailure(resip::Handle<resip::InviteSession>, const resip::SipMessage&) {}
    virtual void onFailure(resip::ClientInviteSessionHandle cis, const resip::SipMessage& msg);
    virtual void onForkDestroyed(resip::ClientInviteSessionHandle) {}
    virtual void onInfoSuccess(resip::InviteSessionHandle, const resip::SipMessage& msg) {}
    virtual void onInfoFailure(resip::InviteSessionHandle, const resip::SipMessage& msg) {}
    virtual void onProvisional(resip::ClientInviteSessionHandle cis, const resip::SipMessage& msg);
    virtual void onConnected(resip::ClientInviteSessionHandle, const resip::SipMessage& msg);
    virtual void onConnected(resip::InviteSessionHandle, const resip::SipMessage& msg);
    virtual void onStaleCallTimeout(resip::ClientInviteSessionHandle) {}
    virtual void onRedirected(resip::ClientInviteSessionHandle, const resip::SipMessage& msg) {}
    virtual void onAnswer(resip::InviteSessionHandle is, const resip::SipMessage& msg, const resip::SdpContents& sdp);
    virtual void onEarlyMedia(resip::ClientInviteSessionHandle, const resip::SipMessage& msg, const resip::SdpContents& sdp) {}
    virtual void onOfferRequired(resip::InviteSessionHandle, const resip::SipMessage& msg) {}
    virtual void onOfferRejected(resip::Handle<resip::InviteSession>, const resip::SipMessage *msg) {}
    virtual void onInfo(resip::InviteSessionHandle, const resip::SipMessage& msg);
    virtual void onRefer(resip::InviteSessionHandle, resip::ServerSubscriptionHandle, const resip::SipMessage& msg);
    virtual void onReferAccepted(resip::InviteSessionHandle, resip::ClientSubscriptionHandle, const resip::SipMessage& msg);
    virtual void onReferRejected(resip::InviteSessionHandle, const resip::SipMessage& msg);
    virtual void onReferNoSub(resip::Handle<resip::InviteSession>, const resip::SipMessage&);
    virtual void onRemoved(resip::ClientRegistrationHandle) {}
    virtual int onRequestRetry(resip::ClientRegistrationHandle, int retrySeconds, const resip::SipMessage& response) {}
    virtual void onNewSession(resip::ServerInviteSessionHandle sis, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg);
    virtual void onNewSession(resip::ClientInviteSessionHandle cis, resip::InviteSession::OfferAnswerType oat, const resip::SipMessage& msg);
    virtual void onTerminated(resip::InviteSessionHandle is, resip::InviteSessionHandler::TerminatedReason reason, const resip::SipMessage* msg);
    virtual void onOffer(resip::InviteSessionHandle is, const resip::SipMessage& msg, const resip::SdpContents& sdp);


    void addObserver(CallObserver *obs);
    void release(Call *call);
    void setRONATimeout(int rona);

private:
    UserAgent *mUserAgent;
    RTPSessionFactory *mRtpSessionFactory;
    MusicOnHoldServer *mMusicOnHoldServer;
    CallObserverList mCallObservers;
    time_t mLastTimeCheck;
    int mRONA; // Redirect On No Answer timeout
};

#endif

