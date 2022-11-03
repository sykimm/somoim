#ifndef _MANAGER_H
#define _MANAGER_H
#include <string>
#include <vector>
#include <iostream>
#include "leader.h"
#include "club.h"
#include "post.h"
using namespace std;


void sendMsg(int sd, string s);

class Manager{
    // 회원리스트
    vector<Member*> memArr;
    vector<Club*> clubArr;
    // <회원id, [가입한 모임들,,,]>
    map<string, vector<Club*>> memMap; //회원가입할때 추가
    // <모임id, [가입한 회원들,,,]>
    map<int, vector<Member*>> clubMap;
    // vector<Member*> member_mng;
    // vector<Club*> club_mng;
    vector<Post*> post_mng;
    
public:
    Manager(){
        cout << "생성자" << endl;
    };
    ~Manager(){};
    bool join(int sd);
    bool login(int sd, Member& m);
    void clubPage(int sd, vector<Club*>::iterator it);
    void showAllClubs(int sd);
    void searchClub();
    void makeClub(int sd, Member& l);
    void showMyClubs(int sd, string id);
};


#endif