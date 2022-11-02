#ifndef _MEMBER_H
#define _MEMBER_H

#include "club.h"


// 일반 멤버였다가 모임을 개설해서 리더가 되는 경우,
// 리더가 탈퇴등의 이유로 운영진이 리더가 되는 경우 어떡하지?

class Member{
protected:
    string id; // 이름이나 쓰게
    // vector<Club*> myClubs;
    vector<string> srchList;
public:
    Member(){ }
    Member(string id){
        this->id = id;
    }
    virtual ~Member() {}
    string getId() { return id; }
    bool addPost();
    bool addReply(int pid);
    virtual bool delPost(int pid) { return true; }
    virtual bool delReply(int pid) { return true; }
    bool like(int pid); // 포스트에 좋아요 눌러줌
    bool unlike(int pid); // 포스트에 좋아요 해제
    virtual string type() { return "member"; } // member/manager/leader 반환
    // void showMyClubs();
};


#endif