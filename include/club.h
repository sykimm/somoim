#pragma once
#include <string>
#include <vector>
#include "member.h"
#include "util.h"
#include "post.h"
using namespace std;

bool showfiles(const char* archive, vector<string> &fileList);

class Club{
    int cid;
    char archive[1024]; // 서버에 있는 이 모임에 대한 db 위치
    string clubName;
    string description;
    vector<Post*> postArr;
    vector<Member*> memArr;
    // vector<Member*> archive;
    void showArrList(int sd, vector<Post*> arr);
public:
    static int n;
    static void setN() { n = 0; }
    Club();
    int getId(){ return cid; }
    string getName(){ return clubName; }
    string getDesc(){ return description; }
    void setName(string s){ clubName = s; }
    void setDesc(string s){ description = s; }
    void addPost(int sd, string mid);
    void showPostList(int sd);
    void addMember(Member* m){
        memArr.push_back(m);
    }
    int getTotalNo(){ return memArr.size(); }
    bool isMember(string mid){ 
        for(Member* mptr: memArr){
            if (mptr->getId() == mid)
                return true;
        }
        return false;
    }
    void postPage(int sd);
    void delPost(int sd);
    void searchPost(int sd);
    void boardPage(int sd, string mid);
    void showArchive(int sd);
    void download(int sd);
    void upload(int sd);
    void delArchive(int sd);
    void srchArchive(int sd);
};



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
