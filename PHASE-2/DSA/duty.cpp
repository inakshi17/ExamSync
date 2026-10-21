#include "duty.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <stack>
using namespace std;

void DutyScheduler::assign(const vector<Exam>& exams, const vector<Room>& rooms, int invigilatorsPerRoom) {
    facultyDuties.clear();
    allDuties.clear();
    if (facMap.empty() || exams.empty() || rooms.empty()) return;

    for (auto& kv : facMap) {
        kv.second.duties = 0;
    }

    vector<string> dates;
    unordered_set<string> seenDates;
    for (const auto& e : exams) {
        if (!e.date.empty() && seenDates.find(e.date) == seenDates.end()) {
            seenDates.insert(e.date);
            dates.push_back(e.date);
        }
    }

    priority_queue<FacEntry, vector<FacEntry>, greater<FacEntry>> minHeap;
    for (const auto& kv : facMap) {
        minHeap.push({0, kv.first});
    }

    for (const string& examDate : dates) {
        unordered_set<string> assignedToday;
        vector<FacEntry> pickedToday;

        for (size_t rmIdx = 0; rmIdx < rooms.size(); rmIdx++) {
            const auto& rm = rooms[rmIdx];
            string dutySlot = (rmIdx % 2 == 0) ? "09:00 AM - 12:00 PM" : "01:00 PM - 04:00 PM";
            for (int k = 0; k < invigilatorsPerRoom && !minHeap.empty(); k++) {
                // Syllabus Unit 1: Stack Data Structure (LIFO Push & Pop)
                stack<FacEntry> setAsideStack;
                FacEntry candidate;
                bool found = false;

                while (!minHeap.empty()) {
                    FacEntry top = minHeap.top();
                    minHeap.pop();

                    if (assignedToday.find(top.fid) == assignedToday.end()) {
                        candidate = top;
                        found = true;
                        break;
                    } else {
                        // Push busy candidate onto LIFO stack to set aside
                        setAsideStack.push(top);
                    }
                }

                // Restore candidates back into Priority Queue (Min-Heap) using Stack pop
                while (!setAsideStack.empty()) {
                    minHeap.push(setAsideStack.top());
                    setAsideStack.pop();
                }

                if (!found) break; 

                Duty d;
                d.fid      = candidate.fid;
                d.fname    = facMap[candidate.fid].getName();
                d.date     = examDate;
                d.time     = dutySlot;
                d.roomName = rm.rname;
                d.building = rm.building;
                d.floor    = formatFloorName(rm.floor);

                facultyDuties[candidate.fid].push_back(d);
                allDuties.push_back(d);
                assignedToday.insert(candidate.fid);

                facMap[candidate.fid].duties++;

                candidate.duties++;
                pickedToday.push_back(candidate);
            }
        }

        for (const auto& fe : pickedToday) {
            minHeap.push(fe);
        }
    }
}

void DutyScheduler::assign(vector<Exam>& exams, int needPerSlot) {
    (void)needPerSlot;
    vector<Room> defaultRooms;
    assign(exams, defaultRooms, 1);
}

vector<Duty> DutyScheduler::getDutiesForFaculty(const string& fid) const {
    auto it = facultyDuties.find(fid);
    if (it != facultyDuties.end()) {
        return it->second;
    }
    return {};
}

void DutyScheduler::printSchedule() {
    cout << "\n=======================================================\n";
    cout << "          FACULTY DUTY ALLOCATION SUMMARY             \n";
    cout << "=======================================================\n";
    cout << "  Total Faculty Members    : " << facMap.size() << " teachers\n";
    cout << "  Total Duties Scheduled   : " << allDuties.size() << " duty slots\n";
    if (!facMap.empty()) {
        double avg = (double)allDuties.size() / facMap.size();
        cout << "  Average Duties / Teacher : " << fixed << setprecision(1) << avg << " duties\n";
    }
    cout << "=======================================================\n";

    cout << "\n  Preview of First 10 Duty Assignments:\n";
    cout << "  " << left << setw(13) << "Date"
         << setw(23) << "Time Slot"
         << setw(16) << "Building"
         << setw(13) << "Floor"
         << setw(22) << "Class / Room"
         << "Faculty Name" << "\n";
    cout << "  " << string(100, '-') << "\n";

    int limit = min((int)allDuties.size(), 10);
    for (int i = 0; i < limit; i++) {
        const auto& d = allDuties[i];
        cout << "  " << left << setw(13) << d.date
             << setw(23) << d.time
             << setw(16) << d.building
             << setw(13) << d.floor
             << setw(22) << d.roomName
             << d.fname << " (" << d.fid << ")\n";
    }
    cout << "  " << string(100, '-') << "\n\n";
}

int DutyScheduler::getDutyCount(const string& fid) {
    if (facMap.find(fid) == facMap.end()) {
        return 0;
    }
    return facMap[fid].duties;
}

void DutyScheduler::saveToFile(const string& fname) {
    ofstream fout(fname);
    if (!fout.is_open()) {
        cout << "[ERROR] Could not open file for writing: " << fname << endl;
        return;
    }

    fout << "Faculty_ID,Faculty_Name,Date,Time_Slot,Building,Floor,Room\n";

    for (const auto& d : allDuties) {
        fout << d.fid << ","
             << "\"" << d.fname << "\","
             << d.date << ","
             << d.time << ","
             << "\"" << d.building << "\","
             << "\"" << d.floor << "\","
             << "\"" << d.roomName << "\"\n";
    }

    fout.close();
    cout << "  [Duty] Saved " << allDuties.size() << " duty records to " << fname << "\n";
}
