#include "ctx.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) { cout << "{\"error\":\"no command\"}"; return 1; }
    vector<string> a(argv + 1, argv + argc);
    Ctx c;
    if (a[0] != "login") c.load();                    // login only needs the auth table
    string out = cmdSeating(c, a);
    if (out.empty()) out = cmdOther(c, a);
    cout << (out.empty() ? "{\"error\":\"unknown command\"}" : out);
    return 0;
}

