LIB_SRCS = utils.c sock_events.c libc_overrides.c
LIB_OBJS = $(LIB_SRCS:.c=.o)

EXTRA_LIBS =

CFLAGS += -Wall
LDFLAGS= -fPIC

ifdef ENABLE_TITAN
EXTRA_LIBS += titan/titan.a
CFLAGS += -DENABLE_TITAN
LDFLAGS += -L/usr/lib/titan -lttcn3-dynamic
endif

default: libudtrace.so

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -o $@ -c $^

libudtrace.so: $(LIB_OBJS) $(EXTRA_LIBS)
	$(CC) $(LDFLAGS) -fPIC -shared -Wl,-soname,libudtrace.so -o $@ $^ -ldl

titan/titan.a:
	$(MAKE) -C titan titan.a

clean:
	rm -f *.o libudtrace.so
