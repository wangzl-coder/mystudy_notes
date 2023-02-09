CC ?= gcc
AR ?= ar

CFLAGS += -I./
CFLAGS += -pthread
CFLAGS += -Wall
LDFLAGS += -pthread
LDFLAGS += -Wall


TARGET:=queue_pool_demo
LIB += libqueue_pool.a
LIBOBJS += queue_pool.o
OBJS += main.o

LIBNAME :=  $(subst .a,, $(LIB))
LIBNAME :=  $(subst lib,, $(LIBNAME))

$(TARGET):$(LIB) $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ -L ./ -l$(LIBNAME)

$(LIB):$(LIBOBJS)
	$(AR) -rcs $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm $(TARGET) $(OBJS) $(LIBOBJS) $(LIB)

