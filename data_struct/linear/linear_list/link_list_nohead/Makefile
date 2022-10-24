CC ?= gcc
TARGET := student
OBJS := main.o stu_list.o



INCLUDE += ./
CFLAGS += -I $(INCLUDE)


$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ $(CFLAGS) -o $@

clean:
	rm $(OBJS) $(TARGET)
