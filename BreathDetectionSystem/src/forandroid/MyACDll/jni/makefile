include ../../makefile.def
LIBS=libMyAC
LIBSPATH=../../lib
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all:$(OBJS) $(LIBS).so $(LIBS).a

%.o:%.cpp ../include/%.h
	$(CC) $< $(CCFLAGS) -o $@

$(LIBS).so:$(OBJS)
	gcc -Wall -shared $^ -o $(LIBS).so
	mv $(LIBS).so $(LIBSPATH)

$(LIBS).a:$(OBJS)
	ar r $@ $^
	mv $(LIBS).a $(LIBSPATH)

clean:
	$(RM) $(OBJS)

