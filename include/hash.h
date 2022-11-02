#ifndef _HASH_DF294208_F203_4892_8059_44FA1D9BECEE_
#define _HASH_DF294208_F203_4892_8059_44FA1D9BECEE_

/*hash에 의한 포인터가 가리기는 진정한 메모리 인지를 확인하기 위한 값*/
#define HASH_MAGIC_CODE 0x48415348 /*HASH*/

#define ERR_HASH_OK 0
#define ERR_HASH_CREATE -2000
#define ERR_HASH_MAGICCODE -2001
#define ERR_HASH_ALLOC -2002
#define ERR_HASH_NOT_FOUND -2003


#define HASH_DEFAULT_SIZE 17
#define HASH_KEY_SIZE 256

#define FALSE 0
#define TRUE !FALSE

#define BEFORE_START_POSITION ((POSITION) -1)

typedef void* LPDATA;
typedef void* POSITION; 
typedef unsigned int UINT;


typedef struct _tagNode { // 링크드리스트
UINT         hashValue;
char         key[HASH_KEY_SIZE];
LPDATA       value;
struct _tagNode* pNext; //자기참조 구조체
} NODE;


typedef NODE *LPNODE;
typedef const NODE *LPC_NODE;

//메모리를 해제할 수 있는 함수 포인터 정의;
typedef void (*LPFN_FREE)(void*);

typedef struct _tagHash {
int magicCode;
int nHashSize;  // 최대크기
int count; // 실제 크기
LPFN_FREE free; // 함수 포인터 (함수 주소대입하는 변수)
LPNODE array[0]; // 0짜리 배열,,,(가변되지 않는 크기라 가정)
} HASH;

typedef HASH *LPHASH;
typedef const HASH* LPC_HASH;


int hashCreate(LPHASH* lppRet);
int hashCount(LPHASH lpHash, int* pCount);
int hashGetValue(LPHASH lpHash, const char* key, LPDATA* value);
int hashSetValue(LPHASH lpHash, const char* key, const LPDATA value);
int hashIsKey(LPHASH lpHash, const char* key, int* isValue);
int hashRemoveKey(LPHASH lpHash, const char* key);
int hashGetFirstPostion(LPHASH lpHash, POSITION* position);
int hashGetNextPostion(LPHASH lpHash, POSITION* position, char** pKey, LPDATA* pValue);
int hashSetFree(LPHASH lpHash, void(*)(void*));
int hashDestroy(LPHASH lpHash);


#endif /*_HASH_DF294208_F203_4892_8059_44FA1D9BECEE_*/


