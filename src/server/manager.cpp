#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <algorithm>
using namespace std;
#include "manager.h"
// #include "member.h"





bool Manager::join(int sd){ //회원가입
    string id;
    int n;
    char buf[1024];
    sprintf(buf,"%s", "ID 입력>>");
    send(sd, buf, strlen(buf), 0);
    // n = recv(sd, buf, sizeof(buf), 0);
    // buf[n] = '\0';
    // id = buf;
    id = recvMsg(sd, buf);
    
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
    // n = recv(sd, buf, sizeof(buf), 0);
    // buf[n] = '\0';
    // id = buf;
    id = recvMsg(sd, buf);

    for(auto it=memArr.begin(); it!=memArr.end(); it++){
        if ((*it)->getId() == id){
            m = **it;
            return true;
        }
    }
    return false;
}

bool Manager::checkIsMember(int cid, string mid){
    bool isMember = false;
    auto clubs = memMap.find(mid);
    if (clubs != memMap.end()){
        for (auto cptr: clubs->second){
            if (cptr->getId() == cid){
                isMember = true;
                break;
            }
        }
    }
    return isMember;
}

void Manager::clubPage(int sd, vector<Club*>::iterator it, Member& m){
    char choice[2];
    int n, menu;
    int cid = (*it)->getId();
    bool isMember;
    vector<Member*> memlist = clubMap.at(cid);

    char buf[1024];

    while (1){
        sprintf(buf, "---%s 모임페이지(%ld명)---\n", (*it)->getName().c_str(), memlist.size());
        sendMsg(sd, buf);
        sprintf(buf, "%s\n", (*it)->getDesc().c_str());
        sendMsg(sd, buf);
        if (isMember=checkIsMember(cid, m.getId())){
            sendMsg(sd, "[1] 게시판\n");
            sendMsg(sd, "[2] 채팅창\n");
            sendMsg(sd, "[3] 나가기\n");
        }else{
            sendMsg(sd, "[1] 가입하기\n");
            sendMsg(sd, "[2] 나가기\n");
        }

        // n = recv(sd, choice, sizeof(choice), 0);
        // choice[n]='\0';
        recvMsg(sd, choice);
        if(choice[0]=='\n')
            continue;
        menu = atoi(choice);

        if (isMember){
            switch(menu){
            case 1:
                boardPage((*it), sd, m);
                break;
            case 2:
                enterChat(m.getId(), cid, sd);
                chatPage(cid, sd, m);
                exitChat(m.getId(), cid, sd);
                break;
            case 3:
                return;
            default:
                sendMsg(sd, "잘못 선택하셨습니다.\n");
            }
        }else{
            switch(menu){
            case 1:
                joinClub((*it), m);
                // enterChat(m.getId(), sd);
                break;
            case 2:
                return;
            default:
                sendMsg(sd, "잘못 선택하셨습니다.\n");
            }
        }
        
    }
    
}

void Manager::chatPage(int clubId, int sd, Member& m){
    int n;
    char recvbuf[100], idbuf[150];
    map<int, vector<int>>::iterator it;
    
    
    while (1){
        it = chatInfo.find(clubId);
        cout << "chatPage "<< endl;
        for (int fd: it->second){
            cout << fd << ", ";
        }
        cout << endl;

        if (it == chatInfo.end()){
            cout << "해당 club은 chatInfo에 등록되지 않음" << endl;
            return;
        }
        // 읽은것
        if((n = read(sd, recvbuf, sizeof(recvbuf))) == -1){
            perror("read");
        }
        if (strcmp(recvbuf, "exit") == 0)
            break;
        cout << "n bytes: " << n << endl;
        *(recvbuf+n-1) = '\n';
        *(recvbuf+n) = '\0';

        // 전체에 뿌리기
        for(auto fd: it->second){
            if (fd == sd) continue;
            sprintf(idbuf, "[%s] %s", m.getId().c_str(), recvbuf);
            if(send(fd, idbuf, sizeof(idbuf), 0) == -1)
                perror("send");
            // if(send(fd, recvbuf, sizeof(recvbuf), 0) == -1)
            //     perror("send");
        }
    }
        
    
}

void Manager::boardPage(Club* club, int sd, Member& m){
    int n, menu;
    char choice[2];

    club->showAllPost(sd);
    while (1){
        sendMsg(sd, "[1] 글보기\n");
        sendMsg(sd, "[2] 글쓰기\n");
        sendMsg(sd, "[3] 나가기\n");
        recvMsg(sd, choice);
        if(choice[0]=='\n')
            continue;
        else
            break;
    }       
    menu = atoi(choice);
    switch (menu)
    {
    case 1:
        cout << "글번호 입력해서 보여주는 함수 만들기" << endl;
        break;
    case 2:
        club->addPost(sd, m.getId());
        break;
    default:
        cout << "보여주기" << endl;
        break;
    }
}

void Manager::showAllClubs(int sd, Member& m){
    int menu, menu2, n;
    int i = 0;
    vector<Club*>::iterator it;
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
            // n = recv(sd, choice, sizeof(choice), 0);
            // choice[n]='\0';
            recvMsg(sd, choice);
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
            // n = recv(sd, choice, sizeof(choice), 0);
            // choice[n]='\0';
            recvMsg(sd, choice);
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
    // n = recv(sd, name, sizeof(name), 0);
    // name[n]='\0';
    recvMsg(sd, name);
    c->setName(name);
    sendMsg(sd, "2. 모임 설명글 >>");
    // n = recv(sd, desc, sizeof(desc), 0);
    // desc[n]='\0';
    recvMsg(sd, desc);
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

/// @brief 이미 만들어진 모임에 멤버가 추가되는 경우
/// @param c 
/// @param m 
bool Manager::joinClub(Club* c, Member& m){
    // 1. clubMap에 사람 추가하기
    // 이미 있는 모임이니까 clubMap에 모임id 이미 등록되어 있음
    auto it = clubMap.find(c->getId()); // vector<Member*>를 가리킴
    if (it != clubMap.end()){
        (it->second).push_back(&m);
    }else{
        cout << "clubMap에 등록되지 않은 club id 참조" << endl;
        return false;
    }
    // 2. memMap에 모임 추가
    auto it2 = memMap.find(m.getId());
    if (it2 != memMap.end()){
        (it2->second).push_back(c);
    }else{
        cout << "memMap에 등록되지 않은 member id 참조" << endl;
        return false;
    }
    return true;
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

// void Manager::enterChat(string mid, int sd){
//     map<int, vector<int>>::iterator it;
//     auto joinedClubIter = memMap.find(mid); // 이 멤버가 가입한 클럽iterator
//     if (joinedClubIter == memMap.end()){
//         // 가입한 모임이 없다면 이 함수가 불려서도 안됨
//         cout << "가입한 모임이 없다면 이 함수가 불려서도 안됨" << endl;
//         return;
//     }
//     for (auto cptr: joinedClubIter->second){
//         it = chatInfo.find(cptr->getId());
//         if (it == chatInfo.end()){
//             // 1. 처음 개설해서 key값이 없는경우    
//             vector<int> v = {sd};
//             chatInfo.insert(make_pair(cptr->getId(), v));
//             cout << sd << "추가됨" << endl;
//         }else{
//             // 2. 일반회원들이 가입하는 경우
//             (it->second).push_back(sd);
//             cout << sd << "추가됨" << endl;
//         }
        
//     }
// }

// void Manager::exitChat(string mid, int sd){
//     vector<int> joinedClubs;
//     auto it = memMap.find(mid);
    
//     if (it != memMap.end()){
//         for (auto club: it->second){
//             joinedClubs = chatInfo.at(club->getId());
//             joinedClubs.erase(
//                 remove(joinedClubs.begin(), joinedClubs.end(), sd),
//                 joinedClubs.end()
//             );
//             cout << sd << " 제거됨" << endl;
//         }
//     }
// }

void Manager::enterChat(string mid, int cid, int sd){
    map<int, vector<int>>::iterator it;
    auto joinedClubIter = memMap.find(mid); // 이 멤버가 가입한 클럽iterator
    if (joinedClubIter == memMap.end()){
        // 가입한 모임이 없다면 이 함수가 불려서도 안됨
        cout << "가입한 모임이 없다면 이 함수가 불려서도 안됨" << endl;
        return;
    }
    
    for (auto cptr: joinedClubIter->second){
        if (cptr->getId() != cid) // 해당 클럽에 대한 채팅정보만 알고싶다
            continue;

        it = chatInfo.find(cid);
        if (it == chatInfo.end()){
            // 1. 클럽 채팅창 아무도없다가 처음 개설해서 key값이 없는경우    
            vector<int> v = {sd};
            chatInfo.insert(make_pair(cid, v));
            cout << sd << "추가됨" << endl;
        }else{
            // 2. 채팅창에 2명 이상째 들어오는 경우
            (it->second).push_back(sd);
            cout << sd << "추가됨" << endl;
        }
        break;
    }
}

void Manager::exitChat(string mid, int cid, int sd){
    vector<int> joinedClubs;
    auto it = memMap.find(mid);
    
    if (it != memMap.end()){
        for (auto club: it->second){
            if (club->getId() != cid) // 해당 클럽에 대한 채팅정보만 알고싶다
                continue;

            joinedClubs = chatInfo.at(club->getId());
            joinedClubs.erase(
                remove(joinedClubs.begin(), joinedClubs.end(), sd),
                joinedClubs.end()
            );
            cout << sd << " 제거됨" << endl;
            for (int fd: joinedClubs){
                cout << fd << ", ";
            }
            chatInfo.at(club->getId()) = joinedClubs;
            cout << endl;
            break;
        }
    }
}

