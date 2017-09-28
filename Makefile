OBJS = main.o Message.o UDP_Server.o UDP_Client.o
CC = g++
WARNINGS = -Wall -Werror -Wfatal-errors -Wextra -pedantic
CXXFLAGS = -c -g -O3 -march=native -std=c++11 $(WARNINGS)


all: main

main.o: main.cpp common.h UDP_Server.h UDP_Client.h
	$(CC) $(CXXFLAGS) main.cpp
	
Message.o: Message.cpp Message.h common.h UDP_Server.h UDP_Client.h 
	$(CC) $(CXXFLAGS) Message.cpp
	
UDP_Server.o: UDP_Server.cpp UDP_Server.h common.h
	$(CC) $(CXXFLAGS) UDP_Server.cpp
	
UDP_Client.o: UDP_Client.cpp UDP_Client.h common.h 
	$(CC) $(CXXFLAGS) UDP_Client.cpp
	
main: main.o Message.o UDP_Server.o UDP_Client.o
	$(CC) -Wall -g $(OBJS) -o main
	
clean:
	rm -f *.o 
	