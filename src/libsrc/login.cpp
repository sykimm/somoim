#include <iostream>
#include "manager.h"
using namespace std;


extern "C" {
    void login(int sd, Manager& mngr){
        cout << "login() called" << endl;
        
    }
}