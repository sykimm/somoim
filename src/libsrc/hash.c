#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

unsigned int getHashValue(const char* key)
{
	unsigned int nHash = 0;
	
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	
	return nHash;
}


LPNODE nodeCreate(const char* key, LPDATA value)
{
	LPNODE lpNode;
	
	//Node 메모리를 할당한다.
	lpNode = (LPNODE) calloc (sizeof(NODE), 1);
	if (NULL == lpNode) {
		return NULL;
	}

	//Hash값을 생성한다.
	lpNode->hashValue = getHashValue(key);

	//Hash Key 값을 복사한다.
	strcpy(lpNode->key, key);
	
	//Hash Value 값을 기록한다.
	lpNode->value = value;
	
	return lpNode;
}


int hashCreate(LPHASH* lppRet)
{
	LPHASH lpHash;
	
	//hash 구조체 메모리를 할당합니다.
	lpHash = (LPHASH) calloc (sizeof(HASH) + sizeof(LPNODE) * HASH_DEFAULT_SIZE , 1);
	if (NULL == lpHash) {
		return ERR_HASH_CREATE;
	}
	
	//hash magic code를 설정합니다.
	lpHash->magicCode = HASH_MAGIC_CODE;
	lpHash->nHashSize = HASH_DEFAULT_SIZE;
	//할당한 메모리의 주소를 인자에 설정하여 값을 리턴합니ㄷ
	*lppRet = lpHash;
	
	return ERR_HASH_OK;
}

int hashCount(LPHASH lpHash, int* pCount)
{
	//hash 메모리인가을 확인합니다
	if(HASH_MAGIC_CODE != lpHash->magicCode) {
		return ERR_HASH_MAGICCODE;
	}
	//hash table의 크기를 설정한다.
	*pCount = lpHash->count;
	
	return ERR_HASH_OK;
}

// 삭제할 키를 전달
int hashRemoveKey(LPHASH lpHash, const char* key)
{
	LPNODE lpNode, lpPrev;
	unsigned int hashValue = getHashValue(key); //key에 대한 hash value값을 생성한다.
	int nHashBucket;
	
	//hash 메모리인가을 확인합니다
	if(HASH_MAGIC_CODE != lpHash->magicCode) {
		return ERR_HASH_MAGICCODE;
	}
	
	//node을 buckey의 위치를 구한다.
	nHashBucket = hashValue % lpHash->nHashSize;
	//node 배열로 부터 링크드 리스트의 Header를 구한다.
	lpNode = lpHash->array[nHashBucket];
	
	if (NULL == lpNode) {
		//key에 대한 node가 존재하지 않을 경우 
		return ERR_HASH_OK;
	}
	
	//key가 존재하는 경우 위치를 찾는다
	lpPrev = NULL;
	while (NULL != lpNode) {
		if (0 == strcmp(lpNode->key, key)) {
			//메모리를 해제해야 할 경우 메모리 해제 함수를 호출한다.
			if (lpHash->free) {
				lpHash->free(lpNode->value);
			}
			if (NULL == lpPrev) {
				//삭제될 노드의 위치가 시작인경우 
				lpHash->array[nHashBucket] = lpNode->pNext;
			} else {
				//삭제될 노드의 위치가 시작이 아닌 경우
				lpPrev->pNext = lpNode->pNext;
			}
			//링크드 리스트를 관라하는 메모리를 해제한다.
			free(lpNode);
			break;
		}
		//Node의 이전위치를 기록한다.
		lpPrev = lpNode;
		// 다음 위치로 이동한다.
		lpNode = lpNode->pNext;
	}
	
	//새롭게 삽입된 노드를 관리 하기 위해 count를 증가한다.
	lpHash->count--;
	
	return ERR_HASH_OK;
}

// key를 전달해서 value를 읽어오는 경우
int hashGetValue(LPHASH lpHash, const char* key, LPDATA* value)
{
	LPNODE lpNode;
	unsigned int hashValue = getHashValue(key);
	int nHashBucket;

	//hash 메모리인가을 확인합니다
	if(HASH_MAGIC_CODE != lpHash->magicCode) {
		return ERR_HASH_MAGICCODE;
	}

	//node의 buckey의 위치를 구한다.
	nHashBucket = hashValue % lpHash->nHashSize;
	//vlaue의 값을 초기화한다.
	*value = NULL;
	
	//node 배열로 부터 링크드 리스트의 Header를 구한다.
	lpNode = lpHash->array[nHashBucket];
	//Node의 다음이 NULL이 아닐때 까지 이동을 하면 자료를 찾는다
	while (NULL != lpNode) {
		//링크드 리스트에서 key와 같은 노드를 찾는다.
		if (0 == strcmp(lpNode->key, key)) {
			*value = lpNode->value;
			break;
		}
		//다음 노드로 이동을 한다.
		lpNode = lpNode->pNext;
	}
	
	return ERR_HASH_OK;
}

// key, value 해시테이블에 등록
int hashSetValue(LPHASH lpHash, const char* key, const LPDATA value)
{
	LPNODE lpNode;
	int nHashBucket;
	LPNODE lpTemp;
	int isExist;
	
	if(lpHash == NULL) {
		return ERR_HASH_CREATE;
	}
	
	//hash 메모리인가을 확인합니다
	if(HASH_MAGIC_CODE != lpHash->magicCode) {
		return ERR_HASH_MAGICCODE;
	}
	
	//Node를 생성한다.
	lpNode = nodeCreate(key, value);
	if (NULL == lpNode) {
		return ERR_HASH_ALLOC;
	}
	
	//node을 삽입할 buckey의 위치를 구한다.
	nHashBucket = lpNode->hashValue % lpHash->nHashSize;
	//node 배열로 부터 링크드 리스트의 Header를 구한다.
	lpTemp = lpHash->array[nHashBucket];
	
	//hash table에 key가 존재하는지 확인한다.
	hashIsKey(lpHash, key, &isExist);
	if (TRUE == isExist) {
	
		//key가 존재하는 경우 위치를 찾는다
		while (NULL != lpTemp) {
			if (0 == strcmp(lpTemp->key, key)) {
				//메모리를 해제해야 할 경우 메모리 해제 함수를 호출한다.
				if (lpHash->free) {
					lpHash->free(lpTemp->value);
				}
				
				//key의 위치에 value를 설정한다.
				lpTemp->value = value;
				break;
			}
			// 다음 위치로 이동한다.
			lpTemp = lpTemp->pNext;
		}
	
	} else {
		//새로운 노드는 항상 시작위치에 삽입한다.
		//만약 마지막 위치 삽입을 하게 하려면 위치를 찾기 
		//위해 이동을 하는 코드가 삽입된야 함(속도 저하 문제 발생 원인)
		lpNode->pNext = lpHash->array[nHashBucket];
		lpHash->array[nHashBucket] = lpNode;
		
		//새롭게 삽입된 노드를 관리 하기 위해 count를 증가한다.
		lpHash->count++;
	}
	
	return ERR_HASH_OK;
}

int hashIsKey(LPHASH lpHash, const char* key, int* isValue)
{
	LPNODE lpNode;
	unsigned int hashValue = getHashValue(key);
	int nHashBucket;
	
	//hash 메모리인가을 확인합니다
	if(HASH_MAGIC_CODE != lpHash->magicCode) {
		return ERR_HASH_MAGICCODE;
	}
	
	//node을 삽입할 buckey의 위치를 구한다.
	nHashBucket = hashValue % lpHash->nHashSize;
	//초기는 찾지 못한것으로 설정한다..
	*isValue = FALSE;
	
	//node 배열로 부터 링크드 리스트의 Header를 구한다.
	lpNode = lpHash->array[nHashBucket];
	if (NULL == lpNode) {//리스트에 시작을 나타낸다.
		return ERR_HASH_OK;
	} else {
		//마직막 위치에 삽입한다.
		while (NULL != lpNode) {
			//링크드 리스트에서 key와 같은 노드를 찾는다.
			if (0 == strcmp(lpNode->key, key)) {
				*isValue = TRUE;
			break;
			}
			//다음 노드로 이동을 한다.
			lpNode = lpNode->pNext;
		}
	}
	
	return ERR_HASH_OK;
}


// 해시테이블 전체에 접근을 시작할때
int hashGetFirstPostion(LPHASH lpHash, POSITION* position)
{
	//hash 메모리인가을 확인합니다
	if(HASH_MAGIC_CODE != lpHash->magicCode) {
		return ERR_HASH_MAGICCODE;
	}
	
	if (0 == lpHash->count) {
		*position = NULL;
	} else {
		*position = BEFORE_START_POSITION;
	}
	return ERR_HASH_OK;
}

// 해시테이블의 원소를 연속적으로 가져올때
// position : 다음 node의 주소를 받아옴
// pkey, pvalue : 현재 노드의 키와 밸류
int hashGetNextPostion(LPHASH lpHash, POSITION* position, char** pKey, LPDATA* pValue)
{
	LPNODE lpNode;
	LPNODE lpNext;
	int i;
	
	//hash 메모리인가을 확인합니다
	if(HASH_MAGIC_CODE != lpHash->magicCode) {
		return ERR_HASH_MAGICCODE;
	}
	
	//다음 위치 포인터를 이용하여 노드를 얻는다
	lpNode  = (LPNODE) *position;
	if (lpNode == NULL) {
		return ERR_HASH_OK;
	}
	
	if (lpNode == (LPNODE) BEFORE_START_POSITION) {
		// 현재의 노드가 시작인가를 확인한다.
		for (i = 0; i< lpHash->nHashSize; i++) {
			//Node 배열로 부터 자료가 삽입된 list의 시작 위치를 얻는다
			lpNode = lpHash->array[i];
			if (NULL != lpNode) {
				break;
			}
		}
		//더이상 자료가 존재지 않음
		if (NULL == lpNode) {
			*position = (POSITION) NULL;
			return ERR_HASH_OK;
		}
	}
	
	//노드의 다음위치를 설정 한다.
	lpNext = lpNode->pNext;
	if (NULL == lpNext) {
		// 다음 자료를 찾을 수 없으면 다음 bucket에 찾는다.
		for (i= (lpNode->hashValue % lpHash->nHashSize) + 1; i < lpHash->nHashSize; i++) {
			lpNext = lpHash->array[i];
			if (NULL != lpNext) {
				break;
			}
		}
	}
	
	*position = (POSITION) lpNext;
	// 인자에 리턴할 값을 기록한다.
	*pKey = lpNode->key;
	*pValue = lpNode->value;
	return ERR_HASH_OK;
}

// 사용자가 정의한 자료구조를 해제하는 함수 등록
int hashSetFree(LPHASH lpHash, void(* lpfn_free)(void*) )
{
	//hash 메모리인가을 확인합니다
	if(HASH_MAGIC_CODE != lpHash->magicCode) {
		return ERR_HASH_MAGICCODE;
	}
	
	lpHash->free = lpfn_free;
	return ERR_HASH_OK;
}


// 해시테이블 삭제
int hashDestroy(LPHASH lpHash)
{
	LPNODE lpNode, lpTemp;
	int i;
	
	//hash 메모리인가을 확인합니다
	if(HASH_MAGIC_CODE != lpHash->magicCode) {
		return ERR_HASH_MAGICCODE;
	}
	
	for (i=0;i<lpHash->nHashSize;i++) {
		lpNode = lpHash->array[i];
		while (NULL != lpNode) {
			//삭제할 Node를 임시 변수에 대입한다.
			lpTemp = lpNode;
			//Node를 다음 위치로 이동한다.
			lpNode = lpTemp->pNext;
			//노드에 삽입된 value 를 해제할 함수가 존재하면 해제 함수를 호출한다.
			if (lpHash->free) {
				lpHash->free(lpTemp->value);
			}
			//노드 메모리를 제거한다.
			free(lpTemp);
		}
	}
	
	free(lpHash);
	
	return ERR_HASH_OK;
}
