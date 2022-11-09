#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
// #include <string.h>
#include <string>
#include <cstring>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
// #include "menu.h"
//#include "member.h"
#include "manager.h"
#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <cereal/archives/binary.hpp>
#include <sstream>
using namespace std;

#define BACKLOG 1024
int Club::n = 0;
int Post::n = 0;

class ARGS{
public:
	int *fd;
	Manager *mngr;
};

// void sendMsg(int sd, string s){
// 	// send(sd, s, strlen(s), 0);
// 	send(sd, s.c_str(), s.size(), 0);
// }


void loginPage(int sd, Manager& mngr, Member& m){
	int menu, n;
	char choice[2];

	
	while (1){
		//메뉴를 출력한다.
		sendMsg(sd, "---로그인페이지---\n");
		sendMsg(sd, "[1] 전체모임\n");
		sendMsg(sd, "[2] 나의모임\n");
		sendMsg(sd, "[3] 모임개설\n");
		sendMsg(sd, "[4] 로그아웃\n");
		
		//메뉴를 입력받는다.
		sendMsg(sd, "선택>> ");
		n = recv(sd, choice, sizeof(choice), 0);
		choice[n]='\0';
		if(choice[0]=='\n')
			continue;
		menu = atoi(choice);
		cout << "from Client> " << menu << endl;
			
		//메뉴입력에 따라 분기 하여 해당 기능을 수행한다.
		switch(menu){
		case 1: // 전체모임
			sendMsg(sd, "clear");
            mngr.showAllClubs(sd, m);
            break;
        case 2: // 나의모임
			sendMsg(sd, "clear");
            mngr.showMyClubs(m, sd);
            break;
        case 3: // 모임개설
			sendMsg(sd, "clear");
            mngr.makeClub(sd, m);
			sendMsg(sd, "clear");
            break;
        case 4: // 로그아웃
			sendMsg(sd, "clear");
            return; // 메인페이지로 돌아감
		default:
			sendMsg(sd, "clear");
			sendMsg(sd, "잘못 입력되었습니다");
			break;
		}
	}

}


void mainPage(int sd, Manager& mngr){
	int i, menu;
	Member m;
	int n;
	char choice[2];
	char buffer[1024];

	
	while (1) {
        //메뉴를 출력한다.
		sendMsg(sd, "---메인페이지---\n");
		sendMsg(sd, "[1] 로그인\n");
		sendMsg(sd, "[2] 회원가입\n");
		sendMsg(sd, "[3] 종료\n");
		
		//메뉴를 입력받는다.
		sendMsg(sd, "선택>> ");
		n = recv(sd, choice, sizeof(choice), 0);
		choice[n]='\0';
		if(choice[0]=='\n')
			continue;
				
		menu = atoi(choice);
		cout << "from Client> " << menu << endl;
			
		//메뉴입력에 따라 분기 하여 해당 기능을 수행한다.
		switch(menu){
		case 1:
			if (mngr.login(sd, m)){
				cout << "main()에서 login해서 받아온 m의 id : " << m.getId() << endl;
				sendMsg(sd, "clear");
				sendMsg(sd, m.getName() + "님 안녕하세요!\n");
				loginPage(sd, mngr, m);
			}else{
				sendMsg(sd, "clear"); usleep(2000);
				sendMsg(sd, "ID 또는 비밀번호가 일치하지 않습니다.\n");
			}
			break;
		case 2:
			if (mngr.join(sd)){
				sendMsg(sd, "clear"); usleep(2000);
				sendMsg(sd, "회원가입 성공\n");
			}else{
				sendMsg(sd, "clear"); usleep(2000);
				sendMsg(sd, "회원가입 실패\n");
			}
				
            break;
		case 3:
			mngr.saveData();
			return;
		default:
			sendMsg(sd, "clear"); usleep(2000);
			sendMsg(sd, "잘못 입력되었습니다");
			break;
		}
    }
	// system("clear");
}


void* start_main(void* arg)
{
	ARGS* params = (ARGS*) arg;
	int sd = *(params->fd);

	mainPage(sd, *(params->mngr));
	close(sd);
	return NULL;
}



char line[]="\n─────────────────────────────────────────────────────────────────────\n";
char thick_line[]="\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";




int main(){

	int sockfd, new_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	int sin_size;
	int yes=1;
	pthread_t tid;
	Manager mngr;

	
    ifstream fin("userinfo.txt");
    if (!fin){
        cout << "userinfo.txt 열 수 없음" << endl;
    }else{
		cout << "load 하기" << endl;
        mngr.loadData(fin);
    }


	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(60000);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(my_addr.sin_zero), '\0', 8);

	if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}

	if(listen(sockfd, BACKLOG) == -1) 
	{
		perror("listen");
		exit(1);
	}
	
	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);

		if((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, (socklen_t*) &sin_size)) == -1)
		{
			perror("accept");
			continue;
		}
		printf("server : got connection from %s \n", inet_ntoa(their_addr.sin_addr));
		ARGS *params = new ARGS();
		params->fd = &new_fd;
		params->mngr = &mngr;
		

		if(pthread_create(&tid, NULL, start_main, params)!=0) {
			perror("pthread_create");
		} else {
			pthread_detach(tid);
		}
	}

	return 0;
}
