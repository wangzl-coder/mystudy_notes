CC ?= gcc

TARGET := queue_demo

OBJS := main.o queue.o

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ -o $@

clean:
	rm $(TARGET) $(OBJS)
