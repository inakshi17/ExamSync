#pragma once
#include "structs.h"
#include <unordered_map>
using namespace std;

// authentication manager class
class Auth {
public:
    // hash table of users: username -> User object
    unordered_map<string, User> users;

    // load credentials from csv
    void load(const string& file);

    // verify credentials and return user pointer
    User* login(const string& uname, const string& pwd);

    // lookup user by username
    User* getUser(const string& uname);
};

// prints main login banner
void printBanner();
