OBJS = client.o
SRCS = $(OBJS:.o=.cpp)

client: $(OBJS)
	g++ -o $@ $(OBJS) -lpthread

.cpp.o:	$(SRCS)
	g++	-c $<

clean:
	rm *.o client
