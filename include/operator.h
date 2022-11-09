#pragma once
#include "member.h"
using namespace std;

class Operator : public Member {
public:
    Operator(string id, string passwd, string name, string phoneNo) : Member(id, passwd, name, phoneNo) {}
    virtual ~Operator() {}
    // bool hideChat(int tid); // talk id로 채팅 가리기 기능
    virtual string type() { return "operator"; }
};