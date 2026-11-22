#include "seating.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unordered_set>
#include <cmath>
#include <cctype>
using namespace std;

// ── SeatingEngine helpers ────────────────────────────────────────────────────

// Realistic grid: use cols from rooms.csv directly, derive rows from capacity.
// Real classrooms: rows >= cols (more rows than columns).
// rooms.csv columns: Row_Groups=rowGrp, Seat_Columns=cols, Seats_Per_Column=perCol
void SeatingEngine::init(const Room& rm) {
    roomName = rm.rname;
    building = rm.building;
    floor    = rm.floor;
    capacity = rm.cap;

    // Use actual columns from rooms.csv, derive rows so rows >= cols (realistic)
    cols = (rm.cols > 0) ? rm.cols : 8;
    rows = (rm.cap + cols - 1) / cols;  // ceil(cap / cols) → more rows than cols

    grid.resize(rows, vector<Seat>(cols));
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++) {
            grid[r][c].r = r; grid[r][c].c = c;
            grid[r][c].roll = ""; grid[r][c].branch = "";
        }
}

// Horizontal/diagonal neighbors must not share the same paper code
bool SeatingEngine::canPlace(int r, int c, const string& code) {
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            if (dc == 0) continue;  // vertical (same column) is allowed
            int nr = r + dr, nc = c + dc;
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
            if (!grid[nr][nc].branch.empty() && grid[nr][nc].branch == code)
                return false;
        }
    }
    return true;
}

bool SeatingEngine::placeStu(const Student& stu) {
    string code = getStudentCode(stu);
    for (int c = 0; c < cols; c++)
        for (int r = 0; r < rows; r++) {
            if (!grid[r][c].roll.empty()) continue;
            if (canPlace(r, c, code)) {
                grid[r][c].roll = stu.roll;
                grid[r][c].branch = code;
                return true;
            }
        }
    return false;
}

void SeatingEngine::printGrid() {
    cout << "\n=== Seating Grid: " << roomName << " ===\n";
    cout << "     ";
    for (int c = 0; c < cols; c++) cout << setw(7) << ("C" + to_string(c+1));
    cout << "\n";
    for (int r = 0; r < rows; r++) {
        cout << "R" << setw(3) << (r+1) << " ";
        for (int c = 0; c < cols; c++) {
            string cell = grid[r][c].branch.empty() ? " --- " : grid[r][c].branch;
            cout << setw(7) << cell;
        }
        cout << "\n";
    }
}

int SeatingEngine::emptySeats() {
    int count = 0;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (grid[r][c].roll.empty()) count++;
    return count;
}

// ── Main allocation strategy ─────────────────────────────────────────────────

// Check no same-code horizontal/diagonal neighbor in a string grid
static bool isSeatSafe(const vector<vector<string>>& grid, int r, int c,
                        int rows, int cols, const string& code) {
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            if (dc == 0) continue;
            int nr = r+dr, nc = c+dc;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols)
                if (!grid[nr][nc].empty() && grid[nr][nc] == code) return false;
        }
    }
    return true;
}

unordered_map<string, Allotment>
OptimalInterleavingStrategy::allocate(const vector<Student>& stus,
                                      const vector<Room>& rooms,
                                      bool printSummary) {
    unordered_map<string, Allotment> result;
    if (stus.empty() || rooms.empty()) return result;

    // Group students by paper code (e.g. TCS3, BCA1, MCA3)
    unordered_map<string, vector<Student>> codeMap;
    for (const auto& s : stus) codeMap[getStudentCode(s)].push_back(s);

    unordered_map<string, size_t> codeIdx;
    for (const auto& kv : codeMap) codeIdx[kv.first] = 0;

    // Sort codes: largest group first, then alphabetically
    // (No even-sem priority needed — we only receive odd-sem students per date)
    vector<string> allCodes;
    for (const auto& kv : codeMap) allCodes.push_back(kv.first);
    sort(allCodes.begin(), allCodes.end(), [&](const string& a, const string& b) {
        if (codeMap[a].size() != codeMap[b].size())
            return codeMap[a].size() > codeMap[b].size();  // larger group first
        return a < b;
    });

    size_t totalStudents = stus.size();
    size_t totalPlaced   = 0;

    unordered_map<string, int> roomSeatedCount;
    for (const auto& rm : rooms) roomSeatedCount[rm.rname] = 0;

    while (totalPlaced < totalStudents) {
        size_t placedAtStartOfSession = totalPlaced;

        for (const auto& rm : rooms) {
            if (totalPlaced >= totalStudents) break;

            // Realistic grid: cols from rooms.csv, rows = ceil(cap/cols)
            int cols = (rm.cols > 0) ? rm.cols : 8;
            int rows = (rm.cap + cols - 1) / cols;
            vector<vector<string>> grid(rows, vector<string>(cols, ""));

            // Allow at most 4 distinct paper codes per room (invigilator limit)
            vector<string> roomActiveCodes;
            unordered_set<string> roomCodeSet;
            for (const string& c : allCodes) {
                if (roomActiveCodes.size() >= 4) break;
                if (codeIdx[c] < codeMap[c].size()) {
                    roomActiveCodes.push_back(c);
                    roomCodeSet.insert(c);
                }
            }
            if (roomActiveCodes.empty()) break;

            int cap = rm.cap;
            int seatsFilledInRoom = 0;

            // Fill column by column; alternate codes across columns
            for (int c = 0; c < cols; c++) {
                if (totalPlaced >= totalStudents || seatsFilledInRoom >= cap) break;

                // Pick a code different from the previous column
                string colPrevCode = (c > 0) ? grid[0][c-1] : "";
                string colCode = "";
                for (const string& cand : roomActiveCodes) {
                    if (codeIdx[cand] < codeMap[cand].size() && cand != colPrevCode) {
                        colCode = cand; break;
                    }
                }
                // Try admitting a new code if room still has slots
                if (colCode.empty() && roomCodeSet.size() < 4) {
                    for (const string& cand : allCodes) {
                        if (!roomCodeSet.count(cand) &&
                            codeIdx[cand] < codeMap[cand].size() &&
                            cand != colPrevCode) {
                            colCode = cand;
                            roomActiveCodes.push_back(cand);
                            roomCodeSet.insert(cand);
                            break;
                        }
                    }
                }
                // Fallback: any available code
                if (colCode.empty()) {
                    for (const string& cand : roomActiveCodes)
                        if (codeIdx[cand] < codeMap[cand].size()) { colCode = cand; break; }
                }
                if (colCode.empty()) continue;

                // Fill seats vertically down column c
                for (int r = 0; r < rows; r++) {
                    if (totalPlaced >= totalStudents || seatsFilledInRoom >= cap) break;

                    // Switch code if current exhausted
                    if (codeIdx[colCode] >= codeMap[colCode].size()) {
                        string nextCode = "";
                        for (const string& cand : roomActiveCodes)
                            if (codeIdx[cand] < codeMap[cand].size() &&
                                isSeatSafe(grid, r, c, rows, cols, cand))
                            { nextCode = cand; break; }
                        if (!nextCode.empty()) colCode = nextCode;
                        else continue;
                    }

                    if (!isSeatSafe(grid, r, c, rows, cols, colCode)) continue;

                    const Student& stu = codeMap[colCode][codeIdx[colCode]++];
                    grid[r][c] = colCode;
                    totalPlaced++;
                    seatsFilledInRoom++;
                    roomSeatedCount[rm.rname]++;

                    Allotment alt;
                    alt.roll     = stu.roll;
                    alt.roomName = rm.rname;
                    alt.building = rm.building;
                    alt.floor    = formatFloorName(rm.floor);
                    alt.row      = r + 1;
                    alt.col      = c + 1;
                    alt.seatNo   = seatsFilledInRoom;
                    alt.session  = "Whole Exam Session (Fixed Seat)";
                    alt.code     = colCode;
                    result[stu.roll] = alt;
                }
            }
        }

        // Break if no progress (prevents infinite loop)
        if (totalPlaced == placedAtStartOfSession) {
            // Fallback: assign remaining students to any seat
            for (const auto& rm : rooms) {
                if (totalPlaced >= totalStudents) break;
                for (const auto& kv : codeMap) {
                    const string& code = kv.first;
                    while (codeIdx[code] < codeMap[code].size() && totalPlaced < totalStudents) {
                        const Student& stu = codeMap[code][codeIdx[code]++];
                        totalPlaced++;
                        Allotment alt;
                        alt.roll = stu.roll; alt.roomName = rm.rname;
                        alt.building = rm.building;
                        alt.floor = formatFloorName(rm.floor);
                        alt.row = 1; alt.col = 1; alt.seatNo = 1;
                        alt.session = "Whole Exam Session (Fixed Seat)";
                        alt.code = code;
                        result[stu.roll] = alt;
                    }
                }
            }
            break;
        }
    }

    if (printSummary) {
        cout << "\n=======================================================\n";
        cout << "            EXAM SEATING ALLOCATION SUMMARY\n";
        cout << "=======================================================\n";
        cout << "  " << left << setw(28) << "Room / Hall"
             << setw(10) << "Cols" << setw(10) << "Rows"
             << setw(12) << "Capacity" << setw(10) << "Seated" << "\n";
        cout << "  -------------------------------------------------------\n";
        int totalCapSum = 0, totalSeatedSum = 0;
        for (const auto& rm : rooms) {
            int c = (rm.cols > 0) ? rm.cols : 8;
            int r = (rm.cap + c - 1) / c;
            int seated = roomSeatedCount[rm.rname];
            cout << "  " << left << setw(28) << rm.rname
                 << setw(10) << c << setw(10) << r
                 << setw(12) << rm.cap << setw(10) << seated << "\n";
            totalCapSum += rm.cap;
            totalSeatedSum += seated;
        }
        cout << "  -------------------------------------------------------\n";
        cout << "  Total rooms     : " << rooms.size() << "\n";
        cout << "  Total capacity  : " << totalCapSum << " seats\n";
        cout << "  Total seated    : " << totalSeatedSum << " / " << totalStudents << "\n";
        cout << "  Rule            : Max 4 paper codes/room, alternating columns\n";
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

// ── Seat-preserving allocation for per-date runs ──────────────────────────────
// preAssigned: students who already have a seat from a previous date's run.
//   - Their seat is copied into result unchanged (fixed seat across all exams).
//   - Their physical cell is marked occupied so no new student gets the same seat.
// New students (not in preAssigned) get a fresh seat from remaining cells.
unordered_map<string, Allotment>
allocateSeats(const vector<Student>& stus,
              const vector<Room>& rooms,
              const unordered_map<string, Allotment>& preAssigned,
              bool printSummary) {

    if (preAssigned.empty())
        return allocateSeats(stus, rooms, printSummary);

    // Split: already have seat vs need new seat
    vector<Student> needsSeat;
    unordered_map<string, Allotment> result;

    // Pre-occupied cells per room: set of (row-1)*cols+(col-1) linear indices
    // We store per room: map<roomName, set<int>>
    unordered_map<string, unordered_set<int>> roomOccupied;

    for (const auto& s : stus) {
        auto it = preAssigned.find(s.roll);
        if (it != preAssigned.end()) {
            // Keep existing seat
            result[s.roll] = it->second;
            const Allotment& a = it->second;
            // Mark the physical cell as occupied for this date
            // We need cols to compute linear index
            // Find room cols
            int rmCols = 8;
            for (const auto& rm : rooms) {
                if (rm.rname == a.roomName) { rmCols = (rm.cols > 0) ? rm.cols : 8; break; }
            }
            int idx = (a.row - 1) * rmCols + (a.col - 1);
            roomOccupied[a.roomName].insert(idx);
        } else {
            needsSeat.push_back(s);
        }
    }

    if (needsSeat.empty()) return result;

    // Group new students by code
    unordered_map<string, vector<Student>> codeMap;
    for (const auto& s : needsSeat) codeMap[getStudentCode(s)].push_back(s);

    unordered_map<string, size_t> codeIdx;
    for (const auto& kv : codeMap) codeIdx[kv.first] = 0;

    vector<string> allCodes;
    for (const auto& kv : codeMap) allCodes.push_back(kv.first);
    sort(allCodes.begin(), allCodes.end(), [&](const string& a, const string& b) {
        if (codeMap[a].size() != codeMap[b].size())
            return codeMap[a].size() > codeMap[b].size();
        return a < b;
    });

    size_t totalNew = needsSeat.size();
    size_t totalPlaced = 0;
    unordered_map<string, int> roomSeatedCount;
    for (const auto& rm : rooms) roomSeatedCount[rm.rname] = 0;

    while (totalPlaced < totalNew) {
        size_t placedAtStart = totalPlaced;

        for (const auto& rm : rooms) {
            if (totalPlaced >= totalNew) break;

            int cols = (rm.cols > 0) ? rm.cols : 8;
            int rows = (rm.cap + cols - 1) / cols;
            vector<vector<string>> grid(rows, vector<string>(cols, ""));

            // Pre-mark occupied cells (from preAssigned students sitting here today)
            int alreadyUsed = 0;
            if (roomOccupied.count(rm.rname)) {
                for (int idx : roomOccupied[rm.rname]) {
                    int r = idx / cols, c = idx % cols;
                    if (r < rows && c < cols) {
                        grid[r][c] = "__TAKEN__";
                        alreadyUsed++;
                    }
                }
            }

            // Available seats in this room for new students
            int availableCap = rm.cap - alreadyUsed;
            if (availableCap <= 0) continue;

            vector<string> roomActiveCodes;
            unordered_set<string> roomCodeSet;
            for (const string& c : allCodes) {
                if (roomActiveCodes.size() >= 4) break;
                if (codeIdx[c] < codeMap[c].size()) {
                    roomActiveCodes.push_back(c);
                    roomCodeSet.insert(c);
                }
            }
            if (roomActiveCodes.empty()) break;

            int seatsFilledInRoom = 0;

            for (int c = 0; c < cols; c++) {
                if (totalPlaced >= totalNew || seatsFilledInRoom >= availableCap) break;

                string colPrevCode = (c > 0) ? grid[0][c-1] : "";
                // Clear __TAKEN__ tag for column-code selection
                if (colPrevCode == "__TAKEN__") colPrevCode = "";

                string colCode = "";
                for (const string& cand : roomActiveCodes)
                    if (codeIdx[cand] < codeMap[cand].size() && cand != colPrevCode)
                    { colCode = cand; break; }

                if (colCode.empty() && roomCodeSet.size() < 4) {
                    for (const string& cand : allCodes)
                        if (!roomCodeSet.count(cand) &&
                            codeIdx[cand] < codeMap[cand].size() &&
                            cand != colPrevCode)
                        { colCode = cand; roomActiveCodes.push_back(cand); roomCodeSet.insert(cand); break; }
                }
                if (colCode.empty())
                    for (const string& cand : roomActiveCodes)
                        if (codeIdx[cand] < codeMap[cand].size()) { colCode = cand; break; }
                if (colCode.empty()) continue;

                for (int r = 0; r < rows; r++) {
                    if (totalPlaced >= totalNew || seatsFilledInRoom >= availableCap) break;
                    if (!grid[r][c].empty()) continue;  // skip pre-taken cells

                    if (codeIdx[colCode] >= codeMap[colCode].size()) {
                        string nextCode = "";
                        for (const string& cand : roomActiveCodes)
                            if (codeIdx[cand] < codeMap[cand].size() &&
                                isSeatSafe(grid, r, c, rows, cols, cand))
                            { nextCode = cand; break; }
                        if (!nextCode.empty()) colCode = nextCode; else continue;
                    }
                    if (!isSeatSafe(grid, r, c, rows, cols, colCode)) continue;

                    const Student& stu = codeMap[colCode][codeIdx[colCode]++];
                    grid[r][c] = colCode;
                    totalPlaced++;
                    seatsFilledInRoom++;
                    roomSeatedCount[rm.rname]++;

                    Allotment alt;
                    alt.roll     = stu.roll;
                    alt.roomName = rm.rname;
                    alt.building = rm.building;
                    alt.floor    = formatFloorName(rm.floor);
                    alt.row      = r + 1;
                    alt.col      = c + 1;
                    alt.seatNo   = alreadyUsed + seatsFilledInRoom;
                    alt.session  = "Whole Exam Session (Fixed Seat)";
                    alt.code     = colCode;
                    result[stu.roll] = alt;
                }
            }
        }

        if (totalPlaced == placedAtStart) break;  // no progress
    }

    if (printSummary) {
        cout << "  Pre-assigned (seat kept) : " << preAssigned.size() << " students\n";
        cout << "  Newly allocated          : " << totalPlaced << " / " << totalNew << " students\n";
    }
    return result;
}
