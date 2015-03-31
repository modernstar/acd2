#include "Logging.h"
#include "UserAgent.h"
#include "CallManager.h"
#include "Acd.h"
#include "StaticConfiguration.h"
#include "DynamicConfiguration.h"
#include "MusicOnHoldServer.h"

#include "WebSocketController.h"

int main(int argc, char **argv)
{
    std::string staticConfigFile;
    std::string queueConfigFile;
    bool daemon = false;
    
    for (int i=0;i<argc;i++)
    {
        std::string param = argv[i];
        if (param=="-s" && argc>(i+1))
        {
            staticConfigFile = argv[i+1];
        } else if (param=="-q" && argc>(i+1))
        {
            queueConfigFile = argv[i+1];
        } else if (param=="-d"){
            daemon=true;
        }
    }

    if (staticConfigFile== "")
    {
        printf("Please provide a config file\r\n");
        exit(-1);
    }

    StaticConfiguration config(staticConfigFile);
    Logging::log("Starting ACD Server\r\n");
    if (daemon)
    {
        //TODO: should tell logging facility to stop output
        if (fork()>0) exit(0);
         
    }

    RTPSessionFactory rtpSessionFactory(atoi(config.getValue("LOWPORT").c_str()),atoi(config.getValue("HIGHPORT").c_str()));

    IControlServer *cs = new WebSocketController();

    AccountSettings account;
    MusicOnHoldServer moh(config.getValue("MOH"));
    std::string localIP=config.getValue("LISTENADDRESS");
    rtpSessionFactory.setLocalIP(localIP);

    account.mUserName = config.getValue("ACCOUNT_USERNAME");
    account.mPin = config.getValue("ACCOUNT_PASSWORD");
    account.mServer = config.getValue("ACCOUNT_PBX_IP");
    account.mDisplayName = config.getValue("ACCOUNT_DISPLAY_NAME");
    account.mPort = atoi(config.getValue("LISTENPORT").c_str());
    UserAgent ua(account,config.getValue("ACCOUNT_USERAGENT"));
    ua.registerAccount();

    CallManager callManager(&ua, &rtpSessionFactory, &moh);
    callManager.setRONATimeout(atoi(config.getValue("RONA").c_str()));
    Acd acd(&callManager);

    // Load queue config now that the engine is up
    DynamicConfiguration dconfig;
    dconfig.addObserver(&acd);
    dconfig.loadScript(queueConfigFile);

    char c = 'c';
    cs->setACD(&acd);
    cs->start();
    moh.start();
    while (1){
        ua.work();
        cs->work();
        printf("\r %c\r",c);
        if (c=='/') c='\\'; else c='/';
    }
    moh.stop();
    cs->stop();
    dconfig.removeObserver(&acd);

    delete cs;
    //TODO: Cleanup
}
