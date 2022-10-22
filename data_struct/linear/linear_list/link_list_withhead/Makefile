CC ?= gcc
TARGET := weather
OBJS := main.o weather_list.o



INCLUDE += ./
CFLAGS += -I $(INCLUDE)


$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ $(CFLAGS) -o $@

clean:
	rm $(OBJS) $(TARGET)
