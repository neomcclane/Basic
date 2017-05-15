#include <iostream>

using namespace std;

int main() {
    int* ptrI = new int(10);


    int& rI = *ptrI;
    rI = 85;
    cout << "i: " << *ptrI;

    delete ptrI;
    
    return 0;
}