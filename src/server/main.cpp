#include <iostream>
#include <string>
#include "menu.h"
#include "manager.h"
using namespace std;

int Club::n = 0;

void loginPage(Manager& mngr, Member& m){
    int c;
    
    while(1){
        while(1){
            cout << "---로그인페이지---" << endl;
            cout << "1. 전체 모임" << endl;
            cout << "2. 모임 검색" << endl;
            cout << "3. 내 모임" << endl;
            cout << "4. 모임 개설" << endl;
            cout << "5. 로그아웃" << endl;
            cout << "선택>> ";
            cin >> c;
            if (1<=c && c<=5) break;
        }
        switch (c)
        {
        case 1: // 전체 모임 리스트 보여주기
            mngr.showAllClubs();
            break;
        case 2: // 모임 검색
            mngr.searchClub();
            break;
        case 3: // 내 모임
            mngr.showMyClubs(m.getId());
            break;
        case 4: // 모임 개설
            cout << m.getId() << "가 main.cpp에서 모임개설을 위해 makeClub()을 부릅니다. " << endl;
            mngr.makeClub(m);
            break;
        case 5: // 로그아웃
            return;
        }
    }
    
}


int main(){
    int ch1;
    Manager mngr;
    Member m;
    

    while (1){
        cout << "---메인페이지---" << endl;
        cout << "1. 로그인" << endl;
        cout << "2. 회원가입" << endl;
        cout << "3. 종료" << endl;
        cout << "선택>> ";
        cin >> ch1;

        switch(ch1){
        case 1: // 로그인
            if (mngr.login(m)){
                cout << "main()에서 login해서 받아온 m의 id : " << m.getId() << endl;
                loginPage(mngr, m);
            }
            else
                cout << "해당 id의 회원이 없습니다." << endl;
            break;
        case 2: // 회원가입
            mngr.joinMember();
            break;
        case 3: // 종료
            exit(1);
            break;
        default:
            break;
        }

    }


    return 0;
}
