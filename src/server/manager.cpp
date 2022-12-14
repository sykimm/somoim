#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <algorithm> 
#include <termios.h>
#include <stdio.h>
using namespace std;
#include "manager.h"



bool Manager::join(int sd){ //회원가입
    int n;
    char id[10], pw[10], name[10], phoneNo[20];

    sendMsg(sd, "1) ID 입력>>");
    recvMsg(sd, id);

    sendMsg(sd, "2) PW 입력>>");
    recvMsg(sd, pw);

    sendMsg(sd, "3) 이름 입력>>");
    recvMsg(sd, name);

    sendMsg(sd, "4) 전화번호 입력>>");
    recvMsg(sd, phoneNo);

    Member* m = new Member(id, pw, name, phoneNo);
    memArr.push_back(m);

    return true;
}


string getpasswd(int sd)
{   
    sendMsg(sd, "pw");
    usleep(2 * 1000);
    char passwd[20];
    // char c[1];
    
    recvMsg(sd, passwd);
    return passwd;
}

bool Manager::login(int sd, Member& m){
    int n;
    char id[10], c;
    string password;

    sendMsg(sd, "ID 입력>>");
    recvMsg(sd, id);

    sendMsg(sd, "비번 입력>>");
    usleep(2 * 1000);
    password = getpasswd(sd);
    
    for(Member* mptr: memArr){
        if (mptr->getId() == id && mptr->getPW() == password){ //  
            m = *mptr;
            return true;
        }
    }
    return false;
}


void Manager::clubPage(int sd, Club* clubPtr, Member& m){
    char choice[2];
    int n, menu;
    int cid = clubPtr->getId();
    bool isMember;

    char buf[1024];

    while (1){
        sendMsg(sd, "clear");
        sprintf(buf, "---%s 모임페이지(%d명)---\n", clubPtr->getName().c_str(), clubPtr->getTotalNo());
        sendMsg(sd, buf);
        sprintf(buf, "소개: %s\n", clubPtr->getDesc().c_str());
        sendMsg(sd, buf);
        if (isMember=clubPtr->isMember(m.getId())){
            sendMsg(sd, "[1] 게시판\n");
            sendMsg(sd, "[2] 채팅창\n");
            sendMsg(sd, "[3] 자료실\n");
            sendMsg(sd, "[4] 나가기\n");
        }else{
            sendMsg(sd, "[1] 가입하기\n");
            sendMsg(sd, "[2] 나가기\n");
        }

        recvMsg(sd, choice);
        if(choice[0]=='\n')
            continue;
        menu = atoi(choice);

        if (isMember){
            switch(menu){
            case 1:
                sendMsg(sd, "clear"); usleep(2000);
                clubPtr->boardPage(sd, m.getId());
                break;
            case 2:
                sendMsg(sd, "clear"); usleep(2000);
                enterChat(cid, sd);
                chatPage(cid, sd, m);
                exitChat(cid, sd);
                break;
            case 3:
                sendMsg(sd, "clear"); usleep(2000);
                clubPtr->showArchive(sd);
                break;
            case 4:
                sendMsg(sd, "clear"); usleep(2000);
                return;
            default:
                sendMsg(sd, "clear"); usleep(2000);
                sendMsg(sd, "잘못 선택하셨습니다.\n");
            }
        }else{
            switch(menu){
            case 1:
                sendMsg(sd, "clear"); usleep(2000);
                joinClub(clubPtr, m);
                break;
            case 2:
                sendMsg(sd, "clear"); usleep(2000);
                return;
            default:
                sendMsg(sd, "clear"); usleep(2000);
                sendMsg(sd, "잘못 선택하셨습니다.\n");
            }
        }
        
    }
    
}

void Manager::chatPage(int clubId, int sd, Member& m){
    int n;
    char recvbuf[100], idbuf[150];
    map<int, vector<int>>::iterator it = chatInfo.begin();
    
    for(auto fd: it->second){
        sprintf(idbuf, "%s님이 채팅창에 입장하셨습니다.\n", m.getId().c_str());
        if(send(fd, idbuf, sizeof(idbuf), 0) == -1)
            perror("send");
        usleep(2000);
    }

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
        
        cout << "before (B):" << recvbuf <<"(" << n << ")" << endl;
        *(recvbuf+n) = '\n';
        *(recvbuf+n+1) = '\0';
        if (strcmp(recvbuf, "exit\n") == 0)
            break;
        cout << "after :" << recvbuf << endl;

        // 전체에 뿌리기
        for(auto fd: it->second){
            if (fd == sd) continue;
            
            sprintf(idbuf, "[%s] %s", m.getId().c_str(), recvbuf);
            if(send(fd, idbuf, sizeof(idbuf), 0) == -1){
                perror("send");
            }
            usleep(2000);
        }
    }

    for(auto fd: it->second){
        if (fd == sd){
            sendMsg(sd, "clear");
        }
        sprintf(idbuf, "%s님이 퇴장하셨습니다.\n", m.getId().c_str());
        if(send(fd, idbuf, sizeof(idbuf), 0) == -1)
            perror("send");
    }
}


void Manager::showAllClubs(int sd, Member& m){
    int menu, menu2, n;
    int i;
    vector<Club*>::iterator it;
    char buf[1024];
    char choice[2];

    while (1){
        i = 0;
        sendMsg(sd, "---모임 목록---\n");
        for(it=clubArr.begin(); it!=clubArr.end(); it++){
            sprintf(buf, "%d\t%s\t%s\n", ++i, (*it)->getName().c_str(), (*it)->getDesc().c_str());
            sendMsg(sd, buf);
        }

        sendMsg(sd, "---------------\n");
        sendMsg(sd, "[1] 모임 선택\n");
        sendMsg(sd, "[2] 뒤로\n");
        
        while (1){
            sendMsg(sd, "선택>> ");
            recvMsg(sd, choice);
            if(choice[0]=='\n')
                continue;
            else
                break;
        }

        menu = atoi(choice);
        switch(menu){
        case 1:
            sendMsg(sd, "모임 번호 선택 >>");
            recvMsg(sd, choice);
            menu2 = atoi(choice);
            if (menu2 > clubArr.size()){
                sendMsg(sd, "clear"); 
                sendMsg(sd, "현재 모임이 없습니다.\n");
                break;
            }
            clubPage(sd, *(clubArr.begin()+menu2-1), m);
            break;
        case 2: // 뒤로
            sendMsg(sd, "clear");
            return;
        }
    }
    
}

void Manager::makeClub(int sd, Member& l){
    int n;
    char name[32];
    char desc[1024];

    Club *c = new Club();
    sendMsg(sd, "모임 개설을 시작합니다\n");
    sendMsg(sd, "1. 모임 이름 >>");
    recvMsg(sd, name);
    cout << name << endl;
    c->setName(name);
    sendMsg(sd, "2. 모임 설명글 >>");
    recvMsg(sd, desc);
    c->setDesc(desc);
    c->addMember((Leader*) &l);

    clubArr.push_back(c);
    l.addClub(c->getId());

    cout << "모임 개설을 완료했습니다." << endl;
}

/// @brief 이미 만들어진 모임에 멤버가 추가되는 경우
bool Manager::joinClub(Club* c, Member& m){
    // 1. clubMap에 사람 추가하기
    // 이미 있는 모임이니까 clubMap에 모임id 이미 등록되어 있음
    c->addMember(&m);
    // 2. memMap에 모임 추가
    m.addClub(c->getId());
    return true;
}



void Manager::enterChat(int cid, int sd){
    auto it = chatInfo.find(cid);
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
}

void Manager::exitChat(int cid, int sd){
    vector<int> joinedClubs = chatInfo.at(cid);
    joinedClubs.erase(
        remove(joinedClubs.begin(), joinedClubs.end(), sd),
        joinedClubs.end()
    );
    chatInfo.at(cid) = joinedClubs;
}

void Manager::showMyClubs(Member& m, int sd){
    vector<Club *>::iterator cpiter = clubArr.begin(); // Club Pointer Iterator
    vector<vector<Club *>::iterator> myclub;
    char buf[1024], choice[2];
    int i;
    int menu, menu2;

    while (1){
        i = 0;
        sendMsg(sd, "---내모임---\n");
        for(int myClub :m.getMyCid()){
            while ((*cpiter)->getId() != myClub){
                cpiter++;
            }
            // myClub이랑 같은것일때 빠져나와서 출력
            myclub.push_back(cpiter);
            sprintf(buf, "%d\t%s\t%s\n", ++i, (*cpiter)->getName().c_str(), (*cpiter)->getDesc().c_str());
            sendMsg(sd, buf);
        }
        sendMsg(sd, "---------------\n");
        sendMsg(sd, "[1] 모임 선택\n");
        sendMsg(sd, "[2] 뒤로\n");

        while (1){
            sendMsg(sd, "선택>> ");
            recvMsg(sd, choice);
            if(choice[0]=='\n')
                continue;
            else
                break;
        }

        menu = atoi(choice);
        switch(menu){
        case 1:
            sendMsg(sd, "모임 번호 선택 >>");
            recvMsg(sd, choice);
            menu2 = atoi(choice); // 1~n까지 size=n일때
            if (menu2 > myclub.size()){
                sendMsg(sd, "clear"); 
                sendMsg(sd, "현재 모임이 없습니다.\n");
                break;
            }
            clubPage(sd, *myclub[menu2-1], m);
            break;
        case 2: // 뒤로
            sendMsg(sd, "clear");
            return;
        }

    }
}


void Manager::loadData(ifstream& fin){
    string line;
	vector<string> v;
    
    
    while(getline(fin, line)){		
        v = parseLine(line);
        memArr.push_back(new Member(v[0], v[1], v[2], v[3]));
    }
    
}


void Manager::saveData(){
    FILE *fp;
    fp = fopen("userinfo.txt", "w");

    for(Member* m: memArr){
        fputs(m->getId().c_str(), fp);
        fputs(",", fp);
        fputs(m->getPW().c_str(), fp);
        fputs(",", fp);
        fputs(m->getName().c_str(), fp);
        fputs(",", fp);
        fputs(m->getphoneNo().c_str(), fp);
        fputs("\n", fp);
    }
    fclose(fp);

    // FILE *fp2;
    // fp2 = fopen("clubInfo.txt", "w");
    // for(Club* m: clubArr){
    //     fputs(to_string(m->getId()).c_str(), fp);
    //     fputs(",", fp);
    //     fputs(m->getName().c_str(), fp);
    //     fputs(",", fp);
    //     fputs(m->getDesc().c_str(), fp);
    //     fputs(",", fp);
    //     fputs(m->getphoneNo().c_str(), fp);
    //     fputs("\n", fp);
    // }
    // fclose(fp2);
}

