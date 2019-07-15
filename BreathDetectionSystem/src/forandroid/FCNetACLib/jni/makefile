include ../../makefile.def

LIBS=libFCNetAC
LIBSPATH=../../llib
LIBOBJS=../../llib/libNetCtrl.a $(ADDONSOBJS)
#LIBOBJS=$(ADDONSOBJS)
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all:$(OBJS) $(LIBS).so $(LIBS).a

%.o:%.cpp ./include/%.h 
	$(CC) $< $(CCFLAGS) -o $@

#$(LIBS).so:$(OBJS) $(BASESERVOBJS)
$(LIBS).so:$(OBJS) $(LIBOBJS)
#$(LIBS).so:$(OBJS)
	gcc -Wall -shared $^ -o $(LIBS).so
	cp -dpR $(LIBS).so $(LIBSPATH)
#	mv $(LIBS).so ../testClient2/

#$(LIBS).a:$(OBJS) $(LIBOBJS)
$(LIBS).a:$(OBJS)
	ar r $@ $^
	mv $(LIBS).a $(LIBSPATH)

clean:
	$(RM) $(OBJS) $(LIBS).so

