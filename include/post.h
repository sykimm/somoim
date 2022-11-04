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
    string time;
    int likes;
    vector<map<int, string>> replys;
public:
    static int n;
    Post(string writer){   // 작성하기 누르면
        this->pid = n++;
        this->memId = writer; 
    }
    ~Post(){};
    void setTitle(string s) { this->title = s; }
    string getTitle() { return title; }
    void setContent(string s) { this->content = s; }
    void upload(); // time 설정
    void addReply(map<int, string> reply);
    void delReply(int rid);
};