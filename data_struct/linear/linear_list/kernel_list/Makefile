CC ?= gcc
TARGET := list
OBJS := main.o



INCLUDE += ./
CFLAGS += -I $(INCLUDE)


$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ $(CFLAGS) -o $@

clean:
	rm $(OBJS) $(TARGET)
