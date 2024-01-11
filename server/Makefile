CC := $(CROSS_COMPILE)gcc
SRC := aesdsocket.c logging.c fileutils.c signals.c
OBJS := $(SRC:.c=.o)
TARGET := aesdsocket

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean :
	-rm -f *.o $(TARGET) *.elf *.map

run : $(TARGET)
	./$(TARGET)
