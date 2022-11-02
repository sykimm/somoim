#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;
#include "manager.h"
// #include "member.h"


bool Manager::join(int sd){ //회원가입
    string id;
    int n;
    char buf[1024];
    sprintf(buf,"%s", "ID 입력>>");
    send(sd, buf, strlen(buf), 0);
    n = recv(sd, buf, sizeof(buf), 0);
    buf[n] = '\0';
    id = buf;
    
    Member* m = new Member(id);
    memArr.push_back(m);

    vector<Club*> v;
    memMap.insert(make_pair(m->getId(), v));
    return true;
}

bool Manager::login(int sd, Member& m){
    string id;
    int n;
    char buf[1024];
    sprintf(buf,"%s", "ID 입력>>");
    send(sd, buf, strlen(buf), 0);
    n = recv(sd, buf, sizeof(buf), 0);
    buf[n] = '\0';
    id = buf;

    for(auto it=memArr.begin(); it!=memArr.end(); it++){
        if ((*it)->getId() == id){
            m = **it;
            return true;
        }
    }
    return false;
}

void Manager::clubPage(vector<Club*>::iterator it){
    int id = (*it)->getId();
    vector<Member*> memlist = clubMap.at(id);
    cout << "---" << (*it)->getName() << "---" << endl;
    cout << (*it)->getDesc() << endl;
    cout << "1. 모임멤버 (" << memlist.size() << "명)" << endl;
    
}

void Manager::showAllClubs(){
    int c, c2;
    int i = 0;
    auto it=clubArr.begin();

    cout << "---모임 목록---" << endl;
    for(; it!=clubArr.end(); it++){
        cout << ++i << '\t' << (*it)->getName() << '\t' << (*it)->getDesc() << endl;
        //'\t' << (it->second).size() << "명" << 
    }
    cout << "---------------" << endl;
    cout << "1. 모임 선택" << endl;
    cout << "2. 뒤로" << endl;
    cout << "선택>> " << endl;
    cin >> c;

    switch(c){
    case 1:
        cout << "모임 번호 선택 >>" << endl;
        cin >> c2;
        clubPage(it+i);
        break;
    case 2:
        return;
    }
}

void Manager::searchClub(){
    cout << "아직 구현 안함" << endl;
}

void Manager::makeClub(Member& l){
    Club *c = new Club();
    string s;
    cout << "모임 개설을 시작합니다." << endl;
    cout << "1. 모임 이름 >>";
    // cin.getline(s, 20);
    getline(cin, s);
    c->setName(s);
    cout << "2. 모임 설명글 >>";
    // cin.getline(s, 255);
    getline(cin, s);
    c->setDesc(s);
    
    // 1. 전체 모임 리스트에 현 모임 추가
    clubArr.push_back(c);
    // 2. 모임이랑 회원들을 연결해줄 clubMap 해쉬에 모임 넣어주기
    vector<Member*> v = {(Leader*) &l};
    // v.push_back();
    clubMap.insert(make_pair(c->getId(), v));
    // 3. 회원 별로 가입한 모임 저장하는 memMap 해쉬에 추가
    cout << l.getId() << " 찾기" << endl;
    auto it = memMap.find(l.getId());
    if (it != memMap.end()){
        cout << "found " << it->first << endl;
        (it->second).push_back(c);
    }else{
        cout << "not found " << l.getId() << endl;
    }

    cout << "모임 개설을 완료했습니다." << endl;
}

void Manager::showMyClubs(string id){
    try{
        vector<Club*> myclubs = memMap.at(id);
        for(auto club: myclubs){
            cout << club->getName() << '\t' << club->getDesc() << endl;
        }
    }catch(exception &e){
        cout << "가입한 모임이 없습니다" << endl;
    }
    
}