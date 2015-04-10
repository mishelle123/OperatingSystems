AR=ar
ARFLAGS=rvs
GPP=g++

EXESRC=MyCachingFileSystem.cpp
EXEOBJ=MyCachingFileSystem.o MyCachingFileSystemDirect.o

FUSEFLAGS = `pkg-config fuse --cflags`
FUSELINK = `pkg-config fuse --libs`

CPPFLAGS = -Wall -g -lm 
CPPFLAGSDIRECT = -DO_DIRECT_MODE -Wall -g -lm 

CACHEFSEXE = MyCachingFileSystem
CACHEFSEXEDIRECT = MyCachingFileSystemDirect

TARGETS = $(CACHEFSEXE)
TARGETSDIRECT = $(CACHEFSEXEDIRECT)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex5.tar
TARSRCS=$(EXESRC) Makefile README

all: $(TARGETS)

directMode: $(TARGETSDIRECT)

$(CACHEFSEXE).o: $(EXESRC)
	$(GPP) $(CPPFLAGS) $(FUSEFLAGS) -o $@ -c $< 

$(CACHEFSEXE): $(CACHEFSEXE).o
	$(GPP) $(CPPFLAGS) -o $@ $(CACHEFSEXE).o $(FUSELINK)


$(CACHEFSEXEDIRECT).o: $(EXESRC) 
	$(GPP) $(CPPFLAGSDIRECT) $(FUSEFLAGS)  -o $@ -c $<  
	

$(CACHEFSEXEDIRECT): $(CACHEFSEXEDIRECT).o
	$(GPP) $(CPPFLAGSDIRECT) -o $(CACHEFSEXE) $(CACHEFSEXEDIRECT).o $(FUSELINK) 		
	
clean:
	$(RM) $(TARGETS) $(TARGETSDIRECT) $(EXEOBJ) *~

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
	
test:
	MyCachingFileSystem /tmp/root /tmp/mount 20 1000 -s -f
	
