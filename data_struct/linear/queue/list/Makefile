CC ?= gcc

TARGET := list_queue_demo

OBJS := main.o list.o queue.o

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ -o $@

clean:
	rm $(TARGET) $(OBJS)
