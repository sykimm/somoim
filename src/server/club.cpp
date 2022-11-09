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
#include <fstream>
using namespace std;
#include "club.h"

#define BUFSIZE 256


Club::Club(){ 
    char cwd[512];
    int status;
    cid = n++; 
    
    getcwd(cwd, sizeof(cwd));
    sprintf(archive, "%s/database/%s", cwd, to_string(cid).c_str());
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
    sendMsg(sd, "clear"); usleep(2000);
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
            sendMsg(sd, "clear"); usleep(2000);
            return;
        default:
            sendMsg(sd, "clear"); usleep(2000);
            sendMsg(sd, "잘못입력했습니다\n");
        }
    }
    
}

void Club::delPost(int sd, string mid){
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
    
    vector<Post*>::iterator it = postArr.begin()+pid-1;
    if ((*it)->getWriter() == mid){
        postArr.erase(postArr.begin()+pid-1);
        sendMsg(sd, "clear"); usleep(2000);
        sendMsg(sd, "삭제되었습니다.\n");
    }else{
        sendMsg(sd, "작성자만 삭제가능합니다.\n");
    }
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
    // sendMsg(sd, "나가기(엔터)n");
    // recvMsg(sd, buf);

    while (1){
        sendMsg(sd, "나가기(엔터)n");
        recvMsg(sd, buf);
        if (buf[0] == '\n'){}
            break;
    }
    sendMsg(sd, "clear"); usleep(2000);

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
            sendMsg(sd, "clear"); usleep(2000);
            postPage(sd);
            break;
        case 2: // 글쓰기
            sendMsg(sd, "clear"); usleep(2000);
            addPost(sd, mid);
            break;
        case 3: // 글삭제
            sendMsg(sd, "clear"); usleep(2000);
            delPost(sd, mid);
            break;
        case 4: // 검색
            sendMsg(sd, "clear"); usleep(2000);
            searchPost(sd);
            break;
        case 5: // 나가기
            sendMsg(sd, "clear"); usleep(2000);
            return;
            break;
        default:
            sendMsg(sd, "clear"); usleep(2000);
            sendMsg(sd, ">> 잘못입력했습니다\n");
            break;
        }
    }
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
    
    while (1){
        sendMsg(sd, "다운로드 받을 파일번호 선택\n");
        if (!showfiles(archive, fileList, sd)){
            break;
        }

        recvMsg(sd, choice);
        if(choice[0]=='\n')
            continue;
        menu = atoi(choice);

        if (menu == -1)
            return;
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
    
    while (1){
        sendMsg(sd, "업로드할 파일번호 선택\n");
        usleep(2 * 1000);
        sendMsg(sd, "uploaddd");
        usleep(2 * 1000);
        
        recvMsg(sd, choice);
        sendMsg(sd, choice);
        
        recvMsg(sd, filename);
        cout << "filename: " << filename << endl;
        if(filename == "-1")
            return;
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

        if (menu == fileList.size() + 1)
            return;
        else if (menu >= 1 && menu <= fileList.size()){
            sprintf(fullname, "%s/%s", archive, fileList[menu-1].c_str());
            remove(fullname);
            sendMsg(sd, ">> 삭제가 완료되었습니다.\n");
        }
        else{
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
}