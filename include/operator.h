#pragma once
#include "member.h"
using namespace std;

class Operator : public Member {
public:
    Operator(string id, string passwd) : Member(id, passwd) {}
    virtual ~Operator() {}
    virtual bool delPost(int pid) { return true; } // 남이 쓴 포스트도 삭제 가능
    virtual bool delReply(int pid) { return true; }
    bool hideChat(int tid); // talk id로 채팅 가리기 기능
    virtual string type() { return "operator"; }
};