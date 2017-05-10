#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]) {
    char c0[] = "b";
    char c1[] = "a";
    char* ptrChar = c0;
    char* ptrChar1 = c1;

    if(strcmp(ptrChar, ptrChar1) == 0) {
        cout << "igualdad" << endl;
    }
    
    cout <<"stcmp: " << strcmp(ptrChar, ptrChar1) << endl;
    


    return 0;
}