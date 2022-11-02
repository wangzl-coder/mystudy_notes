CC ?= gcc

TARGET := list_stack_demo

OBJS := main.o list.o stack.o

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ -o $@

clean:
	rm $(TARGET) $(OBJS)
