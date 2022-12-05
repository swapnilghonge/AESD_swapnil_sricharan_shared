SRC := bme.c
TARGET = bme
OBJS := $(SRC:.c=.o)

CC=$(CROSS_COMPILER)gcc

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	-rm -f *.o $(TARGET) *.elf *.map
