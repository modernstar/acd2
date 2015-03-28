# acd2
A SIP ACD server created for CTI systems. Provides a websocket/JSON control interface

This project has dependencies on the json repo and the websocket repo. You must clone with --recursive
Also has external dependencies on resiprocate-1.9.6 and ortp-0.22.0

Intro
==============
Why is it called Acd2? Where is Acd1?
I initially made a server that lets agents dial *8 to indicate that they were ready to accept calls
and *9 when they are not ready. The server would keep track of their status using NOITFY (same way BLF works).
Once a call came in on a queue, the server would dispatch the call to the most idle agent. I still have the code
for that version but I did not add it on gihub because I wasn't maitaining it anymore. But if you
want to look at it, go to http://www.dumaisnet.ca/index.php?article=237ad185f38cd5d291b934454121ead0


With this implemention, I removed the Agent logic. Now the server is much more simple: it only answers calls, queues them, and
let's a CTI user pickup a call from a queue using a websocket interface. The call is then blind transfered to that agent.
If the call is not answered within a configurable amount of time, it is requeued.

With this new version, I find that it is more usefull for a CTI system.


Usage
==============
invocation: acdserver -s serverconfig.conf -q queues.conf

Queues config file:
```
queue queueName1
queue queueName2
queue queueName3
```

Server config
```
#Low RTP Port
LOWPORT: 40000

#High RTP Port
HIGHPORT: 41000

#SIP Listening address
LISTENADDRESS: 192.168.1.3

#SIP Listening port
LISTENPORT: 5080

#SIP account username
ACCOUNT_USERNAME: awesomeusername

#SIP account password
ACCOUNT_PASSWORD: superpassword

#SIP server IP
ACCOUNT_PBX_IP: 192.168.1.3:5099

#SIP Display name
ACCOUNT_DISPLAY_NAME: ACD SERVER

#SIP User agent
ACCOUNT_USERAGENT: ACD_SERVER

#Timeout in seconds to requeue a transfered call if destination does not answer
RONA: 10
```
