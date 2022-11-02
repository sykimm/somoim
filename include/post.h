#pragma once
#include <vector>
#include <map>
#include <string>
using namespace std;


class Post {
    int pid;
    int writer; // id
    string title;
    string content;
    string time;
    int likes;
    vector<map<int, string>> replys;
public:
    Post(int pid, int writer){   // 작성하기 누르면
        this->pid = pid;
        this->writer = writer; 
    }
    ~Post(){};
    void setTitle(string s) { this->title = s; }
    void setContent(string s) { this->content = s; }
    void upload(); // time 설정
    void addReply(map<int, string> reply);
    void delReply(int rid);
};