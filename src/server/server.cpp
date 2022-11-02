#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
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
#include <dlfcn.h>
using namespace std;

#define BACKLOG 1024
int Club::n = 0;

typedef int (*LPFN_COMMAND)(int);

class item{
public:
	void *handle;
	string menuTitle;
	string sharedObjectName;
	string commandName;
	LPFN_COMMAND fnCommand;
};

class Menu{
public:
	string mainTitle;
	int menuCount;
	item menuItem[2];
	string choiceMsg;
};

void mainPage(int sd){
	// send(sd, "hi", strlen("hi"), 0);
	// sleep(10);
	int i, menu;
	void* handle;
	Menu *lpMenu = new Menu();
	
	lpMenu->mainTitle = "---메인페이지---";
	lpMenu->menuCount = 2;
	lpMenu->choiceMsg = "선택>> ";
	/////////
	lpMenu->menuItem[0].menuTitle = "[1] 로그인";
	lpMenu->menuItem[0].sharedObjectName = "/home/mobis/ksy/final/src/libsrc/liblogin.so";
	lpMenu->menuItem[0].commandName = "login";
	handle = dlopen(lpMenu->menuItem[0].sharedObjectName.c_str(), RTLD_LAZY);
	if (NULL == handle) return;
	lpMenu->menuItem[0].fnCommand = (LPFN_COMMAND)dlsym(handle, lpMenu->menuItem[0].commandName.c_str());
	lpMenu->menuItem[0].handle = handle;
	/////////
	lpMenu->menuItem[1].menuTitle = "[2] 회원가입";
	lpMenu->menuItem[1].sharedObjectName = "/home/mobis/ksy/final/src/libsrc/libjoinMember.so";
	lpMenu->menuItem[1].commandName = "joinMember";
	handle = dlopen(lpMenu->menuItem[1].sharedObjectName.c_str(), RTLD_LAZY);
	if (NULL == handle) return;
	lpMenu->menuItem[1].fnCommand = (LPFN_COMMAND)dlsym(handle, lpMenu->menuItem[1].commandName.c_str());
	lpMenu->menuItem[1].handle = handle;


	const item* menuItem;
	menuItem = lpMenu->menuItem;

	while (1) {
        //메뉴를 출력한다.
		send(sd, lpMenu->mainTitle.c_str(), lpMenu->mainTitle.size(), 0);
		send(sd, "\n", strlen("\n"), 0);
		for (i=0; i<lpMenu->menuCount; i++){
			send(sd, menuItem[i].menuTitle.c_str(), menuItem[i].menuTitle.size(), 0);
			send(sd, "\n", strlen("\n"), 0);
		}

		char buffer[1024];
		int n;
		char choice[2];
		sprintf(buffer,"[%d] 종료\n", i+1);
		send(sd, buffer, strlen(buffer), 0);
		
		//메뉴를 입력받는다.
		sprintf(buffer, "%s", lpMenu->choiceMsg.c_str());
		send(sd, buffer, strlen(buffer), 0);
		n=recv(sd, choice, sizeof(choice), 0);
		choice[n]='\0';
		if(choice[0]=='\n')
			continue;
				
		menu = atoi(choice);
		cout << "from Client> " << menu << endl;
			
		//메뉴입력에 따라 분기 하여 해당 기능을 수행한다.
		if (1 <= menu && menu <= lpMenu->menuCount) {
			if (NULL != menuItem[menu-1].fnCommand) {
				menuItem[menu-1].fnCommand(sd);
			} else {
				printf("함수가 존재하지 않습니다.\n");
			}	
		} else if ((lpMenu->menuCount+1) == menu) {
			return;
		} else {
			printf("잘못입력되었습니다\n");
		}
    }
}


void* start_main(void* arg)
{
	int sd = *((int*) arg);	
	mainPage(sd);
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
		if(pthread_create(&tid, NULL, start_main, &new_fd)!=0) {
			perror("pthread_create");
		} else {
			pthread_detach(tid);
		}
	}

	return 0;
}
