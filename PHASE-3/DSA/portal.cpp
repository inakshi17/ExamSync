#include "portal.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <unordered_set>
#include <algorithm>
using namespace std;

// ─────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────

void Portal::sep() {
    cout << "============================================\n";
}

void Portal::cls() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ── Insertion sort for small sub-arrays (≤16 elements) ──────────────────────
template<typename T, typename Cmp>
static void insertionSort(vector<T>& a, int l, int r, Cmp cmp) {
    for (int i = l + 1; i <= r; ++i) {
        T key = a[i]; int j = i - 1;
        while (j >= l && cmp(key, a[j])) { a[j + 1] = a[j]; --j; }
        a[j + 1] = key;
    }
}

// ── Two-way merge step ───────────────────────────────────────────────────────
template<typename T, typename Cmp>
static void twoWayMerge(vector<T>& a, int l, int m, int r, Cmp cmp) {
    vector<T> L(a.begin() + l, a.begin() + m + 1);
    vector<T> R(a.begin() + m + 1, a.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < (int)L.size() && j < (int)R.size())
        a[k++] = (cmp(L[i], R[j]) || !cmp(R[j], L[i])) ? L[i++] : R[j++];
    while (i < (int)L.size()) a[k++] = L[i++];
    while (j < (int)R.size()) a[k++] = R[j++];
}

// ── Recursive two-way merge sort (Unit 4 DSA) ───────────────────────────────
template<typename T, typename Cmp>
static void mergeSort(vector<T>& a, int l, int r, Cmp cmp) {
    if (r - l + 1 <= 16) { insertionSort(a, l, r, cmp); return; }
    int m = l + (r - l) / 2;
    mergeSort(a, l, m, cmp);
    mergeSort(a, m + 1, r, cmp);
    twoWayMerge(a, l, m, r, cmp);
}

template<typename T, typename Cmp>
static void twoWayMergeSort(vector<T>& a, Cmp cmp) {
    if (!a.empty()) mergeSort(a, 0, (int)a.size() - 1, cmp);
}

// ── Binary search on roll-sorted student array — O(log n) ───────────────────
static int binarySearch(const vector<Student>& arr, const string& roll, int& steps) {
    steps = 0;
    int lo = 0, hi = (int)arr.size() - 1;
    while (lo <= hi) {
        ++steps;
        int mid = lo + (hi - lo) / 2;
        if (arr[mid].roll == roll) return mid;
        if (arr[mid].roll < roll) lo = mid + 1; else hi = mid - 1;
    }
    return -1;
}

// ── Date string → sortable YYYYMMDD key ─────────────────────────────────────
static string dateKey(const string& d) {
    if (d.size() >= 10) return d.substr(6, 4) + d.substr(3, 2) + d.substr(0, 2);
    return d;
}

// Load saved seating plan from CSV back into seatMap on startup
static void loadSeatingPlan(const string& path,
                             unordered_map<string, Allotment>& seatMap,
                             string& label)
{
    ifstream f(path);
    if (!f.is_open()) return;

    string line;
    getline(f, line); // skip header
    int loaded = 0;

    while (getline(f, line)) {
        if (line.empty()) continue;

        // Parse: Roll_No,Student_Name,Course,Branch,Semester,Paper_Code,
        //        Building,Floor,Room,Seat_No,Row,Col,Session,ExamDate
        // Fields may be quoted — simple CSV parse
        vector<string> fields;
        stringstream ss(line);
        string token;
        while (getline(ss, token, ',')) {
            if (!token.empty() && token.front() == '"') {
                // remove surrounding quotes
                token = token.substr(1);
                if (!token.empty() && token.back() == '"')
                    token.pop_back();
            }
            fields.push_back(token);
        }

        if (fields.size() < 13) continue;

        Allotment a;
        a.roll     = fields[0];
        // fields[1]=name, [2]=course, [3]=branch, [4]=sem, [5]=paper_code
        a.code     = fields[5];
        a.building = fields[6];
        a.floor    = fields[7];
        a.roomName = fields[8];
        try { a.seatNo = stoi(fields[9]); } catch (...) { a.seatNo = 0; }
        try { a.row    = stoi(fields[10]); } catch (...) { a.row = 0; }
        try { a.col    = stoi(fields[11]); } catch (...) { a.col = 0; }
        a.session  = fields[12];
        // field[13] = ExamDate if present
        if (fields.size() >= 14) a.examDate = fields[13];

        seatMap[a.roll] = a;
        loaded++;
    }

    if (loaded > 0) {
        label = "Loaded from saved plan (" + to_string(loaded) + " students)";
        cout << "[Portal] Seating plan restored: " << loaded << " records\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  loadAll — startup data loader
// ─────────────────────────────────────────────────────────────────────────────
void Portal::loadAll(const string& dataPath) {
    cout << "[Portal] Loading ODD Semester Examination System...\n";

    auth.load    (dataPath + "Authentication.csv");
    loadStudents (dataPath + "students.csv",    stuMap);
    loadFaculty  (dataPath + "faculty.csv",     facMap);
    loadRooms    (dataPath + "rooms.csv",        rooms);
    loadExams    (dataPath + "exam_schedule.csv", exams);
    ufmLog.load  (dataPath + "ufm_records.csv");

    // Sort students by roll for O(log n) binary search
    sortedStudents.clear();
    sortedStudents.reserve(stuMap.size());
    for (const auto& kv : stuMap) sortedStudents.push_back(kv.second);
    twoWayMergeSort(sortedStudents, [](const Student& a, const Student& b) {
        return a.roll < b.roll;
    });

    cout << "[Portal] Students loaded : " << stuMap.size() << "\n";
    cout << "[Portal] Faculty loaded  : " << facMap.size() << "\n";
    cout << "[Portal] Exam records    : " << exams.size()  << "\n";

    // Restore seating plan from saved CSV (seats persist across restarts)
    seatMap.clear();
    activeAllocatedLabel.clear();
    loadSeatingPlan(dataPath + "seating_plan.csv", seatMap, activeAllocatedLabel);

    // Reset duty schedule each session (admin re-runs it each exam cycle)
    dutyMap.clear();
    activeDutyAllocatedLabel.clear();
    for (auto& kv : facMap) kv.second.duties = 0;

    ofstream dutyReset(dataPath + "duty_schedule.csv", ios::trunc);
    if (dutyReset.is_open())
        dutyReset << "Faculty_ID,Faculty_Name,Date,Time_Slot,Building,Floor,Room,Role\n";

    cout << "[Portal] System ready.\n\n";
}

// ─────────────────────────────────────────────
//  run — login loop
// ─────────────────────────────────────────────
void Portal::run() {
    string uname, pwd;
    while (true) {
        cls(); sep();
        cout << "   EXAMSYNC — ODD SEMESTER EXAMINATION SYSTEM\n";
        cout << "   Semesters 1 · 3 · 5 · 7  |  Nov 2026 – Jan 2027\n"; sep();
        cout << "  Username (or '0' to exit): ";
        if (!(cin >> uname) || uname == "0") break;
        cout << "  Password: ";
        if (!(cin >> pwd)) break;
        cout << "\n";

        User* u = auth.login(uname, pwd);
        if (!u) {
            cout << "  [!] Invalid credentials.\n  Press Enter to retry...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
            continue;
        }

        cout << "  Welcome, " << u->uname << "!  Role: " << u->role << "\n\n";

        if      (u->role == "STUDENT") studentMenu(*u);
        else if (u->role == "FACULTY") facultyMenu(*u);
        else if (u->role == "ADMIN")   adminMenu(*u);
        else    cout << "  [!] Unknown role. Contact administrator.\n";
    }
}

// ─────────────────────────────────────────────
//  STUDENT MENU
// ─────────────────────────────────────────────
void Portal::studentMenu(User& u) {
    int choice;
    do {
        cls(); sep();
        cout << "  STUDENT PORTAL  [ " << u.uname << " ]\n"; sep();
        cout << "  1. View My Details\n";
        cout << "  2. View My Seat Allotment\n";
        cout << "  3. View My Exam Schedule\n";
        cout << "  4. View My UFM Records\n";
        cout << "  5. Logout\n"; sep();
        cout << "  Choice: ";
        if (!(cin >> choice)) break;
        cout << "\n";

        string sid = u.refId.empty() ? u.uname : u.refId;
        auto it = stuMap.find(sid);
        if (it == stuMap.end()) it = stuMap.find(u.uname);

        switch (choice) {

        // ── 1. My Details ────────────────────────────────────────────────
        case 1: {
            if (it == stuMap.end()) cout << "  [!] Student record not found.\n";
            else it->second.displayDetails();
            break;
        }

        // ── 2. Seat Allotment ────────────────────────────────────────────
        case 2: {
            sep();
            cout << "          EXAMSYNC SEAT ALLOTMENT          \n"; sep();

            if (it != stuMap.end()) {
                Student& s = it->second;
                cout << "  Roll No     : " << s.roll   << "\n";
                cout << "  Name        : " << s.name   << "\n";
                cout << "  Course      : " << s.course << "\n";
                cout << "  Branch      : " << s.branch << "\n";

                // FIX: seatMap now persists across restarts — just look up directly
                auto sit = seatMap.find(sid);
                if (sit == seatMap.end()) sit = seatMap.find(u.uname);

                if (sit != seatMap.end()) {
                    const Allotment& alt = sit->second;
                    cout << "  Building    : " << alt.building << "\n";
                    cout << "  Floor       : " << alt.floor    << "\n";
                    cout << "  Room        : " << alt.roomName << "\n";
                    cout << "  Seat Number : " << alt.seatNo << "\n";

                    // Timing is FIXED by semester — no lookup needed
                    // Sem 1 & 7 = Morning, Sem 3 & 5 = Afternoon
                    string timing = (s.sem == 1 || s.sem == 7)
                        ? "09:30 AM - 12:30 PM (Morning)"
                        : "01:30 PM - 04:30 PM (Afternoon)";
                    cout << "  Timing      : " << timing << "\n";
                } else {
                    cout << "  Seat Status : Not yet allocated.\n";
                    cout << "  Notice      : Ask Admin to run Seating Allocation.\n";
                }
            } else {
                cout << "  [!] Student record not found.\n";
            }
            sep();
            break;
        }

        // ── 3. Exam Schedule ─────────────────────────────────────────────
        case 3: {
            if (it == stuMap.end()) { cout << "  [!] Student record not found.\n"; break; }
            Student& s = it->second;
            sep();
            cout << "  MY EXAM SCHEDULE — " << s.name << " (Sem " << s.sem << ")\n"; sep();

            string prefix = getCoursePrefix(s.course, s.branch);
            string codeP  = prefix + to_string(s.sem);

            struct ExamItem { string code, date, slot; };
            vector<ExamItem> myExams;
            unordered_set<string> seenCode, seenDate;

            // Helper: check if exam branch (long-form) contains student branch keyword
            // e.g. exam="B.Sc Forensic Science", student.branch="Forensic Science" → match
            // Also matches: exam="B.A. (Hons.) Economics", student.branch="Economics" → match
            auto branchMatch = [&](const string& examBranch) -> bool {
                if (examBranch == s.branch) return true;
                // Check student branch is a suffix/keyword of exam branch
                if (!s.branch.empty() && examBranch.find(s.branch) != string::npos) return true;
                // Check course is a prefix keyword of exam branch
                if (!s.course.empty() && examBranch.find(s.course) != string::npos) return true;
                return false;
            };

            // Priority 1: branch keyword match (handles long-form exam schedule branches)
            for (auto& e : exams) {
                if (e.sem != s.sem) continue;
                if (branchMatch(e.branch) && !seenCode.count(e.subCode) && !seenDate.count(e.date)) {
                    seenCode.insert(e.subCode); seenDate.insert(e.date);
                    myExams.push_back({e.subCode, e.date, e.slot});
                }
                if (myExams.size() >= 8) break;
            }
            // Priority 2: subject code prefix match
            if (myExams.size() < 5) {
                for (auto& e : exams) {
                    if (e.sem != s.sem) continue;
                    if (!codeP.empty() && e.subCode.rfind(codeP, 0) == 0 &&
                        !seenCode.count(e.subCode) && !seenDate.count(e.date)) {
                        seenCode.insert(e.subCode); seenDate.insert(e.date);
                        myExams.push_back({e.subCode, e.date, e.slot});
                    }
                    if (myExams.size() >= 8) break;
                }
            }
            // Priority 3: same semester, any exam (fallback — ensure student always sees something)
            if (myExams.size() < 5) {
                for (auto& e : exams) {
                    if (e.sem != s.sem) continue;
                    if (!seenCode.count(e.subCode) && !seenDate.count(e.date)) {
                        seenCode.insert(e.subCode); seenDate.insert(e.date);
                        myExams.push_back({e.subCode, e.date, e.slot});
                    }
                    if (myExams.size() >= 8) break;
                }
            }

            // Sort chronologically
            twoWayMergeSort(myExams, [](const ExamItem& a, const ExamItem& b) {
                return dateKey(a.date) < dateKey(b.date);
            });

            auto sit = seatMap.find(sid);
            if (sit == seatMap.end()) sit = seatMap.find(u.uname);

            if (myExams.empty()) {
                cout << "  No exam schedule found for your branch/semester.\n";
            } else {
                cout << "  #   Code            Date         Time Slot                        Room & Seat\n";
                cout << "  -------------------------------------------------------------------------------\n";
                int idx = 1;
                for (auto& item : myExams) {
                    cout << "  " << left << setw(4) << idx++
                         << setw(16) << item.code
                         << setw(13) << item.date
                         << setw(33) << item.slot;
                    if (sit != seatMap.end())
                        cout << sit->second.roomName << " — Seat " << sit->second.seatNo;
                    else
                        cout << "Pending admin allocation";
                    cout << "\n";
                }
                cout << "  -------------------------------------------------------------------------------\n";
                cout << "  Total: " << myExams.size() << " papers\n";
            }
            sep();
            break;
        }

        // ── 4. UFM Records ───────────────────────────────────────────────
        case 4: {
            auto recs = ufmLog.getByStudent(sid);
            if (recs.empty()) recs = ufmLog.getByStudent(u.uname);
            sep();
            cout << "  UFM Records for: " << sid << "\n"; sep();
            if (recs.empty()) cout << "  No UFM records. Clean record!\n";
            else for (auto& r : recs) ufmLog.printRecord(r);
            sep();
            break;
        }

        case 5: cout << "  Logged out.\n"; break;
        default: cout << "  [!] Invalid option.\n";
        }

        if (choice != 5) {
            cout << "\n  Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
        }
    } while (choice != 5);
}

// ─────────────────────────────────────────────
//  FACULTY MENU
// ─────────────────────────────────────────────
void Portal::facultyMenu(User& u) {
    int choice;
    do {
        cls(); sep();
        cout << "  FACULTY PORTAL  [ " << u.uname << " ]\n"; sep();
        cout << "  1. View My Details\n";
        cout << "  2. View My Duty Schedule\n";
        cout << "  3. File a UFM Report\n";
        cout << "  4. View UFM Reports I Filed\n";
        cout << "  5. Logout\n"; sep();
        cout << "  Choice: ";
        if (!(cin >> choice)) break;
        cout << "\n";

        string fid = u.refId.empty() ? u.uname : u.refId;
        auto fit = facMap.find(fid);
        if (fit == facMap.end()) fit = facMap.find(u.uname);

        switch (choice) {

        case 1: {
            if (fit == facMap.end()) cout << "  [!] Faculty record not found.\n";
            else fit->second.displayDetails();
            break;
        }

        case 2: {
            sep();
            cout << "  MY INVIGILATION DUTY SCHEDULE\n"; sep();
            if (fit != facMap.end()) {
                Faculty& f = fit->second;
                cout << "  ID   : " << f.fid  << "\n";
                cout << "  Name : " << f.getName() << "\n";
                cout << "  Dept : " << f.dept << "\n"; sep();
            }
            if (dutyMap.empty()) {
                cout << "  Duties not yet scheduled. Contact Admin.\n";
                sep(); break;
            }
            if (!activeDutyAllocatedLabel.empty())
                cout << "  Cycle: " << activeDutyAllocatedLabel << "\n";

            auto dit = dutyMap.find(fid);
            if (dit == dutyMap.end()) dit = dutyMap.find(u.uname);
            if (dit == dutyMap.end() || dit->second.empty()) {
                cout << "  No duties assigned to you.\n";
            } else {
                const auto& duties = dit->second;
                cout << "  Total: " << duties.size() << " assigned\n"; sep();
                cout << "  #   Date         Role                Time Slot                  Building         Room\n";
                cout << "  --------------------------------------------------------------------------------------\n";
                int idx = 1;
                for (const auto& d : duties) {
                    cout << "  " << left << setw(4) << idx++
                         << setw(13) << d.date
                         << setw(20) << d.role
                         << setw(27) << d.time
                         << setw(17) << d.building
                         << d.roomName << "\n";
                }
                cout << "  --------------------------------------------------------------------------------------\n";
            }
            sep();
            break;
        }

        case 3: {
            string roll, subCode, room, itype, details;
            sep(); cout << "  --- File UFM Report ---\n";
            cout << "  Student Roll  : "; cin >> roll;
            cout << "  Subject Code  : "; cin >> subCode;
            cout << "  Room          : "; cin >> room;
            cout << "  Incident Type : "; cin.ignore(); getline(cin, itype);
            cout << "  Details       : "; getline(cin, details);
            ufmLog.addReport(roll, subCode, room, u.refId, itype, details);
            ufmLog.saveToFile("data/ufm_records.csv");
            cout << "  [OK] Report filed successfully.\n"; sep();
            break;
        }

        case 4: {
            auto recs = ufmLog.getByFaculty(u.refId);
            sep(); cout << "  UFM Reports Filed by: " << u.refId << "\n"; sep();
            if (recs.empty()) cout << "  No reports filed by you.\n";
            else for (auto& r : recs) ufmLog.printRecord(r);
            sep();
            break;
        }

        case 5: cout << "  Logged out.\n"; break;
        default: cout << "  [!] Invalid option.\n";
        }

        if (choice != 5) {
            cout << "\n  Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
        }
    } while (choice != 5);
}

// ─────────────────────────────────────────────
//  ADMIN MENU
// ─────────────────────────────────────────────
void Portal::adminMenu(User& u) {
    int choice;
    do {
        cls(); sep();
        cout << "  ADMIN PORTAL  [ " << u.uname << " ]\n";
        cout << "  ODD Semester Examination — Nov 2026 to Jan 2027\n"; sep();
        cout << "  1. Run Seating Allocation  (All students, one shot)\n";
        cout << "  2. Run Duty Scheduling     (Sem 1, 3, 5, 7)\n";
        cout << "  3. View All UFM Reports\n";
        cout << "  4. Update UFM Status\n";
        cout << "  5. View Pending UFM Reports\n";
        cout << "  6. Search Student by Roll\n";
        cout << "  7. Search Faculty by ID\n";
        cout << "  8. Logout\n"; sep();
        cout << "  Choice: ";
        if (!(cin >> choice)) break;
        cout << "\n";

        switch (choice) {

        // ── 1. Seating Allocation — ALL students, one shot ───────────────
        case 1: {
            sep();
            cout << "  SEATING ALLOCATION — ODD SEMESTER (Sem 1, 3, 5, 7)\n";
            cout << "  Each student gets ONE fixed seat valid for all their exam dates.\n";
            sep();

            // Check if already allocated
            if (!seatMap.empty()) {
                cout << "  Existing plan: " << seatMap.size() << " students allocated.\n";
                cout << "  [1] Re-run allocation (clears and rebuilds)\n";
                cout << "  [0] Cancel\n";
                cout << "\n  Choice: ";
                int ch = 0;
                if (!(cin >> ch) || ch != 1) { cin.clear(); cin.ignore(1000, '\n'); break; }
            }

            // Always allocate ALL students at once
            seatMap.clear();
            ofstream clr("data/seating_plan.csv");
            if (clr.is_open())
                clr << "Roll_No,Student_Name,Course,Branch,Semester,Paper_Code,"
                       "Building,Floor,Room,Seat_No,Row,Col,Session,Exam_Date\n";

            string allocLabel = "ALL STUDENTS — ODD Semester (Sem 1, 3, 5, 7)";
            vector<Student> targetStudents;
            for (const auto& kv : stuMap) targetStudents.push_back(kv.second);

            // ── Split by fixed shift: Sem 1 & 7 = Morning, Sem 3 & 5 = Afternoon ──────
            // Same physical seats reused by both shifts (different time = no clash).
            vector<Student> morningStus, afternoonStus;
            for (const auto& s : targetStudents) {
                if (s.sem == 1 || s.sem == 7) morningStus.push_back(s);
                else                           afternoonStus.push_back(s);
            }

            int totalCap = 0;
            for (const auto& r : rooms) totalCap += r.cap;
            sep();
            cout << "  Allocating for      : " << allocLabel << "\n";
            cout << "  Total students      : " << targetStudents.size() << "\n";
            cout << "  Morning (Sem 1 & 7) : " << morningStus.size() << " students\n";
            cout << "  Afternoon (Sem 3&5) : " << afternoonStus.size() << " students\n";
            cout << "  Room capacity/shift : " << totalCap << " seats\n";
            sep();

            // Morning: Sem 1 & 7 — fresh allocation, no preAssigned (full reset)
            cout << "  Running morning allocation (Sem 1 & 7)...\n";
            unordered_map<string, Allotment> emptyPre;
            auto morningSeats = allocateSeats(morningStus, rooms, emptyPre, false);
            for (auto& kv : morningSeats) {
                kv.second.session = "09:30 AM - 12:30 PM (Morning)";
                seatMap[kv.first] = kv.second;
            }
            cout << "  Morning done: " << morningSeats.size()
                 << " / " << morningStus.size() << " seated\n";

            // Afternoon: Sem 3 & 5 — rooms start FRESH (different time slot)
            cout << "  Running afternoon allocation (Sem 3 & 5)...\n";
            auto afternoonSeats = allocateSeats(afternoonStus, rooms, emptyPre, false);
            for (auto& kv : afternoonSeats) {
                kv.second.session = "01:30 PM - 04:30 PM (Afternoon)";
                seatMap[kv.first] = kv.second;
            }
            cout << "  Afternoon done: " << afternoonSeats.size()
                 << " / " << afternoonStus.size() << " seated\n";

            activeAllocatedLabel = allocLabel;
            sep();
            cout << "  ALLOCATION COMPLETE\n";
            cout << "  Morning seated  : " << morningSeats.size()
                 << " / " << morningStus.size() << "\n";
            cout << "  Afternoon seated: " << afternoonSeats.size()
                 << " / " << afternoonStus.size() << "\n";
            cout << "  Total seated    : " << (morningSeats.size() + afternoonSeats.size())
                 << " / " << targetStudents.size() << " students\n";

            // Save full seating plan to CSV
            ofstream f("data/seating_plan.csv");
            if (f.is_open()) {
                f << "Roll_No,Student_Name,Course,Branch,Semester,Paper_Code,"
                     "Building,Floor,Room,Seat_No,Row,Col,Session,Exam_Date\n";
                for (const auto& kv2 : stuMap) {
                    const Student& s = kv2.second;
                    auto sit = seatMap.find(s.roll);
                    if (sit == seatMap.end()) continue;
                    const auto& a = sit->second;
                    f << s.roll << ","
                      << "\"" << s.name   << "\","
                      << "\"" << s.course << "\","
                      << "\"" << s.branch << "\","
                      << s.sem << ","
                      << "\"" << a.code     << "\","
                      << "\"" << a.building << "\","
                      << "\"" << a.floor    << "\","
                      << "\"" << a.roomName << "\","
                      << a.seatNo << "," << a.row << "," << a.col << ","
                      << "\"" << a.session  << "\","
                      << "\"" << a.examDate << "\"\n";
                }
                f.close();
                cout << "  Saved  : data/seating_plan.csv (" << seatMap.size() << " total records)\n";
            }
            sep();
            break;
        }

        // ── 2. Duty Scheduling ───────────────────────────────────────────
        case 2: {
            sep();
            cout << "  DUTY SCHEDULING — ODD SEMESTER (Sem 1, 3, 5, 7)\n";
            cout << "  Exam records : " << exams.size() << "\n"; sep();

            dutyMap.clear();
            activeDutyAllocatedLabel = "ODD Semester (Sem 1, 3, 5, 7)";
            for (auto& kv : facMap) kv.second.duties = 0;

            DutyScheduler ds(facMap);
            ds.assign(exams, rooms, 2, 3);  // 2 invigilators/room, 3 flying squad/building
            dutyMap = ds.facultyDuties;

            ds.printSchedule();
            ds.printFlyingSquadRoster();
            ds.saveToFile("data/duty_schedule.csv");
            cout << "  Saved  : data/duty_schedule.csv\n"; sep();
            break;
        }

        // ── 3. All UFM Reports ───────────────────────────────────────────
        case 3: {
            sep();
            cout << "  ALL UFM REPORTS (" << ufmLog.records.size() << " total)\n"; sep();
            if (ufmLog.records.empty()) cout << "  No records yet.\n";
            else for (auto& r : ufmLog.records) ufmLog.printRecord(r);
            break;
        }

        // ── 4. Update UFM Status ─────────────────────────────────────────
        case 4: {
            string uid, newStatus;
            cout << "  UFM Record ID : "; cin >> uid;
            cout << "  New Status    : "; cin >> newStatus;
            ufmLog.updateStatus(uid, newStatus);
            ufmLog.saveToFile("data/ufm_records.csv");
            cout << "  [OK] Status updated.\n";
            break;
        }

        // ── 5. Pending UFM ───────────────────────────────────────────────
        case 5: {
            auto pending = ufmLog.getPending();
            sep();
            cout << "  PENDING UFM REPORTS (" << pending.size() << ")\n"; sep();
            if (pending.empty()) cout << "  No pending reports.\n";
            else for (auto& r : pending) ufmLog.printRecord(r);
            break;
        }

        // ── 6. Search Student ────────────────────────────────────────────
        case 6: {
            string roll;
            cout << "  Roll Number : "; cin >> roll;

            auto it    = stuMap.find(roll);
            int  steps = 0;
            int  bsIdx = binarySearch(sortedStudents, roll, steps);

            sep();
            cout << "  Hash Table  : " << (it != stuMap.end()
                    ? "FOUND — O(1)" : "Not found") << "\n";
            cout << "  Binary Srch : " << (bsIdx != -1
                    ? "FOUND in " + to_string(steps) + " steps — O(log n)"
                    : "Not found") << "\n";
            if (it != stuMap.end()) it->second.displayDetails();
            else cout << "  [!] No student with roll: " << roll << "\n";
            break;
        }

        // ── 7. Search Faculty ────────────────────────────────────────────
        case 7: {
            string fid;
            cout << "  Faculty ID : "; cin >> fid;
            auto it = facMap.find(fid);
            if (it == facMap.end()) cout << "  [!] No faculty with ID: " << fid << "\n";
            else it->second.displayDetails();
            break;
        }

        case 8: cout << "  Logged out.\n"; break;
        default: cout << "  [!] Invalid option.\n";
        }

        if (choice != 8) {
            cout << "\n  Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
        }
    } while (choice != 8);
}
