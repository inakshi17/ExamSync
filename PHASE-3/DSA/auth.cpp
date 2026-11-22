#include "auth.h"
#include "loader.h"
#include <iostream>
using namespace std;

// display main login banner
void printBanner() {
    cout << "\n";
    cout << "=========================================\n";
    cout << "     EXAMSYNC - Smart Exam Portal        \n";
    cout << "=========================================\n";
    cout << "  Login to your account                  \n";
    cout << "-----------------------------------------\n";
    cout << "\n";
}

// load user credentials from file into hash map
void Auth::load(const string& file) {
    loadUsers(file, users);
}

// verify username and password in O(1) time
User* Auth::login(const string& uname, const string& pwd) {
    auto it = users.find(uname); // hash table lookup
    if (it == users.end()) {
        return nullptr; // user not found
    }
    if (it->second.pwd == pwd) {
        return &it->second; // password matched
    }
    return nullptr; // wrong password
}

// lookup user by username
User* Auth::getUser(const string& uname) {
    auto it = users.find(uname);
    if (it == users.end()) {
        return nullptr;
    }
    return &it->second;
}
