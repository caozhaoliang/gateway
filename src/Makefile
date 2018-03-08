BINFILE := gateway-linux

LIBS := z pthread sybdb jsoncpp core

INCS := ../include \
        ../cpp-libs/comm/include \
        ../cpp-libs/core/include \
        ../cpp-libs/db_comm/sqlserver/include \
		/opt/kds/mobile-stock/3rd-soft/include
        #../../../outlib/sqlserver/include \
        #../../../outlib/jsoncpp 
         
#modify by czl for change lib paths at 20180226 begin
#LIBPATHS := ../../../outlib/sqlserver/lib \
#            ../../../outlib/jsoncpp/lib \
#            ../../../core/lib
LIBPATHS := /opt/kds/mobile-stock/3rd-soft/lib \
			../cpp-libs/core/lib


USER_MARCOS := _LINUX_OS_ _DAEMON_ _LINUX_EPOLL_  
test=1
ifeq ($(test), 1)
USER_MARCOS := $(USER_MARCOS) _UNIT_TEST_
endif

CFLAGS = -g -Wall -pg  #-O2  
CC = g++ 

COMMONSRCS=$(wildcard ../cpp-libs/comm/src/*.cpp ../cpp-libs/core/src/*.cpp ../cpp-libs/db_comm/sqlserver/src/*.cpp)

COMMONOBJS=$(patsubst %.cpp,%.o,$(COMMONSRCS))

SOURCE := $(wildcard ./*.cpp)  
OBJS := $(patsubst %.cpp,%.o,$(SOURCE))

all:$(BINFILE)
#add by czl for get version info at 20180222 begin
KDS_PROGRAM_NAME="SP"
KDS_PROGRAM_NAME_CN="金慧盈通短信平台"
MAIN_VERSION_STRING="1.0.0"
VERSION_STRING:=$(shell LANG=C git log --oneline -1 2>/dev/null |cut -c1-7)
version.h:force
	@echo Generating version.h ...
	@echo -e "# ifndef __KDS_MOBILE_STOCK_SMS_PLATFORM_VERSION_H__\n\
	# define __KDS_MOBILE_STOCK_SMS_PLATFORM_VERSION_H__\n\
	# define MAIN_VERSION_STRING \"$(MAIN_VERSION_STRING)\"\n\
	# define MINOR_VERSION_STRING \"$(VERSION_STRING)\"\n\
	# define KDS_PROGRAM_NAME \"$(KDS_PROGRAM_NAME)\"\n\
	# define KDS_PROGRAM_NAME_CN \"$(KDS_PROGRAM_NAME_CN)\"\n\
	# endif" > version.h
force:
#add by czl for get version info at 20180222 end
$(BINFILE):version.h $(OBJS) $(COMMONOBJS)
	$(CC) $(CFLAGS) -o $@ $^   $(addprefix -L,$(LIBPATHS)) $(addprefix -l,$(LIBS)) 
  
%.o:%.cpp
	$(CC) $(CFLAGS) $(addprefix -D,$(USER_MARCOS)) $(addprefix -I,$(INCS)) -c $< -o $@


clean :  
	rm -rf $(COMMONOBJS) $(OBJS) $(BINFILE) version.h
