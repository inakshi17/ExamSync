#include "auth.h"
#include "loader.h"
#include <iostream>
using namespace std;

void printBanner() {
    cout << "\n";
    cout << "=========================================\n";
    cout << "     EXAMSYNC - Smart Exam Portal        \n";
    cout << "=========================================\n";
    cout << "  Login to your account                  \n";
    cout << "-----------------------------------------\n";
    cout << "\n";
}

void Auth::load(const string& file) {
    loadUsers(file, users);
}

User* Auth::login(const string& uname, const string& pwd) {
    auto it = users.find(uname);
    if (it == users.end()) {
        return nullptr;
    }
    if (it->second.pwd == pwd) {
        return &it->second;  
    }
    return nullptr;
}

User* Auth::getUser(const string& uname) {
    auto it = users.find(uname);
    if (it == users.end()) {
        return nullptr;
    }
    return &it->second;
}
