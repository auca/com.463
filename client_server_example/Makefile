CC = gcc
CFLAGS = -ansi -Wall -pedantic -ggdb

DEBUG_DIR = debug

all : client server

client : client.c
	mkdir -p $(DEBUG_DIR)
	$(CC) $(CFLAGS) -o $(DEBUG_DIR)/$@ $<

server : server.c
	mkdir -p $(DEBUG_DIR)
	$(CC) $(CFLAGS) -o $(DEBUG_DIR)/$@ $<

clean :
	rm -rf $(DEBUG_DIR)
