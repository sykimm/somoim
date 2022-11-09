#pragma once
#include "operator.h"
using namespace std;

class Leader : public Operator {
public:
    Leader(string id, string passwd, string name, string phoneNo) : Operator(id, passwd, name, phoneNo) {}
    virtual ~Leader() {}
    // bool dropMember(int id);
    virtual string type() { return "leader"; }
};