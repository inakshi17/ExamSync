#pragma once
#include "auth.h"
#include "structs.h"
#include "ufm.h"
#include "seating.h"
#include "duty.h"
#include "loader.h"
#include <unordered_map>
#include <vector>
using namespace std;

// Portal is the main application class that ties everything together
class Portal {
public:
    // ---- All data containers ----
    Auth                          auth;      // handles user authentication
    unordered_map<string, Student> stuMap;  // roll -> Student (O(1) lookup)
    unordered_map<string, Faculty> facMap;  // fid  -> Faculty (O(1) lookup)
    vector<Room>    rooms;                   // all exam rooms
    vector<Subject> subjects;               // all subjects
    vector<Exam>    exams;                   // all scheduled exams
    UFMLogger       ufmLog;                  // UFM incident manager
    unordered_map<string, Allotment> seatMap; // roll -> Allotment (Room, Bldg, Floor, Seat)

    // Load all CSV data files from a given folder path
    void loadAll(const string& dataPath);

    // Main run loop: shows login, then routes to the right portal
    void run();

private:
    // Menus for each role
    void studentMenu(User& u);
    void facultyMenu(User& u);
    void adminMenu(User& u);

    // Utility helpers
    void sep(); // prints a separator line
    void cls(); // clears the screen
};
