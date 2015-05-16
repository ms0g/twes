OBJS = twebserv.o server.o utils.o http.o
CC = gcc
DEBUG = -g
IDIR = include
DEPS = server.h utils.h http.h
CFLAGS = -I$(IDIR) -Wall -c $(DEBUG)
LFLAGS = -I$(IDIR) -Wall $(DEBUG)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $< 

twebserv: $(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

clean:
	rm -f *.o twebserv
