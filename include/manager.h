#ifndef _MANAGER_H
#define _MANAGER_H
#include <string>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "leader.h"
#include "club.h"
#include "post.h"
#include "util.h"
using namespace std;



class Manager{
    // 회원리스트
    vector<Member*> memArr;
    vector<Club*> clubArr;
    // key: club_id, value : vector<sd>
    map<int, vector<int>> chatInfo;
    char cwd[512], db_dir[1024];

public:
    Manager(){
        cout << "생성자" << endl;        
        getcwd(cwd, sizeof(cwd));
        sprintf(db_dir, "%s/database", cwd);
        if (mkdir(db_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0){
            cout << "자료실 생성 성공" << endl;
        }else{
            cout << "자료실 생성 실패" << endl;
        }
    };
    ~Manager(){};
    bool join(int sd);
    bool login(int sd, Member& m);
    void clubPage(int sd, Club* clubPtr, Member& m);
    void chatPage(int clubId, int sd, Member& m);
    void showAllClubs(int sd, Member& m);
    void searchClub();
    void makeClub(int sd, Member& l);
    bool joinClub(Club* c, Member& m);
    void enterChat(int cid, int sd);
    void exitChat(int cid, int sd);
    void showMyClubs(Member& m, int sd);
};


#endif