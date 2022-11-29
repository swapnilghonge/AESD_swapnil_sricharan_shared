CROSS_COMPILE=

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

ifeq ($(LDFLAGS),)
	LDFLAGS= -lrt
endif

all: servertest clienttest
default: servertest clienttest

servertest : server_socket.o
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o server_test server_socket.o $(LDFLAGS)
	
clienttest : client.o
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o client_test client.o $(LDFLAGS)

server_socket.o : server_socket.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c server_socket.c $(LDFLAGS)

clienttest.o : clienttest.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c client.c $(LDFLAGS)
	
clean :
	rm -f servertest clienttest *.o

	

