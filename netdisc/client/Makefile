SRCS:=$(wildcard *.c)
TARGETS:=client
CC:=gcc
$(TARGETS):$(SRCS)
	gcc $^ -o $@ -pthread -lssl -lcrypto  -Wall -lcrypt
clean:
	rm -rf $(TARGETS)
