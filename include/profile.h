#ifndef _PROFILE_H_DF294208_F203_4892_8059_44FA1D9BECEE_
#define _PROFILE_H_DF294208_F203_4892_8059_44FA1D9BECEE_

#include "hash.h"

#define PROFILE_MAGIC_CODE			0x50524f46 /*PROF*/

#define ERR_PROFILE_OK				0
#define ERR_PROFILE_CREATE			-3000
#define ERR_PROFILE_MAGICCODE		-3001
#define ERR_PROFILE_ALLOC			-3002
#define ERR_PROFILE_OPEN		    -3003
#define ERR_PROFILE_NOT_FOUND		-3004

#ifndef MAX_PATH
    #define MAX_PATH        512
#endif 

typedef struct {
    int  magicCode;
    char filename[MAX_PATH];
    int  isFileLoad;
    LPHASH lpHash;
} PROFILE;

typedef PROFILE *LPPROFILE;
typedef const PROFILE *LPC_PROFILE;


int profileCreate(LPPROFILE* lppRet, char* filename);
int profileGetIntValue(LPC_PROFILE lpProfile, const char* key, int* value);
int profileGetStringValue(LPC_PROFILE lpProfile, const char* key, char** value);
int profileAllDisplay(LPC_PROFILE lpProfile);
int profileDestroy(LPPROFILE lpProfile);

#endif /*_PROFILE_H_DF294208_F203_4892_8059_44FA1D9BECEE_*/
