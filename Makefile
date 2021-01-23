all:
	g++ -g -std=c++11 -o server buff.h poll.h iohandler.h engine.h protocol.h server.h socket.cpp acceptor.cpp endpoint.cpp poll.cpp engine.cpp main.cpp 
clean:
	rm -f server
