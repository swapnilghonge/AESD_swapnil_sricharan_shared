ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif
ifeq ($(CFLAGS),)
	CFLAGS = -g -Wall -Werror
endif
ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif
all:server_socket client
default: server_socket client

clean:
	rm -f *.o server_socket *.elf *.map
	rm -f *.o client *.elf *.map

aesdsocket: aesdsocket.c
	#$(CC) $(CFLAGS)  -c -o aesdsocket.o aesdsocket.c
	#$(CC) $(CFLAGS) -I/ aesdsocket.o -o aesdsocket
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)
	

	

