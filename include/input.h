#ifndef _INPUT_H_DF294208_F203_4892_8059_44FA1D9BECEE_
#define _INPUT_H_DF294208_F203_4892_8059_44FA1D9BECEE_

/*
라인 단위로 문자열을 읽는 함수
만약 문자중 #을 만나면 \n이 나올때까지 모든 문자를 버린다.
*/
#define COMMENT_CODE        '#'

char *inputLine(FILE *fp);

#endif /*_INPUT_H_DF294208_F203_4892_8059_44FA1D9BECEE_*/
