#include "Logging.h"
#include "AppDialogSetRONACommand.h"  
#include <stdio.h> 
#include "Call.h"

using namespace resip;
AppDialogSetRONACommand::AppDialogSetRONACommand(AppDialogSetHandle h) : mHandle(h)
{
}

void AppDialogSetRONACommand::executeCommand()
{ 
    if(mHandle.isValid()) 
    { 
        Call *call = dynamic_cast<Call*>(mHandle.get());
        Logging::log("AppDialogSetRONACommand::executeCommand: %i\r\n",call);
        if (call)
        {
            if (call->getCallState()==Call::AgentRinging)
            {
                call->onAssignFailed();
                mHandle->end();
            }
        }
    }
    else
    {
        Logging::log("AppDialogSetRONACommand::executeCommand: canceled\r\n");
    }
}

Message* AppDialogSetRONACommand::clone() const 
{
    AppDialogSetRONACommand *cmd = new AppDialogSetRONACommand(mHandle);

    return cmd; 
}
std::ostream& AppDialogSetRONACommand::encode(std::ostream& strm) const 
{
    return strm; 
}
std::ostream& AppDialogSetRONACommand::encodeBrief(std::ostream& strm) const 
{
    return strm; 
}
