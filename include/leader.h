#pragma once
#include "operator.h"
using namespace std;

class Leader : public Operator {
public:
    Leader(string id, string passwd, string name, string phoneNo) : Operator(id, passwd, name, phoneNo) {}
    virtual ~Leader() {}
    bool dropMember(int id);
    virtual bool delPost(int pid) { return true; } // 남이 쓴 포스트도 삭제 가능
    virtual bool delReply(int pid) { return true; }
    virtual string type() { return "leader"; }
};