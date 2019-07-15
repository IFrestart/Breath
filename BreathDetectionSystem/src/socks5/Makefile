
G=..

CC=g++
FLAGS=-g 

CCFLAGS=$(FLAGS) -c -I./include
CFLAGS=$(FLAGS) 
RM=rm -f core.* *.log
LIBS=libsocks5

TARGET=test
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
all:$(OBJS) $(TARGET) $(LIBS).so $(LIBS).a

%.o:%.cpp 
	$(CC) $^ $(CCFLAGS) -o $@

$(TARGET):$(TARGET).o $(OBJS)
	$(CC) $^ $(CFLAGS) -o $@

$(LIBS).so:$(OBJS)
	gcc -Wall -shared $^ -o $(LIBS).so
	cp $(LIBS).so ../../lib/

$(LIBS).a:$(OBJS)
	ar r $@ $^

run:all
	./$(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET) $(LIBS).so

