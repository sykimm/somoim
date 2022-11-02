INC = ../../include
CFLAGS = -fPIC -c  -g
OBJS = menu.o profile.o input.o
TARGET = libmenu.so libarray.so libhash.so

all: $(TARGET)

libmenu.so: $(OBJS)
	$(LD) -shared -o $@ $(OBJS)
	cp $@ ../../lib/
#	cp lib$@.so ../../bin/


libarray.so: array.o 
	$(LD) -shared -o $@ $<
	cp $@ ../../lib/
#	cp lib$@.so ../../bin/

libhash.so: hash.o 
	$(LD) -shared -o $@ $<
	cp $@ ../../lib/
#	cp lib$@.so ../../bin/

.c.o:
	$(CC) $(CFLAGS) -I$(INC) $< 

clean:
	rm *.o lib*.so 
