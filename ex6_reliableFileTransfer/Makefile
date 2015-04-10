AR=ar
ARFLAGS=rvs
GPP=g++

CLIENTSRC=clftp.cpp 
CLIENTOBJ=$(CLIENTSRC:.cpp=.o) $(CLIENTDEP:.cpp=.o)

SERVERSRC=srftp.cpp 
SERVEROBJ=$(SERVERSRC:.cpp=.o) $(SERVERDEP:.cpp=.o)

CPPFLAGS = -Wall -g

CLIENTEXE = clftp
SERVEREXE = srftp
TARGETS = $(CLIENTEXE) $(SERVEREXE)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex6.tar
TARSRCS=$(CLIENTSRC) $(CLIENTDEP:.cpp=.h) $(SERVERSRC) $(SERVERDEP:.cpp=.h) Makefile README performance.jpg

all: $(TARGETS)


$(CLIENTEXE).o: $(CLIENTSRC)
	$(GPP) $(CPPFLAGS) -o $@ -c $<

$(CLIENTEXE): $(CLIENTOBJ)
	$(GPP) $(CPPFLAGS) -o $@ $(CLIENTOBJ)
	
	
$(SERVEREXE).o: $(SERVERSRC)
	$(GPP) $(CPPFLAGS) -o $@ -c $<

$(SERVEREXE): $(SERVEROBJ)
	$(GPP) $(CPPFLAGS) -o $@ $(SERVEROBJ)


clean:
	$(RM) $(TARGETS) $(CLIENTOBJ) $(SERVEROBJ) *~

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)