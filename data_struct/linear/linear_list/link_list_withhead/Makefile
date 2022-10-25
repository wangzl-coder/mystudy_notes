CC ?= gcc
TARGET := list
OBJS := main.o weather_list.o list.o



INCLUDE += ./
CFLAGS += -I $(INCLUDE)


$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ $(CFLAGS) -o $@

clean:
	rm $(OBJS) $(TARGET)
