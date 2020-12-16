CC=gcc
CFLAGS=-O3 -Wall

all: server client
utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c
server.o: server.c
	$(CC) $(CFLAGS) -c server.c
client.o: client.c
	$(CC) $(CFLAGS) -c client.c
server: server.o utils.o
	$(CC) $(CFLAGS) -o server  utils.o server.o
client: client.o
	$(CC) $(CFLAGS) -o client client.o

.PHONY: clean
clean:
	rm -f *.o