all: application
OBJECTS = AccountSettings.o MusicOnHoldServer.o Acd.o Call.o CallManager.o UserAgent.o CallFactory.o AppDialogSetEndCommand.o RegistrationManager.o RegistrationDialogSet.o AppDialogSetRONACommand.o Logging.o StaticConfiguration.o helper.o DynamicConfiguration.o RTPSession.o RTPSessionFactory.o WebSocketController.o SoundFile.o PlayList.o
SOURCES=$(OBJECTS:.o=.cpp)
DUMAISLIBSDK=dumaislib/sdk/lib
LIBS=$(DUMAISLIBSDK)/json.a $(DUMAISLIBSDK)//websocket.a
INCLUDES=-I dumaislib/

clean:
	-rm *.a
	-rm *.o
	cd dumaislib && make clean

.cpp.o:
	g++ -std=c++11 $(INCLUDES) -g -c $< -o $@

.PHONY: libs
libs: 
	cd dumaislib && make

application: libs $(OBJECTS) main.o
	g++ -g $(OBJECTS) main.o $(LIBS) -o acdserver -ldum -lresip -lrutil -lpthread -lrt -lortp

run: application
	./acdserver -s acdserver.conf -q queues.conf
daemon: application
	./acdserver -s acdserver.conf -q queues.conf -d

