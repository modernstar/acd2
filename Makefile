all: application
OBJECTS = AccountSettings.o MusicOnHoldServer.o Acd.o Call.o CallManager.o UserAgent.o CallFactory.o AppDialogSetEndCommand.o RegistrationManager.o RegistrationDialogSet.o AppDialogSetRONACommand.o Logging.o StaticConfiguration.o helper.o DynamicConfiguration.o RTPSession.o RTPSessionFactory.o WebSocketController.o SoundFile.o
SOURCES=$(OBJECTS:.o=.cpp)
LIBS=json/json.a websocket/websocket.a
INCLUDES=-I websocket/ -I json/

clean:
	-rm *.a
	-rm *.o
	cd json && make clean
	cd websocket && make clean

.cpp.o:
	g++ -std=c++11 $(INCLUDES) -g -c $< -o $@

json.a: 
	cd json && make

websocket.a: 
	cd websocket && make

application: $(OBJECTS) main.o json.a websocket.a
	g++ -g $(OBJECTS) main.o $(LIBS) -o acdserver -ldum -lresip -lrutil -lpthread -lrt -lortp

run: application
	./acdserver -s acdserver.conf -q queues.conf
daemon: application
	./acdserver -s acdserver.conf -q queues.conf -d

