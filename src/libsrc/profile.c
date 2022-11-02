#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "profile.h"
#include "input.h"

static int _fileLoad(LPPROFILE lpProfile) 
{
	char *str;
    char *key, *value;
    FILE *fp;
    LPHASH lpHash = lpProfile->lpHash;
    
	//파일을 open 한다.
	fp = fopen(lpProfile->filename, "r");
	printf("filename = %s\n", lpProfile->filename);
	if (NULL == fp) {
	    printf("%s 파일을 열수 없습니다\n", lpProfile->filename);
	    exit(1);
	}
	
	while (1) {
	    //파일에 한 라인을 읽어들입니다.
		str = inputLine(fp);

		if (NULL == str) {
			 break;
		}
		if (0 != str[0]) {
    		key = strtok(str, "=");
    		value = key + strlen(key) + 1;
    		//value의 복사본을 hash 에 저장한다.
    		hashSetValue(lpHash, key, strdup(value));
        }
	}
	
	//파일을 닫습니다.
	fclose(fp);
	
	//파일 로딩을 상태를 설정한다.
	lpProfile->isFileLoad = TRUE;
	
	return ERR_PROFILE_OK;
}


int profileCreate(LPPROFILE* lppRet, char* filename)
{
    LPPROFILE lpProfile;
    int nErr;
    
	//profile 구조체 메모리를 할당합니다.
	lpProfile = (LPPROFILE) calloc (sizeof(PROFILE), 1);
	if (NULL == lpProfile) {
		return ERR_PROFILE_CREATE;
	}

    //hash 메모리를 할당합니다.
    nErr = hashCreate(&lpProfile->lpHash);
    if (ERR_HASH_OK != nErr) {
        // 기존에 할당받은 메모리는 해제한다.
        free(lpProfile);
        return nErr;
    }
	
	//profile magic code를 설정합니다.
	lpProfile->magicCode = PROFILE_MAGIC_CODE;
    strcpy(lpProfile->filename, filename);

    //파일을 읽지 않았으면 파일을 읽어서 profile을 생성한다.
    nErr = _fileLoad(lpProfile);
    if (ERR_PROFILE_OK != nErr) {
            return nErr;
    }

    //hash의 value는 free() 함수를 사용하여 메모리를 해제해야한다.
    hashSetFree(lpProfile->lpHash, free);

	//할당한 메모리의 주소를 인자에 설정하여 값을 리턴합니다
	*lppRet = lpProfile;

	return ERR_PROFILE_OK;
}

int profileGetIntValue(LPC_PROFILE lpProfile, const char* key, int* value)
{
    int nErr;
    LPHASH lpHash;
    char* lpValue;
    
	//profile 메모리인가를 확인합니다
	if(PROFILE_MAGIC_CODE != lpProfile->magicCode) {
		return ERR_PROFILE_MAGICCODE;
	}
    
    //profile에 있는 hash 포인터를 임시 변수로 설정한다.
    lpHash = lpProfile->lpHash;

    //key 에 대한 값을 읽어들인다.
    nErr = hashGetValue(lpHash, key, (LPDATA*) &lpValue);
    if (ERR_HASH_OK != nErr) {
        return nErr;
    }
    
    //읽어들인 문자열을 수치로 변환한다.
    *value = atoi(lpValue);
        
    return ERR_PROFILE_OK;
}

int profileGetStringValue(LPC_PROFILE lpProfile, const char* key, char** value)
{
    int nErr;
    LPHASH lpHash;
    
	//profile 메모리인가를 확인합니다
	if(PROFILE_MAGIC_CODE != lpProfile->magicCode) {
		return ERR_PROFILE_MAGICCODE;
	}
    
    //profile에 있는 hash 포인터를 임시 변수로 설정한다.
    lpHash = lpProfile->lpHash;

    //key 에 대한 값을 읽어들인다.
    nErr = hashGetValue(lpHash, key, (LPDATA*) value);
    if (ERR_HASH_OK != nErr) {
        return nErr;
    }
    
    return ERR_PROFILE_OK;
}

int profileAllDisplay(LPC_PROFILE lpProfile)
{
    int nErr;
    LPHASH lpHash;
	char *key;
	char *value;
	POSITION pos;
    
	//profile 메모리인가를 확인합니다
	if(PROFILE_MAGIC_CODE != lpProfile->magicCode) {
		return ERR_PROFILE_MAGICCODE;
	}
    
    //profile에 있는 hash 포인터를 임시 변수로 설정한다.
    lpHash = lpProfile->lpHash;

	//hash 테이블의 처음 위치를 얻는다.
	nErr = hashGetFirstPostion(lpHash, &pos);
	if (ERR_HASH_OK != nErr) {
		printf("%s:%d error code = %d\n",__FILE__, __LINE__, nErr);
	}

	//다음 위치로 이동하여 
	while (pos) {
		nErr = hashGetNextPostion(lpHash, &pos, &key, (LPDATA*) &value);
		if (ERR_HASH_OK != nErr) {
			printf("%s:%d error code = %d\n",__FILE__, __LINE__, nErr);
			break;
		}

		//key 값과 value를 ?력한다.
		printf("key = %s   value = %s\n", key, value);
	}
    
    return ERR_PROFILE_OK;
}


int profileDestroy(LPPROFILE lpProfile)
{
    int nErr;
    
	//profile 메모리인가를 확인합니다
	if(PROFILE_MAGIC_CODE != lpProfile->magicCode) {
		return ERR_PROFILE_MAGICCODE;
	}
    
    //hash 메모리를 해제한다.
    nErr = hashDestroy(lpProfile->lpHash);
    if (ERR_HASH_OK != nErr) {
        return nErr;
    }
    
    //Profile 메모리를 해제한다.    
    free(lpProfile);    
    	
	return ERR_PROFILE_OK;
}

