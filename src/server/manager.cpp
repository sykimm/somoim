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
    char id[10], pw[10];

    sendMsg(sd, "ID 입력>>");
    recvMsg(sd, id);

    sendMsg(sd, "PW 입력>>");
    recvMsg(sd, pw);

    Member* m = new Member(id, pw);
    memArr.push_back(m);

    return true;
}

ssize_t getpasswd (char **pw, size_t sz, int mask, FILE *fp)
{
    if (!pw || !sz || !fp) return -1;       /* validate input   */
#ifdef MAXPW
    if (sz > MAXPW) sz = MAXPW;
#endif

    if (*pw == NULL) {              /* reallocate if no address */
        void *tmp = realloc (*pw, sz * sizeof **pw);
        if (!tmp)
            return -1;
        memset (tmp, 0, sz);    /* initialize memory to 0   */
        *pw =  (char*) tmp;
    }

    size_t idx = 0;         /* index, number of chars in read   */
    int c = 0;

    struct termios old_kbd_mode;    /* orig keyboard settings   */
    struct termios new_kbd_mode;

    if (tcgetattr (0, &old_kbd_mode)) { /* save orig settings   */
        fprintf (stderr, "%s() error: tcgetattr failed.\n", __func__);
        return -1;
    }   /* copy old to new */
    memcpy (&new_kbd_mode, &old_kbd_mode, sizeof(struct termios));

    new_kbd_mode.c_lflag &= ~(ICANON | ECHO);  /* new kbd flags */
    new_kbd_mode.c_cc[VTIME] = 0;
    new_kbd_mode.c_cc[VMIN] = 1;
    if (tcsetattr (0, TCSANOW, &new_kbd_mode)) {
        fprintf (stderr, "%s() error: tcsetattr failed.\n", __func__);
        return -1;
    }

    /* read chars from fp, mask if valid char specified */
    while (((c = fgetc (fp)) != '\n' && c != EOF && idx < sz - 1) ||
            (idx == sz - 1 && c == 127))
    {
        if (c != 127) {
            if (31 < mask && mask < 127)    /* valid ascii char */
                fputc (mask, stdout);
            (*pw)[idx++] = c;
        }
        else if (idx > 0) {         /* handle backspace (del)   */
            if (31 < mask && mask < 127) {
                fputc (0x8, stdout);
                fputc (' ', stdout);
                fputc (0x8, stdout);
            }
            (*pw)[--idx] = 0;
        }
    }
    (*pw)[idx] = 0; /* null-terminate   */

    /* reset original keyboard  */
    if (tcsetattr (0, TCSANOW, &old_kbd_mode)) {
        fprintf (stderr, "%s() error: tcsetattr failed.\n", __func__);
        return -1;
    }

    if (idx == sz - 1 && c != '\n') /* warn if pw truncated */
        fprintf (stderr, " (%s() warning: truncated at %zu chars.)\n",
                __func__, sz - 1);

    return idx; /* number of chars in passwd    */
}

bool Manager::login(int sd, Member& m){
    int n;
    char id[10], password[10], c;
    char* p = password;

    sendMsg(sd, "ID 입력>>");
    recvMsg(sd, id);

    sendMsg(sd, "비번 입력>>");
    // getpasswd(&p, 10, '*', stdin);
    
    for(Member* mptr: memArr){
        if (mptr->getId() == id){ // && mptr->getPW() == password
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
                clubPtr->boardPage(sd, m.getId());
                break;
            case 2:
                enterChat(cid, sd);
                chatPage(cid, sd, m);
                exitChat(cid, sd);
                break;
            case 3:
                cout << "자료실 기능 구현하기" << endl;
                clubPtr->showArchive(sd);
                break;
            case 4:
                return;
            default:
                sendMsg(sd, "잘못 선택하셨습니다.\n");
            }
        }else{
            switch(menu){
            case 1:
                joinClub(clubPtr, m);
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
    map<int, vector<int>>::iterator it = chatInfo.begin();
    
    for(auto fd: it->second){
        sprintf(idbuf, "%s님이 채팅창에 입장하셨습니다.\n", m.getId().c_str());
        if(send(fd, idbuf, sizeof(idbuf), 0) == -1)
            perror("send");
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
        }
    }

    for(auto fd: it->second){
        sprintf(idbuf, "%s님이 퇴장하셨습니다.\n", m.getId().c_str());
        if(send(fd, idbuf, sizeof(idbuf), 0) == -1)
            perror("send");
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
            clubPage(sd, *(clubArr.begin()+menu2-1), m);
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
    sendMsg(sd, "모임 개설을 시작합니다\n");
    sendMsg(sd, "1. 모임 이름 >>");
    recvMsg(sd, name);
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
    auto cpiter = clubArr.begin(); // Club Pointer Iterator
    char buf[1024];
    int i = 0;

    for(int myClub :m.getMyCid()){
        while ((*cpiter)->getId() != myClub){
            cpiter++;
        }
        // myClub이랑 같은것일때 빠져나와서 출력
        sprintf(buf, "%d\t%s\t%s\n", ++i, (*cpiter)->getName().c_str(), (*cpiter)->getDesc().c_str());
        sendMsg(sd, buf);
    }
}