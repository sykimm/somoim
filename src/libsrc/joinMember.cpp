#include <iostream>
#include <string>
#include "manager.h"
using namespace std;


extern "C"{
    void joinMember(int sd, Manager& mngr){
        cout << "joinMember() called" << endl;
        mngr.join(sd);
    }
}