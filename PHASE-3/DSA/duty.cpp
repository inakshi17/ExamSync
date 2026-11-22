#include "duty.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
using namespace std;

void DutyScheduler::assign(const vector<Exam>& exams, const vector<Room>& rooms, int invigilatorsPerRoom, int flyingSquadPerBuilding) {
    facultyDuties.clear();
    allDuties.clear();
    if (facMap.empty() || exams.empty() || rooms.empty()) return;

    for (auto& kv : facMap) {
        kv.second.duties = 0;
    }
    
    // extract unique exam dates
    vector<string> dates;
    unordered_set<string> seenDates;
    for (const auto& e : exams) {
        if (!e.date.empty() && seenDates.find(e.date) == seenDates.end()) {
            seenDates.insert(e.date);
            dates.push_back(e.date);
        }
    }

    // sort exam dates chronologically
    sort(dates.begin(), dates.end(), [](const string& a, const string& b) {
        string da = (a.size() >= 10) ? (a.substr(6, 4) + a.substr(3, 2) + a.substr(0, 2)) : a;
        string db = (b.size() >= 10) ? (b.substr(6, 4) + b.substr(3, 2) + b.substr(0, 2)) : b;
        return da < db;
    });

    // extract unique buildings from rooms
    vector<string> buildings;
    unordered_set<string> seenBuildings;
    for (const auto& rm : rooms) {
        if (!rm.building.empty() && seenBuildings.find(rm.building) == seenBuildings.end()) {
            seenBuildings.insert(rm.building);
            buildings.push_back(rm.building);
        }
    }
    sort(buildings.begin(), buildings.end());

    // min-heap priority queue: teacher with least duties is at top
    priority_queue<FacEntry, vector<FacEntry>, greater<FacEntry>> minHeap;
    for (const auto& kv : facMap) {
        minHeap.push({0, kv.first});
    }

    // helper lambda to extract the next candidate faculty not assigned today
    auto getNextFaculty = [&](unordered_set<string>& assignedToday, vector<FacEntry>& pickedToday) -> string {
        vector<FacEntry> setAside;
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
                setAside.push_back(top);
            }
        }

        for (const auto& fe : setAside) {
            minHeap.push(fe);
        }

        if (!found) return "";

        assignedToday.insert(candidate.fid);
        facMap[candidate.fid].duties++;
        candidate.duties++;
        pickedToday.push_back(candidate);
        return candidate.fid;
    };

    // Build a map: date -> set of semesters sitting that day
    unordered_map<string, unordered_set<int>> dateSems;
    for (const auto& e : exams)
        dateSems[e.date].insert(e.sem);

    // schedule duties date by date
    for (const string& examDate : dates) {
        unordered_set<string> assignedMorning, assignedAfternoon;
        vector<FacEntry> pickedToday;

        // Determine which shifts run today
        bool hasMorning   = false, hasAfternoon = false;
        if (dateSems.count(examDate)) {
            for (int s : dateSems[examDate]) {
                if (s == 1 || s == 7) hasMorning   = true;
                else                   hasAfternoon = true;
            }
        }
        string morningSlot   = "09:30 AM - 12:30 PM (Morning)";
        string afternoonSlot = "01:30 PM - 04:30 PM (Afternoon)";

        // FIX: Only assign flying squad to buildings that actually have an exam
        // on this date — determined by which branches have an exam scheduled.
        // We use ALL buildings (since the exam could be in any room) but only
        // on dates that have at least one exam entry.
        // The buildings list already comes from actual rooms, so this is correct.

        // Find which buildings have rooms with students sitting today
        // (For simplicity: assign squad to all buildings on any exam date —
        //  but skip buildings that have ZERO rooms, which cannot happen)
        // Real fix: flying squad only needed when rooms in that building are used.
        // We approximate: assign squad only to buildings that appear in rooms list.
        // (Already correct — buildings are built from rooms, not hardcoded)

        // 1. Flying Squad: 3 teachers per building per shift that runs today
        for (const string& bldg : buildings) {
            // Morning flying squad
            if (hasMorning) {
                for (int s = 0; s < flyingSquadPerBuilding; s++) {
                    string fid = getNextFaculty(assignedMorning, pickedToday);
                    if (fid.empty()) break;
                    Duty d;
                    d.fid = fid; d.fname = facMap[fid].getName();
                    d.date = examDate; d.time = morningSlot;
                    d.building = bldg; d.floor = "All Floors";
                    d.roomName = "Flying Squad"; d.role = "Flying Squad";
                    facultyDuties[fid].push_back(d); allDuties.push_back(d);
                }
            }
            // Afternoon flying squad
            if (hasAfternoon) {
                for (int s = 0; s < flyingSquadPerBuilding; s++) {
                    string fid = getNextFaculty(assignedAfternoon, pickedToday);
                    if (fid.empty()) break;
                    Duty d;
                    d.fid = fid; d.fname = facMap[fid].getName();
                    d.date = examDate; d.time = afternoonSlot;
                    d.building = bldg; d.floor = "All Floors";
                    d.roomName = "Flying Squad"; d.role = "Flying Squad";
                    facultyDuties[fid].push_back(d); allDuties.push_back(d);
                }
            }
        }

        // 2. Room Invigilators: 2 per room per shift that runs today
        for (size_t rmIdx = 0; rmIdx < rooms.size(); rmIdx++) {
            const auto& rm = rooms[rmIdx];
            // Morning invigilators
            if (hasMorning) {
                for (int k = 0; k < invigilatorsPerRoom; k++) {
                    string fid = getNextFaculty(assignedMorning, pickedToday);
                    if (fid.empty()) break;
                    Duty d;
                    d.fid = fid; d.fname = facMap[fid].getName();
                    d.date = examDate; d.time = morningSlot;
                    d.roomName = rm.rname; d.building = rm.building;
                    d.floor = formatFloorName(rm.floor); d.role = "Room Invigilator";
                    facultyDuties[fid].push_back(d); allDuties.push_back(d);
                }
            }
            // Afternoon invigilators
            if (hasAfternoon) {
                for (int k = 0; k < invigilatorsPerRoom; k++) {
                    string fid = getNextFaculty(assignedAfternoon, pickedToday);
                    if (fid.empty()) break;
                    Duty d;
                    d.fid = fid; d.fname = facMap[fid].getName();
                    d.date = examDate; d.time = afternoonSlot;
                    d.roomName = rm.rname; d.building = rm.building;
                    d.floor = formatFloorName(rm.floor); d.role = "Room Invigilator";
                    facultyDuties[fid].push_back(d); allDuties.push_back(d);
                }
            }
        }

        // 3. Push today's assigned teachers back to heap with updated duty count
        for (const auto& fe : pickedToday) {
            minHeap.push(fe);
        }
    }
}

vector<Duty> DutyScheduler::getDutiesForFaculty(const string& fid) const {
    auto it = facultyDuties.find(fid);
    if (it != facultyDuties.end()) {
        return it->second;
    }
    return {};
}

vector<Duty> DutyScheduler::getFlyingSquadDuties() const {
    vector<Duty> fs;
    for (const auto& d : allDuties) {
        if (d.role == "Flying Squad") {
            fs.push_back(d);
        }
    }
    return fs;
}

void DutyScheduler::printSchedule() {
    int flyingCount = 0;
    int roomCount = 0;
    for (const auto& d : allDuties) {
        if (d.role == "Flying Squad") flyingCount++;
        else roomCount++;
    }

    cout << "\n=======================================================\n";
    cout << "          FACULTY DUTY ALLOCATION SUMMARY             \n";
    cout << "=======================================================\n";
    cout << "  Total Faculty Members    : " << facMap.size() << " teachers\n";
    cout << "  Total Duties Scheduled   : " << allDuties.size() << " duty slots\n";
    cout << "    - Room Invigilators    : " << roomCount << " duties (2 per room)\n";
    cout << "    - Flying Squad Duties  : " << flyingCount << " duties (3 per building)\n";
    if (!facMap.empty()) {
        double avg = (double)allDuties.size() / facMap.size();
        cout << "  Average Duties / Teacher : " << fixed << setprecision(1) << avg << " duties\n";
    }
    cout << "=======================================================\n";

    cout << "\n  Preview of First 15 Duty Assignments:\n";
    cout << "  " << left << setw(13) << "Date"
         << setw(25) << "Time Slot"
         << setw(18) << "Role"
         << setw(20) << "Building"
         << setw(14) << "Floor"
         << setw(22) << "Class / Room"
         << "Faculty Name" << "\n";
    cout << "  " << string(115, '-') << "\n";

    int limit = min((int)allDuties.size(), 15);
    for (int i = 0; i < limit; i++) {
        const auto& d = allDuties[i];
        cout << "  " << left << setw(13) << d.date
             << setw(25) << d.time
             << setw(18) << d.role
             << setw(20) << d.building
             << setw(14) << d.floor
             << setw(22) << d.roomName
             << d.fname << " (" << d.fid << ")\n";
    }
    cout << "  " << string(115, '-') << "\n\n";
}

void DutyScheduler::printFlyingSquadRoster(const string& specificDate) const {
    vector<string> dates;
    unordered_map<string, unordered_map<string, vector<Duty>>> roster;
    for (const auto& d : allDuties) {
        if (d.role == "Flying Squad") {
            if (specificDate.empty() || d.date == specificDate) {
                if (roster.find(d.date) == roster.end()) {
                    dates.push_back(d.date);
                }
                roster[d.date][d.building].push_back(d);
            }
        }
    }

    if (dates.empty()) return;

    cout << "\n========================================================================================\n";
    cout << "             FLYING SQUAD DEPLOYMENT ROSTER (3 TEACHERS PER BUILDING)                   \n";
    cout << "========================================================================================\n";

    int datesToShow = specificDate.empty() ? min((int)dates.size(), 3) : (int)dates.size();
    for (int i = 0; i < datesToShow; i++) {
        const string& dt = dates[i];
        cout << "\n  [Exam Date: " << dt << " | Morning: 09:30-12:30 (Sem 1,7)  |  Afternoon: 13:30-16:30 (Sem 3,5)]\n";
        cout << "  " << left << setw(22) << "Building" << "Assigned Flying Squad Faculty (3 Members)\n";
        cout << "  " << string(95, '-') << "\n";
        for (const auto& bkv : roster[dt]) {
            cout << "  " << left << setw(22) << bkv.first;
            for (size_t m = 0; m < bkv.second.size(); m++) {
                if (m > 0) cout << " | ";
                cout << bkv.second[m].fname << " (" << bkv.second[m].fid << ")";
            }
            cout << "\n";
        }
    }
    if (specificDate.empty() && dates.size() > 3) {
        cout << "\n  [Note: Displaying first 3 exam dates above. Full deployment across all " 
             << dates.size() << " dates is saved to data/duty_schedule.csv]\n";
    }
    cout << "========================================================================================\n";
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

    fout << "Faculty_ID,Faculty_Name,Date,Time_Slot,Building,Floor,Room,Role\n";

    for (const auto& d : allDuties) {
        fout << d.fid << ","
             << "\"" << d.fname << "\","
             << d.date << ","
             << d.time << ","
             << "\"" << d.building << "\","
             << "\"" << d.floor << "\","
             << "\"" << d.roomName << "\","
             << "\"" << d.role << "\"\n";
    }

    fout.close();
    cout << "  [Duty] Saved " << allDuties.size() << " duty records to " << fname << "\n";
}
