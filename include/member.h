#ifndef _MEMBER_H
#define _MEMBER_H
#include <set>
using namespace std;


// 일반 멤버였다가 모임을 개설해서 리더가 되는 경우,
// 리더가 탈퇴등의 이유로 운영진이 리더가 되는 경우 어떡하지?

class Member{
protected:
    string id; // 이름이나 쓰게
    string passwd;
    string name;
    string phoneNo; 
    set<int> clubSet; // 가입한 모임id 리스트
public:
    Member(){ 
        this->id = "";
        this->passwd = "";
        this->name = "";
        this->phoneNo = "";
    }
    Member(string id, string pw, string name, string phoneNo){
        this->id = id;
        this->passwd = pw;
        this->name = name;
        this->phoneNo = phoneNo;
    }
    virtual ~Member() {}
    string getId() { return id; }
    string getPW() { return passwd; }
    string getName() { return name; }
    string getphoneNo() { return phoneNo; }
    void addClub(int cid){
        clubSet.insert(cid);
    }
    virtual string type() { return "member"; } // member/manager/leader 반환
    set<int> getMyCid() { return clubSet; }
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(id, passwd, name, phoneNo); // serialize things by passing them to the archive
    }
};


#endif