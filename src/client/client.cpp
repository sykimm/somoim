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
#include <vector>
#include <termios.h>
#include "macro.h"

using namespace std;

// #define BUFFER_SIZE 1024
#define BUFSIZE 256

void* writer_thread(void *arg);
void* reader_thread(void *arg);
bool showfiles(const char* archive, vector<string> &fileList);

pthread_t tid1, tid2;
bool flag = false;
struct termios oldt, newt;


class ARGS{
public:
	int *fd;
	
};

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
	ARGS *params = new ARGS();
	params->fd = &sockfd;
	

	if(pthread_create(&tid1, NULL, writer_thread, params) != 0){
		perror("pthread_create");
	}
	if(pthread_create(&tid2, NULL, reader_thread, params) != 0){
		perror("pthread_create");
	}

	pthread_join(tid1, NULL);
	
	close(sockfd);

	return 0;
}



void* reader_thread(void *arg){
	// int sock = *((int*)arg);
	ARGS* params = (ARGS*) arg;
	int sock = *(params->fd);

	char cwd[512], archive[1024];
	FILE *fp;
	int pid;
	int n, ret, status, filesize, i;
	char buffer[1024], filename[200], fbuf[BUFSIZE], fullname[2048];
	DIR *dir;
    struct dirent *ent;
	vector<string> fileList;
	char menu[512];
	int m;
	
	char ch;


	getcwd(cwd, sizeof(cwd)); // client 현재 경로


	while(1){
		n = recv(sock, buffer, 512, 0);
		if(n <= 0){
			printf("\n서버 연결 끊김\n");
			break;
		}
		buffer[n] = '\0';
		
		if (flag == false){
			if (strcmp(buffer, "pw") == 0){
				flag = true;
				fflush(stdout);
				
				tcgetattr(STDIN_FILENO, &oldt);
				newt = oldt;
				newt.c_lflag &= ~(ICANON | ECHO);
				tcsetattr(STDIN_FILENO, TCSANOW, &newt); //에코끈상태로
				continue;
			}
		}
		
		if (strcmp(buffer, "TrAnSfEr") == 0)
		{ // copy 하는 경우
			fflush(stdout);
			// 1. 파일명 받기
			n = recv(sock, filename, 512, 0);
			filename[n] = '\0';

			// 2. client의 download 경로 파악 및 파일 오픈 : archive
			sprintf(archive, "%s/download/%s", cwd, filename); // 다운받을 경로
			system("clear");
			printf(">> 다운로드 경로 : %s\n", archive);
			fp = fopen(archive, "wb");
			if (fp == NULL)
				perror("fopen");
			
			// 3. 파일 크기 받기
			recv(sock, &filesize, sizeof(filesize), 0);
			// 4. 내용 받기
			n = BUFSIZE;
			while (n >= BUFSIZE){
				n = recv(sock, fbuf, BUFSIZE, 0); // 서버에서 받은거 fbuf에 받아두고
				// cout << ">> 수신 중: [" << n << "B/" << filesize << "B]" << endl;
				fwrite(fbuf, sizeof(char), n, fp); // fbuf내용을 fp에 적어줌
			}
			fclose(fp);
			system("clear");
			cout << ">> 다운로드가 완료되었습니다." << endl;
		}
		else if (strcmp(buffer, "uploaddd") == 0)
		{
			fflush(stdout);
			fileList.clear();
			sprintf(archive, "%s/download", cwd); // 내폴더
			if (!showfiles(archive, fileList))
				continue;
			// 2. 선택한 파일명 보내기
			n = recv(sock, menu, 2, 0);
			menu[n] = '\0';
			fflush(stdout);
			m = stoi(menu) - 1;
			if (m == fileList.size()){
				send(sock, "-1", strlen("-1"), 0);
				usleep(2 * 1000);
			}else if (m >= 0 && m < fileList.size()){
				send(sock, fileList[m].c_str(), strlen(fileList[m].c_str()), 0);
				// cout << fileList[m].c_str() << "을 업로드합니다." << endl;
				usleep(2 * 1000);
				sprintf(fullname, "%s/%s", archive, fileList[m].c_str());
				fp = fopen(fullname, "rb"); // binary로 open
				if (fp != NULL){
					fseek(fp, 0, SEEK_END); //파일포인터 끝으로 이동해서
					int fsize = ftell(fp); // 파일 크기 계산
					// cout << "filesize: " << fsize << endl;
					fseek(fp, 0, SEEK_SET); // 파일포인터 처음으로 이동
					send(sock, &fsize, sizeof(fsize), 0); // 파일크기 전송
					usleep(2 * 1000);
					int nsize = 0;
					while (nsize != fsize){
						int fpsize = fread(fbuf, sizeof(char), BUFSIZE, fp); // 256씩읽어서 fbuf에 뒀다가
						nsize += fpsize;
						send(sock, fbuf, fpsize, 0);
						usleep(2 * 1000);
						// sprintf(tmp, ">> 송신 중: [%dB/%sB]", n, filesize);
						// sendMsg(sd, tmp);
					}
					fclose(fp);
					system("clear");
					cout << ">> 업로드를 완료했습니다."<< endl;
				}
			}else{
				cout << "잘못 선택했습니다\n";
			}
		}else{
			if (strcmp(buffer, "clear") == 0)
				system("clear");
			else
				printf("%s", buffer);
			fflush(stdout);
		}
		
	}
	pthread_cancel(tid1);
	pthread_exit(NULL);
}

void* writer_thread(void *arg){
	ARGS* params = (ARGS*) arg;
	int sock = *(params->fd);
	

	int n;
	char buffer[1024];
	char c, i;

	while(1){
		i = 0;
		
		while (1){
			buffer[i] = getchar();
			if (buffer[i] == '\n' || buffer[i] == '\r'){
				buffer[i] = '\0';
				if (flag){
					cout << "\n";
					tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // 원복
					flag = false;
				}
				break;
			}

			if (flag){
				cout << "*";
			}
			i++;
		}
		
		if(!strcmp(buffer, "/q"))
			break;
		send(sock, buffer, i, 0);
		i = 0;
		
	}
	pthread_cancel(tid2);
	pthread_exit(NULL);	
}


bool showfiles(const char* archive, vector<string> &fileList){
	DIR *dir;
    struct dirent *ent;
	int i = 0;

	dir = opendir(archive);
	if (dir == NULL){
		perror ("opendir");
		return false;
	}

	while (1) {
		ent = readdir(dir);
		if (ent == NULL){
			break;
		}else{
			if (ent->d_type == DT_REG){
				cout << ++i << "] " << ent->d_name << endl;
				fileList.push_back(ent->d_name);
			}
		}
	}
	cout << ++i << "] 나가기" << endl;
	closedir(dir);
	return true;
}


