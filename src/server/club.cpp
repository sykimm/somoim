#include <iostream>
using namespace std;
#include "club.h"


void Club::addPost(int sd, string mid){
    char title[50], content[100];
    Post *p = new Post(mid);
    sendMsg(sd, "(1) 글 제목>> ");
    recvMsg(sd, title);
    sendMsg(sd, "(2) 내용>> ");
    recvMsg(sd, content);

    p->setTitle(title);
    p->setContent(content);
    postArr.push_back(p);
    sendMsg(sd, ">> 게시글 등록이 완료되었습니다.");
}


void Club::showAllPost(int sd){
    int i = 0;
    char buf[200];
    sendMsg(sd, "------------\n");
    for (Post* p: postArr){
        sprintf(buf, "%d>> %s\n", ++i, p->getTitle());
        sendMsg(sd, buf);
    }
    sendMsg(sd, "------------\n");
}