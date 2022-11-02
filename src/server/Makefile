INC = ../../include
LDFLAGS = -L. -lmanager -lpthread -ldl # -L../libsrc -lmenu -lhash -lpthread -ldl
OBJS = server.o  member.o

SRCS = $(OBJS:.o=.cpp)

all: libmanager.so server

libmanager.so: manager.cpp
	g++ -shared -fPIC -o $@ $< -I$(INC)

server: $(OBJS)
	g++ -o $@ $(OBJS) $(LDFLAGS) -I$(INC)



.cpp.o: $(SRCS)
	g++ -c -g -I$(INC) $<

clean:
	rm $(OBJS) server