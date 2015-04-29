OBJS = twebserver.o server.o
CC = gcc
DEBUG = -g
DEPS = server.h
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $< 

twebserver: $(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

clean:
	rm -f *.o twebserver
