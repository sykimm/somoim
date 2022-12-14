#pragma once
#include <string>
#include <vector>
#include "member.h"
#include "util.h"
#include "post.h"
using namespace std;

void showPost(int sd, Post* p);

class Club{
    int cid;
    char archive[1024]; // 서버에 있는 이 모임에 대한 db 위치
    string clubName;
    string description;
    vector<Post*> postArr;
    vector<Member*> memArr;
    // vector<Member*> archive;
    void showArrList(int sd, vector<Post*> &arr, string mid);
public:
    static int n;
    static void setN() { n = 0; }
    Club(int cid=-1);
    int getId(){ return cid; }
    string getName(){ return clubName; }
    string getDesc(){ return description; }
    void setName(string s){ clubName = s; }
    void setDesc(string s){ description = s; }
    void addPost(int sd, string mid);
    // void showPostList(int sd);
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
    // void postPage(int sd);s
    void delPost(int sd, vector<Post*>::iterator it, string mid);
    void searchPost(int sd, string mid);
    void boardPage(int sd, string mid);
    void showArchive(int sd);
    void download(int sd);
    void upload(int sd);
    void delArchive(int sd);
    void srchArchive(int sd);
};


