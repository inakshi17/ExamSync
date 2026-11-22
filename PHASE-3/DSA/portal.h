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

// Main portal controller — manages login, data, and all role-based menus.
// This system is for the ODD Semester Examination (Sem 1, 3, 5, 7) only.
class Portal {
public:
    Auth                              auth;         // authentication module
    unordered_map<string, Student>    stuMap;        // all students by roll number (hash table)
    unordered_map<string, Faculty>    facMap;        // all faculty by faculty ID (hash table)
    vector<Student>                   sortedStudents;// students sorted by roll for binary search
    vector<Room>                      rooms;         // all examination rooms
    vector<Exam>                      exams;         // odd-semester exam schedule
    UFMLogger                         ufmLog;        // unfair means incident logger
    unordered_map<string, Allotment>  seatMap;       // current seating allotments
    unordered_map<string, vector<Duty>> dutyMap;     // current faculty duty assignments
    string activeAllocatedLabel;                     // label for the last seating run
    string activeDutyAllocatedLabel;                 // label for the last duty scheduling run

    // Load all data files from the given directory path
    void loadAll(const string& dataPath);

    // Start the main login → role-menu loop
    void run();

private:
    void studentMenu(User& u);  // Student portal
    void facultyMenu(User& u);  // Faculty portal
    void adminMenu(User& u);    // Admin portal

    void sep(); // Print separator line
    void cls(); // Clear screen
};
