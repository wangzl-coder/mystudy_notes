CC ?= gcc

TARGET := stack_demo

OBJS := main.o sqstack.o

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ -o $@

clean:
	rm $(TARGET) $(OBJS)
