#ifndef _UTIL_H
#define _UTIL_H
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;

void sendMsg(int sd, string s);
string recvMsg(int sd, char* buf);

#endif  