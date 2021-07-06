all:
	g++ -g -std=c++11 -o server buff.h poll.h iohandler.h engine.h protocol.h server.h socket.cpp acceptor.cpp endpoint.cpp poll.cpp svrmain.cpp 
	g++ -g -std=c++11 -o client buff.h poll.h iohandler.h carriage.h protocol.h client.h socket.cpp startpoint.cpp poll.cpp climain.cpp 
clean:
	rm -f server
	rm -f client
