#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <filesystem>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <vector>
#include <fstream>
#include <cmath>
using namespace std;
#include "club.h"

#define BUFSIZE 256


Club::Club(int cid){ 
    char cwd[512];
    int status;
    if (cid == -1){
        this->cid = n++; 
    }else{
        this->cid = cid;
    }
    
    getcwd(cwd, sizeof(cwd));
    sprintf(archive, "%s/database/%s", cwd, to_string(this->cid).c_str());
    status = mkdir(archive, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    cout << "club archive 생성" << endl;
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
    sendMsg(sd, "clear");
    sendMsg(sd, ">> 게시글 등록이 완료되었습니다\n");
}

void Club::showArrList(int sd, vector<Post*> &arr, string mid){
    int page = 0;
    int idx, no;
    char buf[200], buf2[10];
    vector<Post*>::iterator it = arr.begin();
    int npage;
    
    while(1){
        npage = ceil(arr.size() / 10.);
        sendMsg(sd, "------게시판 목록------\n");
        for (int j=0; j<10; j++){
            idx = page * 10 + j;
            cout << "idx: " << idx << endl;
            if (idx >= arr.size())
                break;
            // sprintf(buf, "%d>> %s\n", idx+1, (*(it+idx))->getTitle().c_str());
            sprintf(buf, "%d>> %s\n", idx+1, arr[idx]->getTitle().c_str());
            // cout << (*(it+idx))->getTitle() << endl;
            sendMsg(sd, buf);
        }
        sendMsg(sd, "-----------------------\n");
        sendMsg(sd, "[1] 이전페이지\n");
        sendMsg(sd, "[2] 다음페이지\n");
        sendMsg(sd, "[3] 글보기\n");
        sendMsg(sd, "[4] 글쓰기\n");
        sendMsg(sd, "[5] 글삭제\n");
        sendMsg(sd, "[6] 제목검색\n");
        sendMsg(sd, "[7] 나가기\n");
        recvMsg(sd, buf);
        if (strcmp(buf, "1") == 0){
            if (page == 0){
                sendMsg(sd, "clear");
                sendMsg(sd, "첫번째 페이지입니다.\n");
            }
            else{
                page--;
                sendMsg(sd, "clear");
            }
        }else if (strcmp(buf, "2") == 0){
            if (page == npage-1){
                sendMsg(sd, "clear");
                sendMsg(sd, "마지막 페이지입니다.\n");
            }else{
                page++;
                sendMsg(sd, "clear");
            }
        }else if (strcmp(buf, "3") == 0){ // 글보기
            sendMsg(sd, ">> 번호 입력\n");
            recvMsg(sd, buf2);
            no = stoi(buf2) - 1;
            sendMsg(sd, "clear");
            showPost(sd, arr[no]);
        }else if (strcmp(buf, "4") == 0){ // 글쓰기
            sendMsg(sd, "clear");
            addPost(sd, mid);
        }
        else if (strcmp(buf, "5") == 0){ // 글삭제
            sendMsg(sd, ">> 삭제할 글번호 입력\n");
            recvMsg(sd, buf2);
            no = stoi(buf2) - 1;
            sendMsg(sd, "clear");
            delPost(sd, arr.begin()+no, mid);
        }else if (strcmp(buf, "6") == 0){ // 제목검색
            sendMsg(sd, "clear");
            searchPost(sd, mid);
        }
        else if (strcmp(buf, "7") == 0){ //나가기
            sendMsg(sd, "clear");
            break;
        }
        else{
            sendMsg(sd, "clear");
            sendMsg(sd, "잘못 선택함\n");
            continue;
        }

    }
}


void showPost(int sd, Post* p){
    char buf[2048];
    char choice[2];
    int menu;
    
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
            sendMsg(sd, "clear");
            return;
        default:
            sendMsg(sd, "clear");
            sendMsg(sd, "잘못입력했습니다\n");
        }
    }
}

void Club::delPost(int sd, vector<Post*>::iterator it, string mid){
    if ((*it)->getWriter() == mid){
        postArr.erase(it-1);
        sendMsg(sd, "삭제되었습니다.\n");
    }else{
        sendMsg(sd, "작성자만 삭제가능합니다.\n");
    }
}

void Club::searchPost(int sd, string mid){
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

    // showArrList(sd, srchPost, mid);
    int page = 0;
    int idx, no;
    char buf2[10];
    vector<Post*>::iterator it = srchPost.begin();
    int npage;
    
    while(1){
        npage = ceil(srchPost.size() / 10.);
        sendMsg(sd, "------검색된 게시판 목록------\n");
        for (int j=0; j<10; j++){
            idx = page * 10 + j;
            cout << "idx: " << idx << endl;
            if (idx >= srchPost.size())
                break;
            sprintf(buf, "%d>> %s\n", idx+1, srchPost[idx]->getTitle().c_str());
            sendMsg(sd, buf);
        }
        sendMsg(sd, "-----------------------\n");
        sendMsg(sd, "[1] 이전페이지\n");
        sendMsg(sd, "[2] 다음페이지\n");
        sendMsg(sd, "[3] 글보기\n");
        sendMsg(sd, "[4] 나가기\n");
        recvMsg(sd, buf);
        if (strcmp(buf, "1") == 0){
            if (page == 0){
                sendMsg(sd, "clear");
                sendMsg(sd, "첫번째 페이지입니다.\n");
            }
            else{
                page--;
                sendMsg(sd, "clear");
            }
        }else if (strcmp(buf, "2") == 0){
            if (page == npage-1){
                sendMsg(sd, "clear");
                sendMsg(sd, "마지막 페이지입니다.\n");
            }else{
                page++;
                sendMsg(sd, "clear");
            }
        }else if (strcmp(buf, "3") == 0){ // 글보기
            sendMsg(sd, ">> 번호 입력\n");
            recvMsg(sd, buf2);
            no = stoi(buf2) - 1;
            sendMsg(sd, "clear");
            showPost(sd, srchPost[no]);
        }
        else if (strcmp(buf, "4") == 0){ //나가기
            sendMsg(sd, "clear");
            break;
        }
        else{
            sendMsg(sd, "잘못 선택함\n");
            continue;
        }
    }
}


void Club::boardPage(int sd, string mid){
    showArrList(sd, postArr, mid);
}

void Club::download(int sd){
    char fullname[2048], fbuf[BUFSIZE];
    DIR *dir;
    struct dirent *ent;
    int i;
    char choice[2];
    int menu;
    vector<string> fileList;
    FILE* file;
    int nsize, fpsize, fsize;
    
    sendMsg(sd, "clear");

    while (1){    
        sendMsg(sd, "다운로드 받을 파일번호 선택\n");
        if (!showfiles(archive, fileList, sd)){
            break;
        }

        recvMsg(sd, choice);
        if(choice[0]=='\n')
            continue;
        menu = atoi(choice);

        if (menu == fileList.size() + 1){
            sendMsg(sd, "clear");
            return;
        }
        else if (menu-1 >= 0 && menu-1 <= fileList.size()){
            // 1. 파일 전송 신호 보내고
            sendMsg(sd, "TrAnSfEr");
            usleep(1 * 1000);
            // 2. 저장할 파일명 보내기
            sendMsg(sd, fileList[menu-1]);
            usleep(2 * 1000);
            cout << fileList[menu-1] << " send!!" << endl;
            // 2. 저장할 파일명 보내기
            sprintf(fullname, "%s/%s", archive, fileList[menu-1].c_str());
            file = fopen(fullname, "rb"); // binary로 open
            if (file != NULL){
                fseek(file, 0, SEEK_END); //파일포인터 끝으로 이동해서
                fsize = ftell(file); // 파일 크기 계산
                cout << "filesize: " << fsize << endl;
                fseek(file, 0, SEEK_SET); // 파일포인터 처음으로 이동
                send(sd, &fsize, sizeof(fsize), 0); // 파일크기 전송
                usleep(2 * 1000);
                nsize = 0;
                while (nsize != fsize){
                    fpsize = fread(fbuf, sizeof(char), BUFSIZE, file); // 256씩읽어서 fbuf에 뒀다가
                    nsize += fpsize;
                    send(sd, fbuf, fpsize, 0);
                    usleep(2 * 1000);
                }
                fclose(file);
            }
        }
        else{
            sendMsg(sd, "잘못 선택했습니다\n");
        }
    }
}

void Club::upload(int sd){
    char filename[1024], fullname[2048], fbuf[BUFSIZE], tmp[512];
    
    int i;
    char choice[512];
    int menu = -100;
    vector<string> fileList;
    FILE* file;
    int nsize, fpsize, fsize, filesize;
    
    sendMsg(sd, "clear");
    while (1){
        sendMsg(sd, "업로드할 파일번호 선택\n");
        usleep(2 * 1000);
        sendMsg(sd, "uploaddd");
        usleep(2 * 1000);
        
        recvMsg(sd, choice);
        sendMsg(sd, choice);
        
        recvMsg(sd, filename);
        cout << "filename: " << filename << endl;
        if(strcmp(filename, "-1") == 0){
            sendMsg(sd, "clear");
            return;
        }
        else {
            // 2. 저장할 파일명 보내기
            sprintf(fullname, "%s/%s", archive, filename);
            cout << "fullname: " << fullname << endl;
            file = fopen(fullname, "wb"); // binary로 open
            if (file == NULL)
				perror("fopen");
            // 3. 파일 크기 받기
			recv(sd, &filesize, sizeof(filesize), 0);
            // 4. 내용 받기
			n = BUFSIZE;
            while (n >= BUFSIZE){
				n = recv(sd, fbuf, BUFSIZE, 0); // 서버에서 받은거 fbuf에 받아두고
				fwrite(fbuf, sizeof(char), n, file); // fbuf내용을 fp에 적어줌
			}
			fclose(file);
            cout << "받음 완료" << endl;
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
        sendMsg(sd, "[3] 자료 삭제\n");
        sendMsg(sd, "[4] 자료 검색\n");
        sendMsg(sd, "[5] 나가기\n");
        

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
            delArchive(sd);
            break;
        case 4:
            srchArchive(sd);
            break;
        case 5:
            sendMsg(sd, "clear");
            return;
            break;
        default:
            sendMsg(sd, "잘못 선택하셨습니다.\n");
            break;
        }
    }
}


void Club::delArchive(int sd){
    char buf[1024], fullname[2048], fbuf[BUFSIZE];
    char choice[2];
    int menu;
    vector<string> fileList;
    
    while (1){
        sendMsg(sd, ">> 삭제할 파일번호 선택\n");
        if (!showfiles(archive, fileList, sd))
            break;

        recvMsg(sd, choice);
        if(choice[0]=='\n')
            continue;
        menu = atoi(choice);

        if (menu == fileList.size() + 1){
            sendMsg(sd, "clear");
            return;
        }
            
        else if (menu >= 1 && menu <= fileList.size()){
            sprintf(fullname, "%s/%s", archive, fileList[menu-1].c_str());
            remove(fullname);
            sendMsg(sd, "clear");
            sendMsg(sd, ">> 삭제가 완료되었습니다.\n");
        }
        else{
            sendMsg(sd, "clear");
            sendMsg(sd, "잘못 선택했습니다\n");
        }
    }
}


void Club::srchArchive(int sd){
    char keyword[1024], buf[1024];
    vector<string> fileList;
    DIR *dir;
    struct dirent *ent;
    int i = 0;
    size_t found;
    string filename;

    sendMsg(sd, "검색어>> ");
    recvMsg(sd, keyword);
    
    
	dir = opendir(archive);
	if (dir == NULL){
		perror ("opendir");
		return;
	}

	while (1) {
		ent = readdir(dir);
		if (ent == NULL){
			break;
		}else{
			if (ent->d_type == DT_REG){
                filename = ent->d_name;
                found = filename.find(keyword);
                if (found != string::npos){
                    sprintf(buf, "%2d] %s\n", ++i, ent->d_name);
                    sendMsg(sd, buf);
                }
			}
		}
	}
    closedir(dir);

    while(1){
        sendMsg(sd, "뒤로(-1)\n");
        recvMsg(sd, buf);
        if (!strcmp(buf, "-1")){
            sendMsg(sd, "clear");
            break;
        }
            
    }
    
}