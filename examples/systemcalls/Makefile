# CROSS_COMPILE := aarch64-none-linux-gnu-
CC := $(CROSS_COMPILE)gcc
SRC := systemcalls.c
OBJS := $(SRC:.c=.o)

all : $(OBJS)

clean:
	-rm -f *.o
