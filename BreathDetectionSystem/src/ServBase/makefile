include ../makefile.def
LIBS=libServBase
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
#BASELIBS=../tinyxmllib/libtinyxml.a ../socks5/libsocks5.a ../jrtplib-2.7/libjrtp.a ../CommBase/libCommBase.a $(ACE_ROOT)/ace/libACE.a 
#BASELIBS=../tinyxmllib/libtinyxml.a ../CommBase/libCommBase.a

LIBSPATH=../../lib/
all:$(OBJS) $(LIBS).so $(LIBS).a

%.o:%.cpp include/%.h 
#	$(CC)  $< $(CCFLAGS) -o $@
	$(CC)  $< $(CCFLAGS) -DWEBCON_HAS_FILELOG -o $@

Main.o:Main.cpp
#	$(CC)  $< $(CCFLAGS) -o $@
	$(CC)  $< $(CCFLAGS) -DWEBCON_HAS_FILELOG -o $@

$(LIBS).so:$(OBJS)
	gcc -Wall -shared $^ -o $(LIBS).so
	cp -dpR $(LIBS).so $(LIBSPATH)

#$(LIBS).a:$(OBJS) $(BASELIBS)
$(LIBS).a:$(OBJS) 
	ar r $@ $^
	#mv $(LIBS).a $(LIBSPATH)
clean:
	$(RM) $(OBJS) $(LIBS).so $(LIBS).a

