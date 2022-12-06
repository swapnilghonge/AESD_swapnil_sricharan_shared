ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif
ifeq ($(CFLAGS),)
	CFLAGS = -g -Wall -Werror
endif
ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif
all:lcd

clean:
	rm -f *.o lcd *.elf *.map

lcd: lcd.o
	#$(CC) $(CFLAGS)  -c -o aesdsocket.o aesdsocket.c
	#$(CC) $(CFLAGS) -I/ aesdsocket.o -o aesdsocket
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)
	

	

