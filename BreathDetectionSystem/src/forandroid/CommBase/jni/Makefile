
G=../../

CC=g++
FLAGS=-g 

#CCFLAGS=$(FLAGS) -c -I./include -I$(ACE_ROOT)
#CFLAGS=$(FLAGS) -lpthread -L$(ACE_ROOT)/ace -lACE
CCFLAGS=$(FLAGS) -c -I./include -I../../jrtplib-2.7/jni/ -I$Gtinyxmllib/jni/include -I$Gsocks5/jni/include
CFLAGS=$(FLAGS) -lpthread 
RM=rm -f core.* *.log
LIBS=libCommBase
LIBSPATH=../../llib

BASELIB=../../llib/libjrtp.a ../../llib/libsocks5.a ../../llib/libtinyxml.a

TARGET=test
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
all:$(OBJS) $(TARGET) $(LIBS).so $(LIBS).a

%.o:%.cpp 
	$(CC) $^ $(CCFLAGS) -o $@

$(TARGET):$(TARGET).o $(OBJS) $(BASELIB)
	$(CC) $^ $(CFLAGS) -o $@

$(LIBS).so:$(OBJS)
	gcc -Wall -shared $^ -o $(LIBS).so
	cp $(LIBS).so $(LIBSPATH)

$(LIBS).a:$(OBJS)
	ar r $@ $^
	cp $(LIBS).a $(LIBSPATH)

run:all
	./$(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET) $(LIBS).so

