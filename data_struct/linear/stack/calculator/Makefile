CC ?= gcc

TARGET := calculator_demo

OBJS := main.o list.o stack.o calculator.o

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ -o $@

clean:
	rm $(TARGET) $(OBJS)
