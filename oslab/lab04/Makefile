CC = gcc
CFLAGS = -Wall -g -pthread
TARGETS = producer consumer smoker
SRCS = producer.c consumer.c smoker.c ipc.c
OBJS = $(SRCS:.c=.o)

all: $(TARGETS)

producer: producer.o ipc.o
	$(CC) $(CFLAGS) -o $@ $^

consumer: consumer.o ipc.o
	$(CC) $(CFLAGS) -o $@ $^

smoker: smoker.o ipc.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

%.o: %.c ipc.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGETS) $(OBJS)

.PHONY: all clean
