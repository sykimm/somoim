INC = ../../include # login.cpp랑 joinMember.cpp에서 "manager.h" include해서
CFLAGS = -fPIC -c -g # 공유라이브러리를 만들기 위한 .o파일을 만들기 위함
TARGET = liblogin.so libjoinMember.so

all: $(TARGET)

liblogin.so: login.cpp 
	g++ -shared -fPIC -I$(INC) -o $@ $<
	cp $@ ../../lib/

libjoinMember.so: joinMember.cpp 
	g++ -shared -fPIC -I$(INC) -o $@ $<
	cp $@ ../../lib/

# .cpp.o:
# 	g++ $(CFLAGS) -I$(INC) $< 

clean:
	rm *.o lib*.so 