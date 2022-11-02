OBJS = client.o
SRCS = $(OBJS:.o=.c)

client: $(OBJS)
	$(CC) -o $@ $(OBJS) -lpthread

.c.o:	$(SRCS)
	$(CC)	-c $<

clean:
	rm *.o client
