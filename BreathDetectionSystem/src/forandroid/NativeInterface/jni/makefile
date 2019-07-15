include ../../makefile.def

BASENAME=ClientNativeInterface 
TARGET=$(PRE)$(BASENAME)$(EXT)
#LIBOBJS=../../lib/libFCNetAC.a ../../lib/libNetCtrl.a $(ADDONSOBJS) $(ACE_ROOT)/ace/libACE.a
#LIBOBJS=../../lib/libFCNetAC.a
#LIBOBJS=$(ADDONSOBJS) $(ACE_ROOT)/ace/libACE.a
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
#CFLAGS+= -lNetCtrl -lFCNetAC
#CFLAGS+= -lFCNetAC -lNetCtrl
CFLAGS+= -lFCNetAC
all:$(OBJS) $(TARGET)

%.o:%.cpp include/%.h 
	$(CC) $< $(CCFLAGS) -o $@

#$(TARGET):$(OBJS) $(LIBOBJS) $(BASESERVOBJS)
$(TARGET):$(OBJS)
#$(TARGET):$(OBJS) $(LIBOBJS)
	#$(CC) $^ $(CFLAGS) -o $@
	$(CC) $^ -o $@ $(CFLAGS)

run:all
	./$(TARGET)

clean:
	$(RM) $(OBJS) $(BASENAME) $(TARGET)

