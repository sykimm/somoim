#ifndef _MANAGER_H
#define _MANAGER_H
#include <string>
#include <vector>
#include <iostream>
#include "leader.h"
#include "club.h"
#include "post.h"
#include "util.h"
using namespace std;



class Manager{
    // 회원리스트
    vector<Member*> memArr;
    vector<Club*> clubArr;
    
    // <회원id, [가입한 모임들,,,]>
    map<string, vector<Club*>> memMap; //회원가입할때 추가
    // <모임id, [가입한 회원들,,,]>
    map<int, vector<Member*>> clubMap;
    // key: club_id, value : vector<sd>
    // [enterChat] chatInfo에 가입한 club(id) 모두에 sd(int)추가하기
    // 1. 멤버가 처음 로그인할 때 (O->X)
    // 2. 모임에 막 가입할 때 (O->X)
    // 3. 모임 개설할 때 (O->X)
    // [exitChat] sd 제거하기
    // 1. 로그아웃할때 (O->X)
    // 2. 모임에서 나갈때 (아직 없음->X)
    // 3. 회원 탈퇴할때 (아직 없음->X)
    map<int, vector<int>> chatInfo;

public:
    Manager(){
        cout << "생성자" << endl;
    };
    ~Manager(){};
    bool join(int sd);
    bool login(int sd, Member& m);
    bool checkIsMember(int cid, string mid);
    void clubPage(int sd, vector<Club*>::iterator it, Member& m);
    void chatPage(int clubId, int sd, Member& m);
    void showAllClubs(int sd, Member& m);
    void boardPage(Club* club, int sd, Member& m);
    void searchClub();
    void makeClub(int sd, Member& l);
    bool joinClub(Club* c, Member& m);
    void showMyClubs(int sd, string id);
    void enterChat(string mid, int cid, int sd);
    void exitChat(string mid, int cid, int sd);
    void addPost(int cid, int sd, string mid);
};


#endif