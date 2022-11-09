#include "util.h"
#include <unistd.h>

void sendMsg(int sd, string s){
	send(sd, s.c_str(), s.size(), 0);
	usleep(2000);
}

string recvMsg(int sd, char* buf){
    int n;
    
    n = recv(sd, buf, sizeof(buf), 0);
    buf[n] = '\0';

    return buf;
}


bool showfiles(const char* archive, vector<string> &fileList, int sd){
	DIR *dir;
    struct dirent *ent;
	int i = 0;
    char buf[1024];

    fileList.clear();

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
				sprintf(buf, "%2d] %s\n", ++i, ent->d_name);
                sendMsg(sd, buf);
				fileList.push_back(ent->d_name);
			}
		}
	}
	sprintf(buf, "%2d] 나가기\n", ++i);
    sendMsg(sd, buf);
	closedir(dir);
	return true;
}



vector<string> parseLine(string line){
	int idx = -1;
	int start = 0;
	int n;
	vector<string> v;
	while(true){
		idx = line.find(",", start);
		if (idx == -1){
			// 없음
			v.push_back(line.substr(start));
			break;
		}else{
			// 있음
			n = idx - start;
			v.push_back(line.substr(start, n));
			start = idx+1;
		}
	}
	return v;
}