#pragma once
#include "structs.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <string>
using namespace std;

// min-heap entry: sorted by lowest duty count for fair balancing
struct FacEntry {
    int duties; 
    string fid;    

    bool operator>(const FacEntry& o) const {
        return duties > o.duties; 
    }
};

// faculty invigilation scheduler class
class DutyScheduler {
public:
    unordered_map<string, Faculty>& facMap;
    unordered_map<string, vector<Duty>> facultyDuties;
    vector<Duty> allDuties;

    DutyScheduler(unordered_map<string, Faculty>& fm) : facMap(fm) {}

    // assign duties across exam dates and rooms using min-heap
    // 2 invigilators per room, 3 flying squad per building
    void assign(const vector<Exam>& exams, const vector<Room>& rooms, int invigilatorsPerRoom = 2, int flyingSquadPerBuilding = 3);

    // get assigned duties for a teacher
    vector<Duty> getDutiesForFaculty(const string& fid) const;

    // get all flying squad duty assignments
    vector<Duty> getFlyingSquadDuties() const;

    // print allocation summary table
    void printSchedule();

    // print flying squad deployment table by date and building
    void printFlyingSquadRoster(const string& specificDate = "") const;

    // get total assigned duties for a teacher
    int getDutyCount(const string& fid);

    // save generated roster to csv
    void saveToFile(const string& fname);
};
