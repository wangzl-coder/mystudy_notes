CC ?= gcc
target := $0
src := $(target).c
srcs := $(wildcard *.c)
targets := $(patsubst %.c,%,$(srcs))


$(target) : $(src)
	CC $^ -o $@

$(targets):
