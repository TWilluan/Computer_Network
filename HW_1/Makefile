CC=g++
CFLAGS=-c -Wall
all: hw1

hw1: whoisclient.o whoisserver.o
	$(CC) whoisserver.o -o whoisserver
	$(CC) whoisclient.o -o whoisclient

whoisserver.o: whoisserver.cpp
	$(CC) $(CFLAGS) whoisserver.cpp

whoisclient.o: whoisclient.cpp
	$(CC) $(CFLAGS) whoisclient.cpp

clean:
	rm *.o