OBJS = twebserv.o server.o utils.o http.o
TARGET = twebserv
CC = gcc
DEBUG = -g
IDIR = include
DEPS = server.h utils.h http.h
CFLAGS = -I$(IDIR) -Wall -c $(DEBUG)
LFLAGS = -I$(IDIR) -Wall $(DEBUG)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $< 

all: $(OBJS)
	$(CC) $(LFLAGS) $^ -o $(TARGET)

clean:
	rm -f *.o twebserv
