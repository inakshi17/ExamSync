#pragma once
#include "structs.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <string>
using namespace std;

struct FacEntry {
    int duties; 
    string fid;    

    bool operator>(const FacEntry& o) const {
        return duties > o.duties; 
    }
};

class DutyScheduler {
public:
    
    unordered_map<string, Faculty>& facMap;

    unordered_map<string, vector<Duty>> facultyDuties;

    vector<Duty> allDuties;

    DutyScheduler(unordered_map<string, Faculty>& fm) : facMap(fm) {}

    void assign(const vector<Exam>& exams, const vector<Room>& rooms, int invigilatorsPerRoom = 2);

    void assign(vector<Exam>& exams, int needPerSlot = 3);

    vector<Duty> getDutiesForFaculty(const string& fid) const;

    void printSchedule();

    int getDutyCount(const string& fid);

    void saveToFile(const string& fname);
};
