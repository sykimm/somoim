#pragma once
#include <vector>
#include <map>
#include <string>
using namespace std;
#include "util.h"


class Post {
    int pid;
    string memId; // id
    string title;
    string content;
    vector<string> replys;
public:
    static int n;
    Post(string writer){   // 작성하기 누르면
        this->pid = n++;
        this->memId = writer; 
    }
    ~Post(){};
    void setTitle(string s) { this->title = s.substr(0, 80); }
    string getTitle() { return title; }
    string getWriter() { return memId; }
    string getContent() { return content; }
    void setContent(string s) { this->content = s.substr(0, 500); }
    void addReply(int sd){
        sendMsg(sd, ">> ");
        char reply[80];
        recvMsg(sd, reply);
        replys.push_back(reply);
        sendMsg(sd, "clear"); usleep(2000);
    }
    void showReplys(int sd){
        int i=0;
        char buf[1024];
        for(string r: replys){
            sprintf(buf, "%2d] %s\n", ++i, r.c_str());
            sendMsg(sd, buf);
        }
    }
};