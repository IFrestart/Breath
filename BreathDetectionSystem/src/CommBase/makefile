include ../makefile.def
LIBS=libCommBase
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
MODCCFLAGS=$(CCFLAGS) -L./jrtplib-2.7 -I./jrtplib-2.7
MODCFLAGS=$(CFLAGS) -L./jrtplib-2.7 -I./jrtplib-2.7 -ljrtp -lpthread 

#BASELIBS=../socks5/libsocks5.a ../jrtplib-2.7/libjrtp.a
#BASELIBS=../jrtplib-2.7/libjrtp.a $(ACE_ROOT)/ace/libACE.a 
#BASELIBS=../jrtplib-2.7/libjrtp.a

LIBSPATH=../../lib/

all:$(OBJS)  $(LIBS).so $(LIBS).a

%.o:%.cpp include/%.h 
	$(CC) $< $(MODCCFLAGS) -o $@

$(LIBS).so:$(OBJS)
	gcc -Wall -shared $^ -o $(LIBS).so
	cp -dpR $(LIBS).so $(LIBSPATH)

#$(LIBS).a:$(OBJS) $(BASELIBS)
$(LIBS).a:$(OBJS)
	ar r $@ $^
	#mv $(LIBS).a $(LIBSPATH)
clean:
	$(RM) $(OBJS) $(LIBS).a $(LIBS).so

