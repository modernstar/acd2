#include "Logging.h"
#include "UserAgent.h"
#include <resip/stack/Uri.hxx>
#include <resip/dum/ClientAuthManager.hxx>
#include <rutil/Log.hxx>
#include <stdio.h>
#include <sstream>
#include "Logging.h"

using namespace resip;
using namespace std;

UserAgent::UserAgent(AccountSettings settings, std::string userAgentName) : mProfile(new MasterProfile){
    this->mAccountSettings = settings;
    this->mUserAgentName = userAgentName;
    this->mRegistered = false;

    Log::setLevel(Log::None);
//  Log::setLevel(Log::Debug);

    // Create SipStack
    this->mSipStack = new SipStack();    
    // Add Transport
    this->mSipStack->addTransport(UDP,settings.mPort);

    // Create DUM
    this->mDum = new DialogUsageManager(*this->mSipStack);

    this->mRegistrationManager.setDum(this->mDum);
    this->mRegistrationManager.addObserver(this);

    // Create/Set MasterProfile

    std::stringstream sst;

    sst << "sip:";
    sst << settings.mUserName << "@" <<settings.mServer;

    NameAddr from(sst.str().c_str());
    from.displayName()=settings.mDisplayName.c_str();
    Uri outboundServer;
    outboundServer.host() = this->mAccountSettings.mServer.c_str();
    this->mProfile->setUserAgent(userAgentName.c_str());
    this->mProfile->setDigestCredential(this->mAccountSettings.mServer.c_str(),this->mAccountSettings.mUserName.c_str(),this->mAccountSettings.mPin.c_str());
    this->mProfile->setDefaultFrom(from);
    this->mProfile->addSupportedMimeType(resip::INFO, resip::Mime("application", "dtmf-relay"));
    this->mProfile->addSupportedMimeType(resip::NOTIFY, resip::Mime("application", "dialog-info+xml"));
    this->mProfile->addSupportedMimeType(resip::NOTIFY, resip::Mime("message", "sipfrag"));
    this->mProfile->addSupportedMimeType(resip::NOTIFY, resip::Mime("application", "simple-message-summary"));
    this->mProfile->addSupportedOptionTag(Token(Symbols::NoReferSub));
    this->mProfile->addSupportedMethod(resip::INFO);
    this->mProfile->addSupportedMethod(resip::NOTIFY);
    this->mDum->setMasterProfile(this->mProfile);

    // Set Managers
    auto_ptr<ClientAuthManager> clientAuth(new ClientAuthManager); 
    this->mDum->setClientAuthManager(clientAuth);   

    //Set Handlers
    this->mDum->setClientRegistrationHandler(&this->mRegistrationManager);

    auto_ptr<AppDialogSetFactory> factory(new CallFactory());
    this->mDum->setAppDialogSetFactory(factory);
    
}

UserAgent::~UserAgent(){
    this->mRegistrationManager.removeObserver(this);
    delete this->mDum;
    delete this->mSipStack;
}

void UserAgent::registerAccount()
{
    this->mRegistered = false;
    this->mRegistrationManager.registerAccount(this->mAccountSettings);
}

resip::DialogUsageManager* UserAgent::getDUM()
{
    return this->mDum;
}

const AccountSettings& UserAgent::getAccountSettings()
{
    return this->mAccountSettings;
}

resip::SipStack* UserAgent::getSipStack()
{
    return this->mSipStack;
}

void UserAgent::work()
{
    FdSet fdset;
    //TODO: use epoll
    this->mSipStack->buildFdSet(fdset);
    int err = fdset.selectMilliSeconds(resipMin((int)mSipStack->getTimeTillNextProcessMS(), 200));
    assert ( err != -1 );
    this->mSipStack->process(fdset);
    while(this->mDum->process());

}

bool UserAgent::isRegistered()
{
    return this->mRegistered;
}

void UserAgent::onRegistered(bool registered)
{
    this->mRegistered = registered;
    Logging::log("registered: %i\r\n",this->mRegistered);

}

std::string UserAgent::getName()
{
    return this->mUserAgentName;
}
