ifeq ($(CC),)
	CC = ${CROSS_COMPILE}gcc
endif
ifeq ($(CFLAGS),)
	CFLAGS = -g -Wall -Werror 
endif
ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif

default: tmp

all : tmp

tmp102: tmp.o
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

clean: 
	-rm -f *.o tmp
	
