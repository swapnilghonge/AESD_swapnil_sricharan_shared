ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif
ifeq ($(CFLAGS),)
	CFLAGS = -g -Wall -Werror
endif
ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif
all:client

clean:
	rm -f *.o client *.elf *.map

client: client.o
	#$(CC) $(CFLAGS)  -c -o client.o client.c
	#$(CC) $(CFLAGS) -I/ client.o -o client
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)
	

	

