OBJS = twebserv.o server.o utils.o
CC = gcc
DEBUG = -g
DEPS = server.h utils.h
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $< 

twebserv: $(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

clean:
	rm -f *.o twebserv
