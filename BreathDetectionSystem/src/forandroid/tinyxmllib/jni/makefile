include ../../makefile.def

TARGET=libtinyxml.so libtinyxml.a
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
all:$(OBJS) $(TARGET)

%.o:%.cpp include/%.h
	$(CC) $< $(CCFLAGS) -o $@

tinyxmlerror.o:tinyxmlerror.cpp
	$(CC) $< $(CCFLAGS) -o $@

tinyxmlparser.o:tinyxmlparser.cpp
	$(CC) $< $(CCFLAGS) -o $@

libtinyxml.so:$(OBJS)
	$(CC) $^ $(CFLAGS) -shared -o $@
	cp libtinyxml.so ../../llib

libtinyxml.a:$(OBJS)
	ar r $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)

