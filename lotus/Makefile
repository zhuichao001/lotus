all:
	g++ -g -std=c++11 -o server buff.h poll.h iohandler.h engine.h protocol.h server.h util.cpp socket.cpp acceptor.cpp endpoint.cpp poll.cpp iohandler.cpp svrmain.cpp -pthread
	g++ -g -std=c++11 -o client buff.h poll.h iohandler.h engine.h protocol.h dialer.h util.cpp socket.cpp startpoint.cpp poll.cpp iohandler.cpp climain.cpp -pthread
clean:
	rm -f server
	rm -f client
