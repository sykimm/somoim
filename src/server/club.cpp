#include <iostream>
#include <unistd.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <vector>
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

void Club::download(int sd){
    char cwd[512], archive[1024], buf[1024], fullname[2048], fbuf[256], clientDir[2048];
    DIR *dir;
    struct dirent *ent;
    int i, n;
    char choice[2];
    int menu;
    vector<string> fileList;
    FILE* file;
    size_t size;
    int nsize, fpsize;
    unsigned long fsize;
    int pid;

    getcwd(cwd, sizeof(cwd)); // 서버에 있는 db 위치
    sprintf(archive, "%s/database/%s", cwd, to_string(cid).c_str()); // 서버에 있는 이 모임에 대한 db 위치
    

    while (1){
        i = 0;
        sendMsg(sd, "다운로드 받을 파일번호 선택(나가기: -1)\n");
        fileList.clear();
        dir = opendir(archive);
        if (dir != NULL) {
            while (1) {
                ent = readdir(dir);
                if (ent == NULL){
                    break;
                }else{
                    if (ent->d_type == DT_REG){
                        sprintf(buf, "%d] %s\n", ++i, ent->d_name);
                        sendMsg(sd, buf);
                        fileList.push_back(ent->d_name);
                    }
                }
            }
            closedir(dir);
        }else{ /* could not open directory */
            perror ("opendir");
        }

        recvMsg(sd, choice);
        if(choice[0]=='\n')
            continue;
        menu = atoi(choice);
        if (menu == -1)
            return;
        else if (menu >= 0 && menu <= fileList.size()){
            // sprintf(clientDir, "TrAnSfEr %s", ent->d_name);
            pid = fork();
            if (pid == -1)
                perror("fork()");
            else if (pid == 0){ // child

            }else{ // 부모
                while (1){
					ret = waitpid(pid, &status, WNOHANG);
					if(ret == 0){ // 2. 변화 없는 경우
						continue;
					}else if (ret == -1){ // 2. 에러
						perror("waitpid");
					}else{ // 자식이끝나서 pid 반환하는 경우
						break;
					}
				}
            }

            cout << "TrAnSfEr" << "send!!" << endl;

            
            sendMsg(sd, "TrAnSfEr");
            cout << ent->d_name << "send!!" << endl;
            sendMsg(sd, ent->d_name);
            
            // n = recv(sd, clientDir, sizeof(clientDir), 0);
            // clientDir[n] = '\0';
            // cout << "clientDir: " << clientDir << endl;

            sprintf(fullname, "%s/%s", archive, ent->d_name);
            file = fopen(fullname, "rb"); // binary로 open
            
            if (file != NULL){
                fseek(file, 0, SEEK_END); //파일포인터 끝으로 이동해서
                fsize = ftell(file); // 파일 크기 계산
                fseek(file, 0, SEEK_SET); // 파일포인터 처음으로 이동
                size = htonl(fsize); // from host byte order to network byte order.
                // send(sd, &size, sizeof(fsize), 0); // 파일크기 전송
                nsize = 0;
                while (nsize != fsize){
                    fpsize = fread(fbuf, 1, 256, file); // 256씩읽어서 fbuf에 뒀다가
                    nsize += fpsize;
                    send(sd, fbuf, fpsize, 0);
                }
                // fread(fbuf, sizeof(char), fsize, file);
                // send(sd, fbuf, fsize, 0);
                fclose(file);
            }

            
        }
        else{
            sendMsg(sd, "잘못 선택했습니다\n");
        }
    }


}


void Club::showArchive(int sd){
    
    char choice[2], buf[1024];
    int menu;

    while (1){
        sprintf(buf, "---%s 자료실 페이지---\n", clubName.c_str());
        sendMsg(sd, buf);
        sendMsg(sd, "[1] 자료 업로드\n");
        sendMsg(sd, "[2] 자료 다운로드\n");
        sendMsg(sd, "[3] 나가기\n");

        recvMsg(sd, choice);
        if(choice[0]=='\n')
            continue;
        menu = atoi(choice);

        switch(menu){
        case 1:
            upload(sd);
            break;
        case 2:
            download(sd);
            break;
        case 3:
            return;
            break;
        default:
            sendMsg(sd, "잘못 선택하셨습니다.\n");
            break;
        }
    }



    
    
    
}