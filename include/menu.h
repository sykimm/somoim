#include "profile.h"

#define MENU_MAGIC_CODE	0X4D45E55

#define 	ERR_MENU_OK	0

#define 	ERR_MENU_CREATE	-4000
#define 	ERR_MENU_MAGICCODE	-4001
#define 	ERR_MENU_ALLOC	-4002
#define 	ERR_MENU_OPEN	-4003
#define 	ERR_MENU_NOT_FOUND	-4004
#define 	ERR_MENU_SHARED_OBJECT	-4005

#define MAIN_MENU_TITLE	"MAIN_MENU_TITLE"
#define MENU_COUNT	"MENU_COUNT"
#define MENU_TITLE_SO	"MENU_TITLE_SO"
#define MENU_TITLE	"MENU_TITLE"
#define MENU_COMMAND	"MENU_COMMAND"
#define CHOICE_MSG	"CHOICE_MSG"

typedef int (*LPFN_COMMAND)(int);

typedef struct {
	char *sharedObjectName;
	void *handle;
	char *menuTitle;
	char *commandName;
	LPFN_COMMAND fnCommand;
} MENUITEM;

typedef struct {
	int magicCode;
	LPPROFILE lpProfile;
	char *mainTitle;
	char * choiceMsg;
	int	menuCount;
	MENUITEM	menuItem[0];
} MENU;

typedef MENU	*LPMENU;
typedef const MENU *LPC_MENU;

extern "C" {
	int menuCreate(LPMENU *lppRet);
	int menuRun(LPC_MENU lpMenu,int sd);
	int menuDestroy(LPMENU lpMenu);
}

