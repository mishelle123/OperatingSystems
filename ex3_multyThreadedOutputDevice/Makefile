AR=ar
ARFLAGS= rvs
GPP=g++

LIBSRC=outputdevice.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)

CPPFLAGS = -Wall -g -lpthread
LOADLIBES = -L./

PFTLIB = liboutputdevice.a
TARGETS = $(PFTLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex3.tar
TARSRCS=$(LIBSRC) Makefile README

all: $(TARGETS)

$(TARGETS): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^


clean:
	$(RM) $(TARGETS) $(PFTLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS) 
	
