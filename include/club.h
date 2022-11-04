#pragma once
#include <string>
#include <vector>
#include "member.h"
#include "util.h"
#include "post.h"
using namespace std;

class Club{
    
    int cid;
    string clubName;
    string description;
    vector<Post*> postArr;
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
    void addPost(int sd, string mid);
    void showAllPost(int sd);
    // int getTotalNo(){ return members.size(); }
    // void addMember(Member* m){
    //     cout << m->type() << endl;
    //     members.push_back(m);
    // }
};