#ifndef USERAGENT_H
#define USERAGENT_H

#include <resip/dum/DialogUsageManager.hxx>
#include <resip/stack/SipStack.hxx>
#include <resip/dum/MasterProfile.hxx>
#include "RegistrationManager.h"
#include "AccountSettings.h"
#include "RegistrationObserver.h"
#include "CallFactory.h"

class UserAgent: public RegistrationObserver{
private:

public:
	UserAgent(AccountSettings settings, std::string userAgentName);
	~UserAgent();

    void registerAccount();

    resip::DialogUsageManager* getDUM();
    resip::SipStack* getSipStack();
    const AccountSettings& getAccountSettings();
    bool  isRegistered();
    void work();
    std::string getName();

    virtual void onRegistered(bool registered);

private:
    AccountSettings mAccountSettings;
    RegistrationManager mRegistrationManager;
    resip::DialogUsageManager* mDum;
    resip::SipStack* mSipStack;
    resip::SharedPtr<resip::MasterProfile> mProfile;

    std::string mUserAgentName;
    bool mRegistered;


};

#endif

