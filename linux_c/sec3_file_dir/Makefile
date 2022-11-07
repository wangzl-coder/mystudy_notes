CC ?= gcc

TARGET := $1

SRC := $(TARGET).c

SRCS := $(wildcard *.c)

TARGETS := $(patsubst %.c,%,$(SRCS))

$(TARGET) : $(SRC)
	$(CC) $^ -o $@

$(TARGETS):

clean:
	rm $(TARGETS)
