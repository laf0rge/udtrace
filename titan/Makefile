
OBJS=General_Types.o Osmocom_Types.o hello.o
OBJS+=MNCC_Types.o MNCC_EncDec.o
OBJS+=PCUIF_Types.o

CXXFLAGS=-DLINUX -DMAKEDEPEND_RUN -I/usr/include/titan -fPIC
LDFLAGS=-L/usr/lib/titan -fPIC -lttcn3-dynamic
LDFLAGS_STATIC=-L/usr/lib/titan -lttcn3 -lxml2 -lssl -lcrypto

all: titan.a

titan.a: $(OBJS)
	$(AR) r $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

clean:
	@rm titan.a $(OBJS)
