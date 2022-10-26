CC ?= gcc
TARGET := main
OBJS := main.o stu_list.o loop_list.o



INCLUDE += ./
CFLAGS += -I $(INCLUDE)


$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ $(CFLAGS) -o $@

clean:
	rm $(OBJS) $(TARGET)
