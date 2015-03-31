#include "Logging.h"
#include "AccountSettings.h"

AccountSettings::AccountSettings(){
}

AccountSettings::AccountSettings(const AccountSettings& settings){
    if (&settings == this) return;

    this->mUserName = settings.mUserName;
    this->mPin = settings.mPin;
    this->mServer = settings.mServer;
    this->mDisplayName = settings.mDisplayName;    
    this->mPort = settings.mPort;
}


AccountSettings::~AccountSettings(){
}

