all:
	g++ -g -std=c++11 -c util.cpp socket.cpp acceptor.cpp startpoint.cpp endpoint.cpp evloop.cpp timer.cpp timer_tracker.cpp
	ar crv liblotus.a util.o socket.o acceptor.o startpoint.o endpoint.o evloop.o timer.o timer_tracker.o
	g++ -g -std=c++11 -o server svrmain.cpp -pthread -L. -llotus
	g++ -g -std=c++11 -o client climain.cpp -pthread -L. -llotus
	rm -f *.o
clean:
	rm -f *.o
	rm -f server
	rm -f client
