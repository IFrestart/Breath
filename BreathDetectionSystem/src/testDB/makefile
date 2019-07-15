ifdef HONGCHENG
        ADDONSFLAGS+=-DHONGCHENG
endif
ifdef DBCFLAG
	ADDONSFLAGS+=-DDBC
endif

ifdef HOTBACK
        ADDONSFLAGS+=-DHOTBACK
endif

ifdef _DEMO_PERSNO_VER
        ADDONSFLAGS+=-D_DEMO_PERSNO_VER
endif

ifdef _BANKSYSTEM_VER
	ADDONSFLAGS+=-D_BANKSYSTEM_VER
endif

ifdef _WISDOM_PENSION_VER
	ADDONSFLAGS+=-D_WISDOM_PENSION_VER
endif

ifdef _WISDOM_PENSION_PVER
	ADDONSFLAGS+=-D_WISDOM_PENSION_PVER
endif

ifdef _CORPSYSTEM_VER
	ADDONSFLAGS+=-D__CORPSYSTEM_VER
endif


include ../makefile.def

#Personal Wisdom pension 2014-02-12
ADDONSFLAGS+=-D_WISDOM_PENSION_PVER
#ADDONSFLAGS+=-DWEBCON_HAS_PRINT_DEBUG

BASENAME=TestDBCommServ
TARGET=$(PRE)$(BASENAME)$(EXT)
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))



all:$(OBJS) $(TARGET)

%.o:%.cpp 
	$(CC) $^ $(CCFLAGS) -I./dbinclude -o $@

$(TARGET):$(OBJS) $(BASESERVOBJS)
	$(CC) $^ $(CFLAGS) -L./lib -lmysqlclient -o $@

#run:all
#	./$(TARGET)

clean:
	$(RM) $(OBJS) $(BASENAME) $(TARGET)

