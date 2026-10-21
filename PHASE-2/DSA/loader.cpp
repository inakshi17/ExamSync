#include "loader.h"

#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

// ─────────────────────────────────────────────
//  loader.cpp  –  Reads all 7 CSV files and
//  fills the data structures used by ExamSync.
//
//  CSV column assumptions (see structs.h):
//   Authentication.csv : Username, Password, Role, Reference_ID
//   students.csv       : Roll_No, Student_Name, Course, Branch, Semester
//   faculty.csv        : Faculty_ID, Faculty_Name, Department
//   rooms.csv          : Room_ID, Room_Name, Building, Floor, Room_Type,
//                        Row_Groups, Seat_Columns, Seats_Per_Column, Capacity
//   subject_codes.csv  : subjectcode, semester, branch
//   exam_schedule.csv  : Exam_ID, Subject_Code, Exam_Date, Slot, Semester,
//                        Course_Branch
//   ufm_records.csv    : UFM_ID, Roll_No, Subject_Code, Room_Name,
//                        Reported_By_Faculty_ID, Timestamp,
//                        Incident_Type, Details, Status
// ─────────────────────────────────────────────


// ── Helper ───────────────────────────────────
// Split one CSV line by commas and return each
// field as a trimmed string inside a vector.
// Handles CRLF (\r\n), leading/trailing spaces, and quotes.
static vector<string> splitLine(string line) {
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ')) {
        line.pop_back();
    }
    vector<string> tok;
    stringstream ss(line);
    string t;
    while (getline(ss, t, ',')) {
        while (!t.empty() && (t.front() == ' ' || t.front() == '\t' || t.front() == '"')) {
            t.erase(t.begin());
        }
        while (!t.empty() && (t.back() == ' ' || t.back() == '\t' || t.back() == '\r' || t.back() == '\n' || t.back() == '"')) {
            t.pop_back();
        }
        tok.push_back(t);
    }
    return tok;
}


// ── loadUsers ────────────────────────────────
// Reads Authentication.csv.
// Columns: Username(0) Password(1) Role(2) Reference_ID(3)
void loadUsers(const string& file, unordered_map<string, User>& umap) {
    ifstream fin(file);

    // Check if file opened successfully
    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);   // skip the header row

    int cnt = 0;          // count records loaded
    while (getline(fin, line)) {
        if (line.empty()) continue;   // skip blank lines

        auto tok = splitLine(line);
        if (tok.size() < 4) continue; // skip malformed rows

        // Fill the User struct
        User u;
        u.uname = tok[0];
        u.pwd   = tok[1];
        u.role  = tok[2];
        u.refId = tok[3];

        // Key = username for O(1) look-up during login
        umap[u.uname] = u;
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}


// ── loadStudents ─────────────────────────────
// Reads students.csv.
// Columns: Roll_No(0) Student_Name(1) Course(2) Branch(3) Semester(4)
void loadStudents(const string& file, unordered_map<string, Student>& smap) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);   // skip header

    int cnt = 0;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        auto tok = splitLine(line);
        if (tok.size() < 5) continue;

        // Fill the Student struct
        Student s;
        s.roll   = tok[0];
        s.name   = tok[1];
        s.course = tok[2];
        s.branch = tok[3];
        s.sem    = stoi(tok[4]); // convert semester string → int

        // Key = roll number
        smap[s.roll] = s;
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}


// ── loadFaculty ──────────────────────────────
// Reads faculty.csv.
// Columns: Faculty_ID(0) Faculty_Name(1) Department(2)
// duties is NOT in the CSV; it starts at 0 for every faculty member.
void loadFaculty(const string& file, unordered_map<string, Faculty>& fmap) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);   // skip header

    int cnt = 0;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        auto tok = splitLine(line);
        if (tok.size() < 3) continue;

        Faculty f;
        f.fid    = tok[0];
        f.fname  = tok[1];
        f.dept   = tok[2];
        f.duties = 0;     // always initialised to zero at load time

        // Key = faculty id
        fmap[f.fid] = f;
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}


// ── loadRooms ────────────────────────────────
// Reads rooms.csv.
// Columns: Room_ID(0) Room_Name(1) Building(2) Floor(3) Room_Type(4)
//          Row_Groups(5) Seat_Columns(6) Seats_Per_Column(7) Capacity(8)
void loadRooms(const string& file, vector<Room>& rooms) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);   // skip header

    int cnt = 0;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        auto tok = splitLine(line);
        if (tok.size() < 9) continue;

        Room r;
        r.rid      = tok[0];
        r.rname    = tok[1];
        r.building = tok[2];
        r.floor    = tok[3];
        r.rtype    = tok[4];
        r.rowGrp   = stoi(tok[5]);
        r.cols     = stoi(tok[6]);
        r.perCol   = stoi(tok[7]);
        r.cap      = stoi(tok[8]);

        rooms.push_back(r);
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}


// ── loadSubjects ─────────────────────────────
// Reads subject_codes.csv.
// Columns: subjectcode(0) semester(1) branch(2)
void loadSubjects(const string& file, vector<Subject>& subs) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);   // skip header

    int cnt = 0;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        auto tok = splitLine(line);
        if (tok.size() < 3) continue;

        Subject s;
        s.code   = tok[0];
        s.sem    = stoi(tok[1]);
        s.branch = tok[2];

        subs.push_back(s);
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}


// ── loadExams ────────────────────────────────
// Reads exam_schedule.csv.
// Columns: Exam_ID(0) Subject_Code(1) Exam_Date(2) Slot(3)
//          Semester(4) Course_Branch(5)
void loadExams(const string& file, vector<Exam>& exams) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);   // skip header

    int cnt = 0;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        auto tok = splitLine(line);
        if (tok.size() < 5) continue;

        Exam e;
        e.eid     = tok[0];
        e.subCode = tok[1];
        e.date    = tok[2];
        if (tok.size() == 5) {
            e.slot = "10:00 AM - 01:00 PM";
            try { e.sem = stoi(tok[3]); } catch (...) { e.sem = 0; }
            e.branch = tok[4];
        } else {
            e.slot = tok[3];
            try { e.sem = stoi(tok[4]); } catch (...) { e.sem = 0; }
            e.branch = tok[5];
        }

        exams.push_back(e);
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}


// ── loadUFM ──────────────────────────────────
// Reads ufm_records.csv.
// Columns: UFM_ID(0) Roll_No(1) Subject_Code(2) Room_Name(3)
//          Reported_By_Faculty_ID(4) Timestamp(5)
//          Incident_Type(6) Details(7) Status(8)
void loadUFM(const string& file, vector<UFM>& ufms) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);   // skip header

    int cnt = 0;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        auto tok = splitLine(line);
        if (tok.size() < 9) continue;

        UFM u;
        u.uid       = tok[0];
        u.roll      = tok[1];
        u.subCode   = tok[2];
        u.room      = tok[3];
        u.facId     = tok[4];
        u.timestamp = tok[5];
        u.itype     = tok[6];
        u.details   = tok[7];
        u.status    = tok[8];

        ufms.push_back(u);
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}
