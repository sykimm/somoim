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
    vector<Member*> memArr;
    // vector<Member*> archive;
    void showArrList(int sd, vector<Post*> arr);
public:
    static int n;
    static void setN() { n = 0; }
    Club();
    int getId(){ return cid; }
    string getName(){ return clubName; }
    string getDesc(){ return description; }
    void setName(string s){ clubName = s; }
    void setDesc(string s){ description = s; }
    void addPost(int sd, string mid);
    void showPostList(int sd);
    void addMember(Member* m){
        memArr.push_back(m);
    }
    int getTotalNo(){ return memArr.size(); }
    bool isMember(string mid){ 
        for(Member* mptr: memArr){
            if (mptr->getId() == mid)
                return true;
        }
        return false;
    }
    void postPage(int sd);
    void delPost(int sd);
    void searchPost(int sd);
    void boardPage(int sd, string mid);
    void showArchive();
};