#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/*동적 라이브러리를 사용하기 위한 해더 파일 */
#include <dlfcn.h>
#include "menu.h"
// #include "manager.h"

int menuCreate(LPMENU* lppRet)
{
    LPPROFILE lpProfile;
    LPMENU lpMenu;
    int   nErr;
    int   menuCount = 0;
    int   i;
    char  szBuf[256];
    void* handle;
    const char *dlsym_error;
    
    //프로파일 메모리를 할당한다.
    nErr = profileCreate(&lpProfile, "input4.txt");
    if (ERR_PROFILE_OK != nErr) {
        return nErr;
    }
    
    //menu의 수를 읽어들인다.
    profileGetIntValue(lpProfile, MENU_COUNT, &menuCount);
    
    //메뉴를 관리하는 구조체 메모리를 할당한다.
    lpMenu = (LPMENU) calloc (sizeof(MENU) + sizeof(MENUITEM) * menuCount, 1);
    if (NULL == lpMenu) {
        //프로파일 구조체 메모리를 해제한다.
        profileDestroy(lpProfile);
        return ERR_MENU_CREATE;
    }
    //메뉴 메직 코드를 설정한다.
    lpMenu->magicCode = MENU_MAGIC_CODE;
    
    //메뉴의 갯수를 설정한다.
    lpMenu->menuCount = menuCount;
    
    //메뉴 구조체에 프로파일 핸들을 설정한다.
    lpMenu->lpProfile = lpProfile;
    
    //menu title을 읽어들인다.
    profileGetStringValue(lpProfile, MAIN_MENU_TITLE, &lpMenu->mainTitle);

    //choice msg을 읽어들인다.
    profileGetStringValue(lpProfile, CHOICE_MSG, &lpMenu->choiceMsg);
    
    //메뉴의 타이틀을 읽어들인다
    //메뉴를 동작할 shared object(library) 를 읽어들인다
    //메뉴를 동작할 함수명을 읽어들인다.
    for (i=0;i<menuCount;i++) {
        //메뉴의 타이틀을 읽어들이기 위해 KEY를 생성한다
        sprintf(szBuf, "%s_%d", MENU_TITLE, i+1);
        //메뉴의 타이틀을 읽어들인다
        profileGetStringValue(lpProfile, szBuf, &lpMenu->menuItem[i].menuTitle);
        
        //메뉴를 동작할 shared object(library) 읽어들이기 위해 KEY를 생성한다
        sprintf(szBuf, "%s_%d", MENU_TITLE_SO, i+1);
        //메뉴를 동작할 shared object(library) 읽어들인다
        profileGetStringValue(lpProfile, szBuf, &lpMenu->menuItem[i].sharedObjectName);

        //메뉴를 동작할 함수명을 위해 KEY를 생성한다
        sprintf(szBuf, "%s_%d", MENU_COMMAND, i+1);
        //메뉴를 동작할 함수명을 읽어들인다
        profileGetStringValue(lpProfile, szBuf, &lpMenu->menuItem[i].commandName);

        //shared object를 로드한다.
        handle = dlopen (lpMenu->menuItem[i].sharedObjectName, RTLD_LAZY);
        if (NULL == handle) {
            //메뉴 관련 구조체 메모리를 해제한다.
            menuDestroy(lpMenu);
            return ERR_MENU_SHARED_OBJECT;
        }
        //메뉴를 처리할 함수 포인터를 설정한다.
        lpMenu->menuItem[i].fnCommand = (LPFN_COMMAND)dlsym(handle, lpMenu->menuItem[i].commandName);
        
        //shared object의 핸들을 설정한다.
        lpMenu->menuItem[i].handle = handle;
    }
    
	//할당한 메모리의 주소를 인자에 설정하여 값을 리턴합니다
	*lppRet = lpMenu;

	return ERR_MENU_OK;
}

int menuRun(LPC_MENU lpMenu, int sd)
{
    // Manager mng;

    int nErr;
    int i, menu;
    const MENUITEM* menuItem;
    
	//Menu 메모리인가를 확인합니다
	if(MENU_MAGIC_CODE != lpMenu->magicCode) {
		return ERR_MENU_MAGICCODE;
	}
	
	//임시 지역변수를 이용하여 menuItem 배열의 설정한다.
	menuItem = lpMenu->menuItem;

    //무한 반복을 하면서 처리를 메뉴를 입력 반는다.    
    while (1) {
        //메뉴를 출력한다.
	send(sd, lpMenu->mainTitle, strlen(lpMenu->mainTitle), 0);
	send(sd, "\n", strlen("\n"), 0);
    for (i=0;i<lpMenu->menuCount;i++) {
            send(sd,menuItem[i].menuTitle, strlen(menuItem[i].menuTitle), 0);
    send(sd, "\n", strlen("\n"), 0);
    }
	char buffer[1024];
	int n;
	char choice[2];
    sprintf(buffer,"[%d] 종료\n\n", i+1);
	send(sd, buffer, strlen(buffer), 0);
        
    //메뉴를 입력받는다.
    sprintf(buffer,"%s\n",lpMenu->choiceMsg);
	send(sd, buffer, strlen(buffer), 0);
	n=recv(sd, choice, sizeof(choice), 0);
	choice[n]='\0';
	if(choice[0]=='\n')
		continue;
			
	menu=atoi(choice);
	printf("from Client> %d\n", menu);
        
    //메뉴입력에 따라 분기 하여 해당 기능을 수행한다.
    if (1 <= menu && lpMenu->menuCount >= menu) {
        if (NULL != menuItem[menu-1].fnCommand) {
            menuItem[menu-1].fnCommand(sd);
        } else {
            printf("함수가 존재하지 않습니다.\n");
        }	
    } else if ((lpMenu->menuCount+1) == menu) {
        return 0;
    } else {
        printf("잘못입력되었습니다\n");
    }
}

}

int menuDestroy(LPMENU lpMenu)
{
    int i, nErr;
    
	//Menu 메모리인가를 확인합니다
	if(MENU_MAGIC_CODE != lpMenu->magicCode) {
		return ERR_MENU_MAGICCODE;
	}
    
    //포로파일 메모리를 해제한다.
    profileDestroy(lpMenu->lpProfile);

    for (i=0;i<lpMenu->menuCount;i++) {
        //shared object를 언로드한다.
        if (NULL != lpMenu->menuItem[i].handle) {
            dlclose(lpMenu->menuItem[i].handle);
        }
    }

    //Menu메모리를 해제한다.
    free(lpMenu);
    
    return ERR_MENU_OK;
}

