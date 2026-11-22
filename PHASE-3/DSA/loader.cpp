#include "loader.h"

#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

// helper: splits a csv line by commas and strips extra whitespace/quotes
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

// load user credentials into hash map
void loadUsers(const string& file, unordered_map<string, User>& umap) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line); 

    int cnt = 0;          
    while (getline(fin, line)) {
        if (line.empty()) continue;   

        auto tok = splitLine(line);
        if (tok.size() < 4) continue; 

        User u;
        u.uname = tok[0];
        u.pwd   = tok[1];
        u.role  = tok[2];
        u.refId = tok[3];

        umap[u.uname] = u;
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}

// load student records into hash map by roll number
void loadStudents(const string& file, unordered_map<string, Student>& smap) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);  

    int cnt = 0;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        auto tok = splitLine(line);
        if (tok.size() < 5) continue;

        Student s;
        s.roll   = tok[0];
        s.name   = tok[1];
        s.course = tok[2];
        s.branch = tok[3];
        s.sem    = stoi(tok[4]); 

        smap[s.roll] = s;
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}

// load faculty members into hash map by faculty id
void loadFaculty(const string& file, unordered_map<string, Faculty>& fmap) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);  

    int cnt = 0;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        auto tok = splitLine(line);
        if (tok.size() < 3) continue;

        Faculty f;
        f.fid    = tok[0];
        f.fname  = tok[1];
        f.dept   = tok[2];
        f.duties = 0;     

        fmap[f.fid] = f;
        cnt++;
    }
    cout << "Loaded " << cnt << " records from " << file << "\n";
}

// load exam rooms and capacities into vector
void loadRooms(const string& file, vector<Room>& rooms) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);  

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

// load exam schedule (date, slot, branch)
void loadExams(const string& file, vector<Exam>& exams) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);  

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
            e.slot = "09:00 AM - 12:00 PM (Morning)";
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

// load ufm incident records
void loadUFM(const string& file, vector<UFM>& ufms) {
    ifstream fin(file);

    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open: " << file << "\n";
        return;
    }

    string line;
    getline(fin, line);  

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
