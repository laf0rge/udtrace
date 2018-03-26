LIB_SRCS = utils.c sock_events.c libc_overrides.c


default: libudtrace.so

libudtrace.so: $(LIB_SRCS)
	$(CC) $(CFLAGS) -fPIC -shared -Wl,-soname,libudtrace.so -o $@ $^ -ldl

clean:
	rm -f *.o libudtrace.so
