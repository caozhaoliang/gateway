BINFILE := libcore.a

LIBS := 

INCS :=  ../../comm/include \
         ../../core/include \
         ../include \
         ../../outlib/jsoncpp
         

LIBPATHS := 


USER_MARCOS := _LINUX_OS_ _DAEMON_ _LINUX_EPOLL_
test=1
ifeq ($(test), 1)
USER_MARCOS := $(USER_MARCOS) _UNIT_TEST_
endif

CFLAGS = -g -Wall -pg  #-O2  
AR_CFLAGS = -crv
CC = g++ 
AR = ar

COMMONSRCS=$(wildcard ../../core/src/*.cpp)

COMMONOBJS=$(patsubst %.cpp,%.o,$(COMMONSRCS))

SOURCE := $(wildcard ./*.cpp)  
OBJS := $(patsubst %.cpp,%.o,$(SOURCE))

all:$(BINFILE)
	cp $(BINFILE) ../lib/

$(BINFILE): $(OBJS) $(COMMONOBJS)
	$(AR) $(AR_CFLAGS) -o $@ $^   $(addprefix -L,$(LIBPATHS)) $(addprefix -l,$(LIBS)) 
  
%.o:%.cpp
	$(CC) $(CFLAGS) $(addprefix -D,$(USER_MARCOS)) $(addprefix -I,$(INCS)) -c $< -o $@


clean :  
	rm -rf $(COMMONOBJS) $(OBJS) $(BINFILE)
	rm ../lib/$(BINFILE)
