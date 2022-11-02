#pragma once
#include <string>
#include <vector>

#include "member.h"
using namespace std;

class Club{
    
    int cid;
    string clubName;
    string description;
    // vector<Member*> members;
public:
    static int n;
    static void setN() { n = 0; }
    Club(){ cid = n++; }
    int getId(){ return cid; }
    string getName(){ return clubName; }
    string getDesc(){ return description; }
    void setName(string s){ clubName = s; }
    void setDesc(string s){ description = s; }
    // int getTotalNo(){ return members.size(); }
    // void addMember(Member* m){
    //     cout << m->type() << endl;
    //     members.push_back(m);
    // }
};