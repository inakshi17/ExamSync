#include "portal.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <unordered_set>
using namespace std;

// -------------------------------------------------------
// getCoursePrefix: returns the subject code prefix for a course/branch
// e.g. MCA -> "MCA", B.Tech CSE -> "TCS", BA Economics -> "BEC"
// -------------------------------------------------------
static string getCoursePrefix(const string& course, const string& branch) {
    if (course == "MCA") return "MCA";
    if (course == "BCA") return "BCA";
    if (course == "MBA") return "MBA";
    if (course == "BBA") return "BBA";
    if (course == "B.Com" || course == "BCom") return "BCM";
    if (course == "M.Com" || course == "MCom") return "MCM";
    if (course == "BHM") return "BHM";
    if (course == "MHM") return "MHM";
    if (course == "CCHO") return "CHO";
    if (course.find("LLB") != string::npos || branch == "Law") return "LAW";
    if (course == "LLM") return "LLM";

    // B.Tech branches
    if (course.find("B.Tech") != string::npos || course.find("BTech") != string::npos) {
        if (branch == "CSE" || branch.find("Computer") != string::npos || branch.find("AI") != string::npos) return "TCS";
        if (branch == "ME" || branch.find("Mechanical") != string::npos) return "TME";
        if (branch == "CE" || branch.find("Civil") != string::npos) return "TCV";
        if (branch == "ECE" || branch.find("Electronics") != string::npos) return "TEC";
        if (branch == "EE" || branch.find("Electrical") != string::npos) return "TEE";
        if (branch == "BT" || branch.find("Bio") != string::npos) return "TBT";
        if (branch == "AERO" || branch.find("Aero") != string::npos) return "TAE";
        if (branch == "PE" || branch.find("Petroleum") != string::npos) return "TPE";
        return "TCS";
    }

    // M.Tech branches
    if (course.find("M.Tech") != string::npos || course.find("MTech") != string::npos) {
        if (branch.find("CSE") != string::npos) return "MCS";
        if (branch.find("BT") != string::npos || branch.find("Bio") != string::npos) return "MTB";
        return "MTS";
    }

    // BA branches
    if (course == "BA") {
        if (branch == "Economics") return "BEC";
        if (branch == "English") return "BEN";
        if (branch == "Political Science") return "BPS";
        if (branch == "Psychology") return "BPY";
        if (branch == "Sociology") return "BSO";
        return "BAR";
    }

    // MA branches
    if (course == "MA") {
        if (branch == "Economics") return "MAE";
        if (branch == "English") return "MEN";
        if (branch == "Political Science") return "MAP";
        if (branch == "Sociology") return "MAS";
        if (branch.find("Psychology") != string::npos) return "MAP";
        return "MAS";
    }

    // B.Sc branches
    if (course.find("B.Sc") != string::npos || course.find("BSc") != string::npos) {
        if (branch.find("Food") != string::npos) return "BFT";
        if (branch.find("Nutrition") != string::npos) return "BND";
        if (branch.find("Microbiology") != string::npos) return "BMB";
        if (branch.find("Biotechnology") != string::npos) return "BBT";
        if (branch.find("Forensic") != string::npos) return "BFS";
        if (branch.find("IT") != string::npos) return "BIT";
        if (branch.find("Computer") != string::npos) return "BCS";
        if (branch.find("Geology") != string::npos) return "BGL";
        if (branch.find("Psychology") != string::npos) return "BCP";
        if (branch.find("Aerospace") != string::npos) return "TAE";
        if (branch.find("Hospitality") != string::npos) return "BHM";
        return "BCS";
    }

    // M.Sc branches
    if (course.find("M.Sc") != string::npos || course.find("MSc") != string::npos) {
        if (branch.find("Food") != string::npos || branch.find("Nutrition") != string::npos) return "MFN";
        if (branch.find("Microbiology") != string::npos) return "MMB";
        if (branch.find("Biotechnology") != string::npos) return "MBT";
        if (branch.find("Forensic") != string::npos) return "MFS";
        if (branch.find("IT") != string::npos) return "MIT";
        if (branch.find("Chemistry") != string::npos) return "MCH";
        if (branch.find("Bioinformatics") != string::npos) return "MBI";
        if (branch.find("Geology") != string::npos) return "MGL";
        if (branch.find("Environmental") != string::npos) return "MEV";
        return "MIT";
    }

    return "";
}

// -------------------------------------------------------
// OOP: Polymorphic Profile Display Helper
// Accepts any Person reference (Student or Faculty) and
// executes dynamic dispatch to call the overridden displayDetails().
// -------------------------------------------------------
static void showProfile(const Person& p) {
    p.displayDetails();
}

// -------------------------------------------------------
// sep() - prints a visual separator line
// -------------------------------------------------------
void Portal::sep() {
    cout << "============================================\n";
}

// -------------------------------------------------------
// cls() - clears the terminal screen
// Works on both Windows and Linux/Mac
// -------------------------------------------------------
void Portal::cls() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// -------------------------------------------------------
// loadAll() - loads every CSV data file into memory
// dataPath should end with '/' e.g. "data/"
// -------------------------------------------------------
void Portal::loadAll(const string& dataPath) {
    cout << "[Portal] Loading data from: " << dataPath << "\n";

    // Load user authentication data
    auth.load(dataPath + "Authentication.csv");

    // Load student, faculty, room, subject, exam data using loader functions
    // Each loader fills the container passed by reference
    loadStudents(dataPath + "students.csv",      stuMap);
    loadFaculty (dataPath + "faculty.csv",       facMap);
    loadRooms   (dataPath + "rooms.csv",         rooms);
    loadSubjects(dataPath + "subject_codes.csv", subjects);
    loadExams   (dataPath + "exam_schedule.csv", exams);

    // Load UFM incident records
    ufmLog.load(dataPath + "ufm_records.csv");

    cout << "[Portal] All data loaded successfully.\n\n";
}

// -------------------------------------------------------
// run() - main loop: login then route to role menu
// -------------------------------------------------------
void Portal::run() {
    string uname, pwd;

    while (true) {
        cls();
        printBanner();

        // Ask for credentials
        cout << "  Username: ";
        cin >> uname;
        cout << "  Password: ";
        cin >> pwd;
        cout << "\n";

        // Try to authenticate
        User* u = auth.login(uname, pwd);
        if (!u) {
            cout << "  [!] Invalid credentials. Please try again.\n";
            cout << "  Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            continue;
        }

        cout << "  Welcome, " << u->uname << "! Role: " << u->role << "\n\n";

        // Route to the right portal based on role
        if      (u->role == "STUDENT") studentMenu(*u);
        else if (u->role == "FACULTY") facultyMenu(*u);
        else if (u->role == "ADMIN")   adminMenu(*u);
        else {
            cout << "  [!] Unknown role. Contact administrator.\n";
        }
    }
}

// -------------------------------------------------------
// studentMenu() - portal for logged-in STUDENT users
// Options: details, exam schedule, UFM records, logout
// -------------------------------------------------------
void Portal::studentMenu(User& u) {
    int choice;
    do {
        cls();
        sep();
        cout << "  STUDENT PORTAL  [ " << u.uname << " ]\n";
        sep();
        cout << "  1. View My Details\n";
        cout << "  2. View My Seat Allotment / Admit Card\n";
        cout << "  3. View My Exam Schedule\n";
        cout << "  4. View My UFM Records\n";
        cout << "  5. Logout\n";
        sep();
        cout << "  Choice: ";
        cin >> choice;
        cout << "\n";

        // Helper to find student by either refId or username
        string sid = u.refId.empty() ? u.uname : u.refId;
        auto it = stuMap.find(sid);
        if (it == stuMap.end() && sid != u.uname) {
            it = stuMap.find(u.uname);
        }

        switch (choice) {

        // ---------- 1. Show student profile (OOP Polymorphism) ----------
        case 1: {
            if (it == stuMap.end()) {
                cout << "  [!] Student record not found for ID: " << sid << "\n";
            } else {
                showProfile(it->second); // Dynamic binding via Person reference
            }
            break;
        }

        // ---------- 2. Show seat allotment / admit card ----------
        case 2: {
            sep();
            cout << "       EXAMSYNC ADMIT CARD & SEAT ALLOTMENT       \n";
            sep();
            if (it != stuMap.end()) {
                Student& s = it->second;
                cout << "  Roll No      : " << s.roll   << "\n";
                cout << "  Student Name : " << s.name   << "\n";
                cout << "  Course       : " << s.course << "\n";
                cout << "  Branch       : " << s.branch << " | Sem: " << s.sem << "\n";
            }

            // Auto-run seating quietly if not run by admin yet
            if (seatMap.empty()) {
                vector<Student> allStus;
                allStus.reserve(stuMap.size());
                for (auto& kv : stuMap) allStus.push_back(kv.second);
                seatMap = allocateSeats(allStus, rooms, false);
            }

            auto sit = seatMap.find(sid);
            if (sit == seatMap.end() && sid != u.uname) {
                sit = seatMap.find(u.uname);
            }

            if (sit != seatMap.end()) {
                const Allotment& alt = sit->second;
                cout << "  Building     : " << alt.building << "\n";
                cout << "  Floor Name   : " << alt.floor << "\n";
                cout << "  Class / Room : " << alt.roomName << "\n";
                cout << "  Seat Number  : Seat " << alt.seatNo << " (Row " << alt.row << ", Col " << alt.col << ")\n";
            } else {
                cout << "  Seat Status  : Not Allocated / Contact Examination Cell\n";
            }
            sep();
            break;
        }

        // ---------- 3. Show exam schedule for this student ----------
        case 3: {
            if (it == stuMap.end()) {
                cout << "  [!] Student record not found.\n";
                break;
            }
            Student& s = it->second;
            sep();
            cout << "              MY ASSIGNED EXAM SCHEDULE\n";
            sep();
            cout << "  Student Name : " << s.name << "\n";
            cout << "  Roll Number  : " << s.roll << "\n";
            cout << "  Course/Branch: " << s.course << " (" << s.branch << ")\n";
            cout << "  Semester     : " << s.sem << "\n";
            sep();

            string prefix = getCoursePrefix(s.course, s.branch);
            string semStr = to_string(s.sem);
            string codePrefix = prefix + semStr; // e.g. "MCA4", "TCS3", "BEC2"

            struct ExamItem {
                string subCode;
                string date;
                string slot;
            };
            vector<ExamItem> assignedExams;
            unordered_set<string> seen;

            // Step 1: Match by semester and subject code prefix (e.g. MCA401, MCA402...)
            for (auto& e : exams) {
                if (e.sem != s.sem) continue;
                if (!codePrefix.empty() && e.subCode.rfind(codePrefix, 0) == 0) {
                    if (seen.find(e.subCode) == seen.end()) {
                        seen.insert(e.subCode);
                        assignedExams.push_back({e.subCode, e.date, e.slot});
                    }
                }
                if (assignedExams.size() >= 5) break; // Standard 5 assigned courses per semester
            }

            // Step 2: Fallback to branch title in exam_schedule if prefix was empty
            if (assignedExams.empty()) {
                for (auto& e : exams) {
                    if (e.sem != s.sem) continue;
                    bool branchMatch = (e.branch == s.branch || e.branch == s.course ||
                                        e.branch.find(s.course) != string::npos);
                    if (branchMatch) {
                        if (seen.find(e.subCode) == seen.end()) {
                            seen.insert(e.subCode);
                            assignedExams.push_back({e.subCode, e.date, e.slot});
                        }
                    }
                    if (assignedExams.size() >= 5) break;
                }
            }

            // Display only the assigned courses in an aligned table
            if (assignedExams.empty()) {
                cout << "  No exams currently scheduled for your semester & course.\n";
            } else {
                cout << "  #   Subject Code   Exam Date\n";
                cout << "  -------------------------------------\n";
                int idx = 1;
                for (auto& item : assignedExams) {
                    cout << "  " << left << setw(4) << idx++
                         << setw(15) << item.subCode
                         << item.date << "\n";
                }
                cout << "  -------------------------------------\n";
                cout << "  Total Assigned Exams: " << assignedExams.size() << " courses\n";
            }
            sep();
            break;
        }

        // ---------- 4. Show UFM records for this student ----------
        case 4: {
            auto recs = ufmLog.getByStudent(sid);
            if (recs.empty() && sid != u.uname) {
                recs = ufmLog.getByStudent(u.uname);
            }
            sep();
            cout << "  UFM Records for Roll: " << sid << "\n";
            sep();
            if (recs.empty()) {
                cout << "  No UFM records found. Clear record!\n";
            } else {
                for (auto& r : recs) ufmLog.printRecord(r);
            }
            sep();
            break;
        }

        case 5:
            cout << "  Logged out.\n";
            break;

        default:
            cout << "  [!] Invalid option.\n";
        }

        // Pause so user can read output before menu redraws
        if (choice != 5) {
            cout << "\n  Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }

    } while (choice != 5);
}

// -------------------------------------------------------
// facultyMenu() - portal for logged-in FACULTY users
// Options: details, duty schedule, file UFM, view UFM, logout
// -------------------------------------------------------
void Portal::facultyMenu(User& u) {
    int choice;
    do {
        cls();
        sep();
        cout << "  FACULTY PORTAL  [ " << u.uname << " ]\n";
        sep();
        cout << "  1. View My Details\n";
        cout << "  2. View My Duty Schedule\n";
        cout << "  3. File a UFM Report\n";
        cout << "  4. View UFM Reports I Filed\n";
        cout << "  5. Logout\n";
        sep();
        cout << "  Choice: ";
        cin >> choice;
        cout << "\n";

        switch (choice) {

        // ---------- 1. Show faculty profile (OOP Polymorphism) ----------
        case 1: {
            string fid = u.refId.empty() ? u.uname : u.refId;
            auto it = facMap.find(fid);
            if (it == facMap.end() && fid != u.uname) {
                it = facMap.find(u.uname);
            }
            if (it == facMap.end()) {
                cout << "  [!] Faculty record not found for ID: " << fid << "\n";
            } else {
                showProfile(it->second); // Dynamic binding via Person reference
            }
            break;
        }

        // ---------- 2. Show duty schedule for this faculty ----------
        case 2: {
            sep();
            cout << "  Duty Schedule for Faculty ID: " << u.refId << "\n";
            sep();
            // DutyScheduler is run by Admin; here we just show duties
            // stored in the faculty record's 'duties' field
            auto it = facMap.find(u.refId);
            if (it == facMap.end()) {
                cout << "  [!] Faculty not found.\n";
            } else {
                cout << "  Assigned Duties: " << it->second.duties << "\n";
            }
            sep();
            break;
        }

        // ---------- 3. File a new UFM report ----------
        case 3: {
            string roll, subCode, room, itype, details;
            sep();
            cout << "  --- File UFM Report ---\n";
            cout << "  Student Roll  : "; cin >> roll;
            cout << "  Subject Code  : "; cin >> subCode;
            cout << "  Room          : "; cin >> room;
            cout << "  Incident Type : "; cin.ignore(); getline(cin, itype);
            cout << "  Details       : "; getline(cin, details);

            // File the report; faculty ID comes from the logged-in user
            ufmLog.addReport(roll, subCode, room, u.refId, itype, details);

            // Auto-save after filing
            ufmLog.saveToFile("data/ufm_records.csv");
            sep();
            break;
        }

        // ---------- 4. View all UFM reports filed by this faculty ----------
        case 4: {
            auto recs = ufmLog.getByFaculty(u.refId);
            sep();
            cout << "  UFM Reports Filed by: " << u.refId << "\n";
            sep();
            if (recs.empty()) {
                cout << "  No UFM reports filed by you.\n";
            } else {
                for (auto& r : recs) ufmLog.printRecord(r);
            }
            sep();
            break;
        }

        case 5:
            cout << "  Logged out.\n";
            break;

        default:
            cout << "  [!] Invalid option.\n";
        }

        if (choice != 5) {
            cout << "\n  Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }

    } while (choice != 5);
}

// -------------------------------------------------------
// adminMenu() - portal for ADMIN users
// Options: seating, duty, UFM management, student/faculty search
// -------------------------------------------------------
void Portal::adminMenu(User& u) {
    int choice;
    do {
        cls();
        sep();
        cout << "  ADMIN PORTAL  [ " << u.uname << " ]\n";
        sep();
        cout << "  1. Run Seating Allocation\n";
        cout << "  2. Run Duty Scheduling\n";
        cout << "  3. View All UFM Reports\n";
        cout << "  4. Update UFM Status\n";
        cout << "  5. View All Pending UFM\n";
        cout << "  6. Search Student by Roll\n";
        cout << "  7. Search Faculty by ID\n";
        cout << "  8. Logout\n";
        sep();
        cout << "  Choice: ";
        cin >> choice;
        cout << "\n";

        switch (choice) {

        // ---------- 1. Run seating allocation ----------
        case 1: {
            sep();
            cout << "  Running Seating Allocation...\n";
            sep();

            // Collect all students from the map into a vector
            vector<Student> allStudents;
            allStudents.reserve(stuMap.size());
            for (auto& kv : stuMap) allStudents.push_back(kv.second);

            // Call seating engine (defined in seating.h)
            // printSummary = true: displays the complete allocation summary table
            seatMap = allocateSeats(allStudents, rooms, true);

            sep();
            cout << "  Seating allocation complete. Total: "
                 << seatMap.size() << " students seated.\n";
            break;
        }

        // ---------- 2. Run duty scheduling ----------
        case 2: {
            sep();
            cout << "  Running Duty Scheduling...\n";
            sep();

            // DutyScheduler takes facMap directly (by reference)
            DutyScheduler ds(facMap);
            ds.assign(exams);   // assign faculty to all exam slots

            // Print the schedule
            ds.printSchedule();
            sep();
            break;
        }

        // ---------- 3. View ALL UFM records ----------
        case 3: {
            sep();
            cout << "  All UFM Records (" << ufmLog.records.size() << " total):\n";
            sep();
            if (ufmLog.records.empty()) {
                cout << "  No UFM records exist yet.\n";
            } else {
                for (auto& r : ufmLog.records) ufmLog.printRecord(r);
            }
            break;
        }

        // ---------- 4. Update status of a UFM record ----------
        case 4: {
            string uid, newStatus;
            cout << "  UFM Record ID  : "; cin >> uid;
            cout << "  New Status     : "; cin >> newStatus;
            ufmLog.updateStatus(uid, newStatus);
            // Save after update
            ufmLog.saveToFile("data/ufm_records.csv");
            break;
        }

        // ---------- 5. View only PENDING UFM records ----------
        case 5: {
            auto pending = ufmLog.getPending();
            sep();
            cout << "  Pending UFM Reports (" << pending.size() << "):\n";
            sep();
            if (pending.empty()) {
                cout << "  No pending UFM reports.\n";
            } else {
                for (auto& r : pending) ufmLog.printRecord(r);
            }
            break;
        }

        // ---------- 6. O(1) student lookup by roll (OOP Polymorphism) ----------
        case 6: {
            string roll;
            cout << "  Enter Roll Number: "; cin >> roll;
            auto it = stuMap.find(roll);
            if (it == stuMap.end()) {
                cout << "  [!] Student not found: " << roll << "\n";
            } else {
                showProfile(it->second); // Dynamic binding via Person reference
            }
            break;
        }

        // ---------- 7. O(1) faculty lookup by ID (OOP Polymorphism) ----------
        case 7: {
            string fid;
            cout << "  Enter Faculty ID: "; cin >> fid;
            auto it = facMap.find(fid);
            if (it == facMap.end()) {
                cout << "  [!] Faculty not found: " << fid << "\n";
            } else {
                showProfile(it->second); // Dynamic binding via Person reference
            }
            break;
        }

        case 8:
            cout << "  Logged out.\n";
            break;

        default:
            cout << "  [!] Invalid option.\n";
        }

        if (choice != 8) {
            cout << "\n  Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }

    } while (choice != 8);
}
