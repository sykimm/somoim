INC = ../../include
OBJS = main.o manager.o member.o
SRCS = $(OBJS:.o=.cpp)

a.out: $(OBJS)
	g++ -o $@ $(OBJS)

.cpp.o: $(SRCS)
	g++ -c -I$(INC) $<

clean:
	rm $(OBJS) a.out