CC=gcc

excu:=$0

src:=$(excu).c

srcs:=$(wildcard *.c)

execus:=$(patsubst %.c,%,$(srcs))

$(excu):$(src)
	$(CC) $^ -o $@


#指定当前文件所有目标，可make + 制表符自动补齐
$(execus):

clean:
	rm $(execus)
