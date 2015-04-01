#ifndef ACCOUNTSETTINGS_H
#define ACCOUNTSETTINGS_H

#include <string>

class AccountSettings{
private:

public:
	AccountSettings();
    AccountSettings(const AccountSettings& settings);
	~AccountSettings();

    std::string mUserName;
    std::string mPin;
    std::string mServer; //outbound server or proxy
    std::string mDomain;
    std::string mDisplayName;
    unsigned int mPort;

};

#endif

