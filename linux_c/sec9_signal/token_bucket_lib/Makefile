CC ?= gcc
AR ?= ar

TARGET := token_bucket_exec

EXECOBJS := main.o 

LIBOBJS := token_bucket.o
LIBNAME := token_bucket
LIB := lib$(LIBNAME).a



CFLAGS += -Wall -I ./
LDFLAGS += -Wall -static

$(TARGET):$(EXECOBJS) $(LIB)
	$(CC) $(LDFLAGS) $(EXECOBJS) -L. -l$(LIBNAME) -o $@

$(LIB):$(LIBOBJS)
	$(AR) rcs $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm *.o $(TARGET) *.a
