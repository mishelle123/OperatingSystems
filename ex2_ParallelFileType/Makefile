AR=ar
ARFLAGS= rvs
GPP=g++

LIBSRC=pft.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)

CPPFLAGS = -Wall -g 
LOADLIBES = -L./

PFTLIB = libpft.a
TARGETS = $(PFTLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS=$(LIBSRC) Makefile README compParaLevel.jpg

all: $(TARGETS)

$(TARGETS): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^


clean:
	$(RM) $(TARGETS) $(PFTLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS) 
	
