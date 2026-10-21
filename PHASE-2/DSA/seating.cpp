#include "seating.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unordered_set>
using namespace std;


void SeatingEngine::init(const Room& rm) {
    roomName = rm.rname;
    building = rm.building;
    floor    = rm.floor;
    capacity = rm.cap;

    rows = (rm.rowGrp > 0) ? rm.rowGrp : 4;

    cols = (rm.cap + rows - 1) / rows;

    grid.resize(rows, vector<Seat>(cols));

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            grid[r][c].r      = r;
            grid[r][c].c      = c;
            grid[r][c].roll   = "";
            grid[r][c].branch = "";
        }
    }
}

bool SeatingEngine::canPlace(int r, int c, const string& branch) {
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;

            int nr = r + dr;
            int nc = c + dc;

            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;

            if (!grid[nr][nc].branch.empty() && grid[nr][nc].branch == branch) {
                return false;
            }
        }
    }
    return true;
}

bool SeatingEngine::placeStu(const Student& stu) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (!grid[r][c].roll.empty()) continue;

            if (canPlace(r, c, stu.branch)) {
                grid[r][c].roll   = stu.roll;
                grid[r][c].branch = stu.branch;
                return true;
            }
        }
    }
    return false;
}

void SeatingEngine::printGrid() {
    cout << "\n=== Seating Grid: " << roomName << " ===" << endl;
    cout << "     ";
    for (int c = 0; c < cols; c++) {
        cout << setw(5) << ("C" + to_string(c));
    }
    cout << endl;

    for (int r = 0; r < rows; r++) {
        cout << "R" << setw(3) << r << " ";
        for (int c = 0; c < cols; c++) {
            string cell = "---";
            if (!grid[r][c].branch.empty()) {
                cell = grid[r][c].branch.substr(0, 3);
            }
            cout << setw(5) << cell;
        }
        cout << endl;
    }
}

int SeatingEngine::emptySeats() {
    int count = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (grid[r][c].roll.empty()) {
                count++;
            }
        }
    }
    return count;
}

unordered_map<string, Allotment>
OptimalInterleavingStrategy::allocate(const vector<Student>& stus, const vector<Room>& rooms, bool printSummary) {
    unordered_map<string, Allotment> result;
    if (stus.empty() || rooms.empty()) return result;

    unordered_map<string, vector<Student>> branchMap;
    for (const auto& s : stus) {
        branchMap[s.branch].push_back(s);
    }

    unordered_map<string, size_t> branchIdx;
    for (const auto& kv : branchMap) {
        branchIdx[kv.first] = 0;
    }

    size_t totalStudents = stus.size();
    size_t totalPlaced = 0;
    int sessionNum = 1;

    unordered_map<string, int> roomSeatedCount;
    for (const auto& rm : rooms) roomSeatedCount[rm.rname] = 0;

    while (totalPlaced < totalStudents) {
        for (const auto& rm : rooms) {
            if (totalPlaced >= totalStudents) break;

            int rows = (rm.rowGrp > 0) ? rm.rowGrp : 4;
            int cols = (rm.cap + rows - 1) / rows;
            vector<vector<string>> grid(rows, vector<string>(cols, ""));

            int cap = rm.cap;
            for (int k = 0; k < cap; k++) {
                if (totalPlaced >= totalStudents) break;

                int r = k / cols;
                int c = k % cols;

                unordered_set<string> forbidden;
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue;
                        int nr = r + dr;
                        int nc = c + dc;
                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                            if (!grid[nr][nc].empty()) {
                                forbidden.insert(grid[nr][nc]);
                            }
                        }
                    }
                }

                string bestBranch = "";
                size_t maxRem = 0;
                for (const auto& kv : branchMap) {
                    const string& bName = kv.first;
                    size_t rem = kv.second.size() - branchIdx[bName];
                    if (rem > 0 && forbidden.find(bName) == forbidden.end()) {
                        if (rem > maxRem) {
                            maxRem = rem;
                            bestBranch = bName;
                        }
                    }
                }

                if (bestBranch.empty()) {
                    for (const auto& kv : branchMap) {
                        size_t rem = kv.second.size() - branchIdx[kv.first];
                        if (rem > maxRem) {
                            maxRem = rem;
                            bestBranch = kv.first;
                        }
                    }
                }

                if (bestBranch.empty()) break;

                const Student& stu = branchMap[bestBranch][branchIdx[bestBranch]++];
                grid[r][c] = stu.branch;
                totalPlaced++;
                roomSeatedCount[rm.rname]++;

                Allotment alt;
                alt.roll     = stu.roll;
                alt.roomName = rm.rname;
                alt.building = rm.building;
                alt.floor    = formatFloorName(rm.floor);
                alt.row      = r + 1;
                alt.col      = c + 1;
                alt.seatNo   = k + 1;
                alt.session  = "Session " + to_string(sessionNum);

                result[stu.roll] = alt;
            }
        }
        sessionNum++;
    }

    if (printSummary) {
        cout << "\n=======================================================\n";
        cout << "            EXAM SEATING ALLOCATION SUMMARY            \n";
        cout << "=======================================================\n";
        cout << "  " << left << setw(28) << "Room / Hall Name"
             << setw(16) << "Capacity"
             << setw(14) << "Assigned" << "\n";
        cout << "  -----------------------------------------------------\n";
        int totalCapSum = 0;
        int totalSeatedSum = 0;
        for (const auto& rm : rooms) {
            int seated = roomSeatedCount[rm.rname];
            cout << "  " << left << setw(28) << rm.rname
                 << setw(16) << rm.cap
                 << setw(14) << seated << "\n";
            totalCapSum += rm.cap;
            totalSeatedSum += seated;
        }
        cout << "  -----------------------------------------------------\n";
        cout << "  Total Examination Rooms   : " << rooms.size() << " rooms\n";
        cout << "  Room Seating Capacity     : " << totalCapSum << " seats/session\n";
        cout << "  Total Students Allocated  : " << totalPlaced << " / " << totalStudents << " (100%)\n";
        cout << "  Total Examination Sessions: " << (sessionNum - 1) << " sessions\n";
        cout << "=======================================================\n\n";
    }

    return result;
}

unordered_map<string, Allotment>
allocateSeats(const vector<Student>& stus, const vector<Room>& rooms, bool printSummary) {
    OptimalInterleavingStrategy strategy;
    SeatingManager manager(&strategy);
    return manager.execute(stus, rooms, printSummary);
}

