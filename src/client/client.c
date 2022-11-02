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
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/unistd.h>
#include "macro.h"

#define BUFFER_SIZE 1024

void* writer_thread(void *arg);
void* reader_thread(void *arg);
pthread_t tid1, tid2;

int main(int argc, char *argv[])
{
	int sockfd, new_fd;
	struct sockaddr_in serv_addr;
	int sin_size;
	
	if(argc!=2){
		fprintf(stderr, "Usage : ./client IP_ADDRESS\n");
		return 1;
	}


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

	int n;
	char buffer[1024];

	while(1){
		n = recv(sock, buffer, 512, 0);
		if(n <= 0){
			printf("\n서버 연결 끊김\n");
			break;
		}
		buffer[n] = '\0';

		printf("%s", buffer);
		fflush(stdout);
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
