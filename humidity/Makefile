CROSS_COMPILE=

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall
endif

ifeq ($(LIBS),)
	LIBS= -lrt -lpthread -lpigpio
endif

all: humidity
default: humidity

humidity : humidity.o 
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o humidity humidity.o $(LIBS)

humidity.o : humidity.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c humidity.c $(LIBS)

clean :
	rm -f humidity *.o
