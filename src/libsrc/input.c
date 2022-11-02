#include <stdio.h>
#include <stdlib.h>
#include "input.h"

char *inputLine(FILE *fp)
{
    int c, n;
    static char line[BUFSIZ+1];
    int flag = 0;
    
    n = 0;
	while ((c = fgetc(fp)) != '\n') {
        if (EOF == c) {
            return(NULL);
        }
            
        //입력 버퍼 보다 크면 while 루프를 종료한다 
        if (n >= BUFSIZ) {
            break;
        }
        
        //읽어들인 문자가 comment 이면 flag 설정한다.
        if (COMMENT_CODE == c) {
            flag = 1;
        }
        
        //flag가 설정되지 않았다면 읽어들인 문자를 버퍼에 기록한다.
        if (1 != flag) {
            line[n++] = c;
        }
    }

    line[n] = '\0';
    return(line);
}

