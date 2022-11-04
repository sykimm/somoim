INC = ../../include
LDFLAGS = -lpthread -ldl # -L../libsrc -lmenu -lhash -lpthread -ldl
OBJS = server.o manager.o util.o club.o
SRCS = $(OBJS:.o=.cpp)

server: $(OBJS)
	g++ -o $@ $(OBJS) $(LDFLAGS) -I$(INC)

.cpp.o: $(SRCS)
	g++ -c -g -I$(INC) $<

clean:
	rm $(OBJS) server