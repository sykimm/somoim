#include "util.h"


void sendMsg(int sd, string s){
	send(sd, s.c_str(), s.size(), 0);
}

string recvMsg(int sd, char* buf){
    int n;
    
    n = recv(sd, buf, sizeof(buf), 0);
    buf[n] = '\0';

    return buf;
}