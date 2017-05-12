#include <iostream>
#include <regex>

using namespace std;

int main() {

    string s = "a+a";
    regex e("^(?=.*[a-z]+)(?=.*[0-9]{2})([a-z0-9]+){1,10}$");
    smatch sm;

    if(regex_search(s, sm, regex{"([a-z])\\1"})) {
        for(string token:sm) {
            cout << token << endl;
        }
    }

    return 0;
}