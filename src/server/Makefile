INC = ../../include
LDFLAGS = -L../libsrc -ljoinMember -llogin -lpthread -ldl # -L../libsrc -lmenu -lhash -lpthread -ldl
OBJS = server.o manager.o member.o
SRCS = $(OBJS:.o=.cpp)


server: $(OBJS)
	g++ -o $@ $(OBJS) $(LDFLAGS)

.cpp.o: $(SRCS)
	g++ -c -g -I$(INC) $<

clean:
	rm $(OBJS) server