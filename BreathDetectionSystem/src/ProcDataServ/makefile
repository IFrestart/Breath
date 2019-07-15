include ../makefile.def

#ifdef __CALALARM_VER
ADDONSFLAGS+=-D__CALALARM_VER
#endif
TARGET=ProcDataServ
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all:$(OBJS) $(TARGET)

%.o:%.cpp include/%.h 
	$(CC) $< $(CCFLAGS) -DWEBCON_HAS_FILELOG -o $@

$(TARGET):$(OBJS) $(BASESERVOBJS)
	$(CC) $^ $(CFLAGS) -DWEBCON_HAS_FILELOG -o $@

run:all
	./$(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET)

