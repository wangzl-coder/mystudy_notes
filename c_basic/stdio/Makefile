CC=gcc

excu:=$0

src:=$(excu).c

srcs:=$(wildcard *.c)

execus:=$(patsubst %.c,%,$(srcs))

$(excu):$(src)
	$(CC) $^ -o $@

$(execus):
