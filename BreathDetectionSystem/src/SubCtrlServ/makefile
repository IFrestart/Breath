include ../makefile.def

#ifdef __CALALARM_VER
ADDONSFLAGS+=-D__CALALARM_VER
#endif
TARGET=SubCtrlServ
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all:$(OBJS) $(TARGET)

%.o:%.cpp include/%.h 
	$(CC) $< $(CCFLAGS) -o $@

$(TARGET):$(OBJS) $(BASESERVOBJS)
	$(CC) $^ $(CFLAGS) -o $@

run:all
	./$(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET)

