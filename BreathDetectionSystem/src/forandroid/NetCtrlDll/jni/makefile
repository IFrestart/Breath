include ../../makefile.def

LIBS=libNetCtrl
LIBSPATH=../../llib

OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all:$(OBJS) $(LIBS).so $(LIBS).a

%.o:%.cpp ./include/%.h 
	$(CC) $< $(CCFLAGS) -o $@

#$(LIBS).so:$(OBJS) $(BASESERVOBJS)
$(LIBS).so:$(OBJS)
	gcc -Wall -shared $^ -o $(LIBS).so
	cp -dpR $(LIBS).so $(LIBSPATH)
#	mv $(LIBS).so ../testClient2/

$(LIBS).a:$(OBJS)
	ar r $@ $^
	mv $(LIBS).a $(LIBSPATH)

clean:
	$(RM) $(OBJS) $(LIBS).so

