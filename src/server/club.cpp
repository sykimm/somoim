#include <iostream>
#include <unistd.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;
#include "club.h"   


Club::Club(){ 
    char cwd[512], clubdir[1024];
    int status;
    cid = n++; 
    
    getcwd(cwd, sizeof(cwd));
    sprintf(clubdir, "%s/database/%s", cwd, to_string(cid).c_str());
    status = mkdir(clubdir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

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
    sendMsg(sd, ">> 게시글 등록이 완료되었습니다\n");
}


void Club::showPostList(int sd){
    showArrList(sd, postArr);
}

void Club::showArrList(int sd, vector<Post*> arr){
    int i = 0;
    char buf[200];
    sendMsg(sd, "------게시판 목록------\n");
    for (Post* p: arr){
        sprintf(buf, "%d>> %s\n", ++i, p->getTitle().c_str());
        sendMsg(sd, buf);
    }
    sendMsg(sd, "-----------------------\n");
}

void Club::postPage(int sd){
    char buf[2048];
    char choice[2];
    int menu;

    sendMsg(sd, "글번호 선택>> ");
    recvMsg(sd, buf);
    int n = atoi(buf);
    
    Post* p = postArr[n-1];
    while(1){
        sendMsg(sd, "----------게시글-----------\n");
        sprintf(buf, "(1) 제목>> %s\n(2) 작성자>> %s\n(3) 내용>> %s\n", 
            p->getTitle().c_str(),
            p->getWriter().c_str(),
            p->getContent().c_str()
        );
        sendMsg(sd, buf);
        p->showReplys(sd);
        sendMsg(sd, "---------------------------\n");

        while (1){
            sendMsg(sd, "[1] 댓글달기\n");
            sendMsg(sd, "[2] 목록으로\n");
            recvMsg(sd, choice);
            if(choice[0]=='\n')
                continue;
            else
                break;
        }       
        menu = atoi(choice);
        switch (menu)
        {
        case 1: // 댓글달기
            p->addReply(sd);
            break;
        case 2: // 나가기
            return;
        default:
            sendMsg(sd, "잘못입력했습니다\n");
        }
    }
    
}

void Club::delPost(int sd){
    char choice[2];

    while (1){
        sendMsg(sd, "삭제할 글 번호>> ");
        recvMsg(sd, choice);
        if(choice[0]=='\n')
            continue;
        else
            break;
    }
    int pid = atoi(choice);
    postArr.erase(postArr.begin()+pid-1);

    sendMsg(sd, "삭제되었습니다.\n");
}

void Club::searchPost(int sd){
    char buf[1024];
    size_t found;
    vector<Post*> srchPost;

    sendMsg(sd, "검색어>> ");
    recvMsg(sd, buf);
    

    for(Post* p: postArr){
        found = p->getTitle().find(buf);
        if (found != string::npos){
            srchPost.push_back(p);
        }
    }

    showArrList(sd, srchPost);
}


void Club::boardPage(int sd, string mid){
    int n, menu;
    char choice[2];

    while (1){
        showPostList(sd);
        while (1){
            sendMsg(sd, "[1] 글보기\n");
            sendMsg(sd, "[2] 글쓰기\n");
            sendMsg(sd, "[3] 글삭제\n");
            sendMsg(sd, "[4] 제목검색\n");
            sendMsg(sd, "[5] 나가기\n");
            recvMsg(sd, choice);
            if(choice[0]=='\n')
                continue;
            else
                break;
        }       
        menu = atoi(choice);
        switch (menu)
        {
        case 1: // 글보기
            postPage(sd);
            break;
        case 2: // 글쓰기
            addPost(sd, mid);
            break;
        case 3: // 글삭제
            delPost(sd);
            break;
        case 4: // 검색
            searchPost(sd);
            break;
        case 5: // 나가기
            return;
            break;
        default:
            sendMsg(sd, ">> 잘못입력했습니다\n");
            break;
        }
    }
}


void Club::showArchive(){
    char cwd[512], ard[1024];
    getcwd(cwd, sizeof(cwd));
    sprintf(ard, "%s/database/%s", cwd, to_string(cid).c_str());
    int status = mkdir("/tmp/a/b/c", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(ard)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            printf ("%s\n", ent->d_name);
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror ("opendir");
    }
}