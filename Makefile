OBJS = twebserv.o server.o
CC = gcc
DEBUG = -g
DEPS = server.h
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $< 

twebserv: $(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

clean:
	rm -f *.o twebserv
