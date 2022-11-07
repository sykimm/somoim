#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <errno.h>
#include <string.h>
#include <string>
#include <iostream>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>   
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "macro.h"

using namespace std;

#define BUFFER_SIZE 1024

void* writer_thread(void *arg);
void* reader_thread(void *arg);
pthread_t tid1, tid2;

int main(int argc, char *argv[])
{
	int sockfd, new_fd;
	struct sockaddr_in serv_addr;
	int sin_size;
	char cwd[512], archive[1024];

	if(argc!=2){
		fprintf(stderr, "Usage : ./client IP_ADDRESS\n");
		return 1;
	}

	getcwd(cwd, sizeof(cwd)); // client 현재 경로
	sprintf(archive, "%s/download", cwd); // 다운받을 경로
	mkdir(archive, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(60000);
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	memset(&(serv_addr.sin_zero), '\0', 8);

	if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
		perror("CONNECT");
		return 1;
	}

	system("clear");

	if(pthread_create(&tid1, NULL, writer_thread, &sockfd) != 0){
		perror("pthread_create");
	}
	if(pthread_create(&tid2, NULL, reader_thread, &sockfd) != 0){
		perror("pthread_create");
	}

	pthread_join(tid1, NULL);
	
	close(sockfd);

	return 0;
}

void* reader_thread(void *arg){
	int sock = *((int*)arg);
	char cwd[512], archive[1024];
	FILE *fp;
	int pid;
	int n, ret, status;
	char buffer[1024], filename[200], fbuf[256];

	while(1){
		n = recv(sock, buffer, 512, 0);
		if(n <= 0){
			printf("\n서버 연결 끊김\n");
			break;
		}
		buffer[n] = '\0';
		

		if (strcmp(buffer, "TrAnSfEr") == 0){ 
			// copy 하는 경우
			pid = fork();
			
			switch(pid){
			case -1:
				perror("fork()");
				break;
			case 0: // child
				// 1. 파일명 받기
				n = read(sock, filename, 512);
				filename[n] = '\0';
				printf("received filename: %s\n", filename);
				// 2. client의 download 경로 파악 및 파일 오픈 : archive
				getcwd(cwd, sizeof(cwd)); // client 현재 경로
				sprintf(archive, "%s/download/%s", cwd, filename); // 다운받을 경로
				printf("archive: %s\n", archive);
				fp = fopen(archive, "wb");
				if (fp == NULL){
					perror("fopen");
				}

				while (1){
					n = recv(sock, fbuf, 256, 0); // 서버에서 받은거 fbuf에 받아두고
					if(n <= 0){
						printf("\n전송 끝\n");
						break;
					}
					fbuf[n] = '\0';
					fwrite(fbuf, n, sizeof(char), fp); // fbuf내용을 fp에 적어줌
					printf("저장\n");
				}
				fclose(fp);
				cout << "\n>> 다운로드가 완료되었습니다." << endl;
				exit(0);
			default:
				// parent
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
				break;
			}
			
		}else{
			printf("%s", buffer);
			fflush(stdout);
		}


		
	}
	pthread_cancel(tid1);
	pthread_exit(NULL);
}

void* writer_thread(void *arg){
	int sock = *((int*)arg);

	int n;
	char buffer[1024];
	
	while(1){
		fgets(buffer, 1024, stdin);
		n = strlen(buffer);
		buffer[n-1] = '\0';
		if(!strcmp(buffer, "/q"))
			break;
		send(sock, buffer, n, 0);
	}
	pthread_cancel(tid2);
	pthread_exit(NULL);	
}
