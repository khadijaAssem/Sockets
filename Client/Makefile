CFLAGS = -O 
CC = g++ 

all: my_client2
 
my_client2: client.o utilities.o client_runner.o
	$(CC) $(CFLAGS) -o my_client2 client_runner.o client.o utilities.o
 
client_runner.o: client_runner.cpp
	$(CC) $(CFLAGS) -c client_runner.cpp 

client.o: client.cpp
	$(CC) $(CFLAGS) -c client.cpp 
 
utilities.o: utilities.cpp
	$(CC) $(CFLAGS) -c utilities.cpp 
 
clean: 
	rm -rf *o 