# CROSS_COMPILE := aarch64-none-linux-gnu-
CC := $(CROSS_COMPILE)gcc
CFLAGS := -Wall -Werror

SRC = $(wildcard *.c) 
OBJS = $(SRC:.c=.o)

all : $(OBJS)

clean:
	-rm -f *.o

# $(info SRC=$(SRC))
