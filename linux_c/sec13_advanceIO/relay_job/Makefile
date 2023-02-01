CC?=gcc
AR?=ar
CFLAGS += -pthread
CFLAGS += -I ./
LDFLAGS += -pthread
EXEC:=rejob
LIBS+=librejob.a
OBJS +=main.o
LIBOBJS += rejob.o

$(EXEC):$(OBJS) $(LIBS)
	$(CC) $^ $(LDFLAGS) -o $@

$(LIBS):$(LIBOBJS)
	$(AR) rcs $@ $^

%.o:%.c
	$(CC) -c $^ $(CFLAGS) -o $@

clean:
	rm $(EXEC) $(LIBS) $(LIBOBJS) $(OBJS)
