CFLAGS = -O 
CC = g++ 

all: my_server
 
my_server: server.o utilities.o server_runner.o
	$(CC) $(CFLAGS) -pthread -o my_server server_runner.o server.o utilities.o
 
server_runner.o: server_runner.cpp
	$(CC) $(CFLAGS) -pthread -c server_runner.cpp 

server.o: server.cpp
	$(CC) $(CFLAGS) -pthread -c server.cpp
 
utilities.o: utilities.cpp
	$(CC) $(CFLAGS) -pthread -c utilities.cpp 
 
clean: 
	rm -rf *o 