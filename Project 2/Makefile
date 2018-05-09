all: line1
line1: server.c client.c
	gcc -g -Wall -o server  server.c -lpthread -lrt
	gcc -g -Wall -o client  client.c -lpthread -lrt
clean:
	rm -fr *~ *.o prime
