TYPE_MODULES:=General Osmocom MNCC PCUIF

TYPE_CC=$(TYPE_MODULES:%=%_Types.cc)
TYPE_HH=$(TYPE_MODULES:%=%_Types.hh)
TYPE_OBJS=$(TYPE_MODULES:%=%_Types.o)
GENERATED=$(TYPE_CC) $(TYPE_HH)

OBJS=$(TYPE_OBJS) hello.o

CXXFLAGS=-DLINUX -DMAKEDEPEND_RUN -I/usr/include/titan -fPIC
LDFLAGS=-L/usr/lib/titan -fPIC -lttcn3-dynamic
LDFLAGS_STATIC=-L/usr/lib/titan -lttcn3 -lxml2 -lssl -lcrypto

all: compile titan.a

$(GENERATED):
	ttcn3_compiler -D *.ttcn

titan.a: $(OBJS)
	$(AR) r $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	@rm -f titan.a $(OBJS) $(GENERATED)
