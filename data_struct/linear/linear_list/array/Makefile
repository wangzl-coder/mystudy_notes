CC ?= gcc

TARGET := arrlist

OBJS += main.o arrlist.o


INCLUDE += arrlist.h

CFLAGS += -I $(INCLUDE)


$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $^ -o $@

clean:
	rm $(OBJS) $(TARGET)
