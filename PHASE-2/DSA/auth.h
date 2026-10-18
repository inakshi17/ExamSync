#pragma once
#include "structs.h"
#include <unordered_map>
using namespace std;


class Auth {
public:
    unordered_map<string, User> users;

    void load(const string& file);
    User* login(const string& uname, const string& pwd);

    User* getUser(const string& uname);
};

void printBanner();
