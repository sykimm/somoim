#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;
#include "manager.h"
// #include "member.h"



void sendMsg(int sd, string s){
	send(sd, s.c_str(), s.size(), 0);
}



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
    sendMsg(sd, "ID 입력>>");
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

void Manager::clubPage(int sd, vector<Club*>::iterator it, Member& m){
    char choice[2];
    int n, menu;
    int id = (*it)->getId();
    vector<Member*> memlist = clubMap.at(id);

    char buf[1024];
    bool isMember = false;
    if (memMap.find(m.getId()) != memMap.end()){
        // 가입하지 않은 모임
        isMember = true;
    }
    

    while (1){
        sprintf(buf, "---%s---\n", (*it)->getName().c_str());
        sendMsg(sd, buf);
        sprintf(buf, "%s\n", (*it)->getDesc().c_str());
        sendMsg(sd, buf);
        sprintf(buf, "- 모임멤버 (%ld명)\n", memlist.size());
        sendMsg(sd, buf);
        if (isMember){
            sendMsg(sd, "[1] 게시판\n");
            sendMsg(sd, "[2] 채팅창\n");
            sendMsg(sd, "[3] 나가기\n");
        }else{
            sendMsg(sd, "[1] 게시판\n");
            sendMsg(sd, "[2] 나가기\n");
        }

        n = recv(sd, choice, sizeof(choice), 0);
        choice[n]='\0';
        if(choice[0]=='\n')
            continue;
        menu = atoi(choice);

        if (isMember){
            switch(menu){
            case 1:
                break;
            case 2:
                chatPage(sd);
                break;
            case 3:
                return;
            default:
                sendMsg(sd, "잘못 선택하셨습니다.\n");
            }
        }else{
            switch(menu){
            case 1:
                break;
            case 2:
                chatPage(sd);
                break;
            case 3:
                return;
            default:
                sendMsg(sd, "잘못 선택하셨습니다.\n");
            }
        }
        
    }
    
}

void Manager::showAllClubs(int sd, Member& m){
    int menu, menu2, n;
    int i = 0;
    auto it;
    char buf[1024];
    char choice[2];

    while (1){
        sendMsg(sd, "---모임 목록---\n");
        for(it=clubArr.begin(); it!=clubArr.end(); it++){
            sprintf(buf, "%d\t%s\t%s\n", ++i, (*it)->getName().c_str(), (*it)->getDesc().c_str());
            sendMsg(sd, buf);
            // cout << ++i << '\t' << (*it)->getName() << '\t' << (*it)->getDesc() << endl;
            //'\t' << (it->second).size() << "명" << 
        }

        sendMsg(sd, "---------------\n");
        sendMsg(sd, "[1] 모임 선택\n");
        sendMsg(sd, "[2] 뒤로\n");
        
        while (1){
            sendMsg(sd, "선택>> ");
            n = recv(sd, choice, sizeof(choice), 0);
            choice[n]='\0';
            if(choice[0]=='\n')
                continue;
            else
                break;
        }       
        menu = atoi(choice);
        // it=clubArr.begin();
        switch(menu){
        case 1:
            sendMsg(sd, "모임 번호 선택 >>");
            n = recv(sd, choice, sizeof(choice), 0);
            choice[n]='\0';
            menu2 = atoi(choice);
            clubPage(sd, clubArr.begin()+menu2-1, m);
            break;
        case 2: // 뒤로
            return;
        }
    }
    
}

void Manager::searchClub(){
    cout << "아직 구현 안함" << endl;
}

void Manager::makeClub(int sd, Member& l){
    int n;
    char name[32];
    char desc[1024];
    Club *c = new Club();
    string s;
    sendMsg(sd, "모임 개설을 시작합니다\n");
    sendMsg(sd, "1. 모임 이름 >>");
    // getline(cin, s);
    n = recv(sd, name, sizeof(name), 0);
    name[n]='\0';
    c->setName(name);
    sendMsg(sd, "2. 모임 설명글 >>");
    n = recv(sd, desc, sizeof(desc), 0);
    desc[n]='\0';
    c->setDesc(desc);
    
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

void Manager::showMyClubs(int sd, string id){
    char buf[1024];
    try{
        vector<Club*> myclubs = memMap.at(id);
        for(auto club: myclubs){
            sprintf(buf, "%s\t%s\n", club->getName().c_str(), club->getDesc().c_str());
            sendMsg(sd, buf);
        }
    }catch(exception &e){
        sendMsg(sd, "가입한 모임이 없습니다\n");
    }
}