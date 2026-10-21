#include "ufm.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>     
#include <iomanip>   
using namespace std;

static vector<string> splitCSV(string line) {
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ')) {
        line.pop_back();
    }
    vector<string> fields;
    stringstream ss(line);
    string tok;
    while (getline(ss, tok, ',')) {
        while (!tok.empty() && (tok.front() == ' ' || tok.front() == '\t' || tok.front() == '"')) {
            tok.erase(tok.begin());
        }
        while (!tok.empty() && (tok.back() == ' ' || tok.back() == '\t' || tok.back() == '\r' || tok.back() == '\n' || tok.back() == '"')) {
            tok.pop_back();
        }
        fields.push_back(tok);
    }
    return fields;
}

void UFMLogger::load(const string& file) {
    ifstream fin(file);
    if (!fin.is_open()) {
        cout << "[UFM] No existing UFM file found. Starting fresh.\n";
        return;
    }

    string line;
    getline(fin, line); 

    while (getline(fin, line)) {
        if (line.empty()) continue;
        auto f = splitCSV(line);
        if (f.size() < 9) continue; 

        UFM u;
        u.uid       = f[0];
        u.roll      = f[1];
        u.subCode   = f[2];
        u.room      = f[3];
        u.facId     = f[4];
        u.timestamp = f[5];
        u.itype     = f[6];
        u.details   = f[7];
        u.status    = f[8];

        idIndex[u.uid] = (int)records.size();
        records.push_back(u);
    }

    cout << "[UFM] Loaded " << records.size() << " UFM records.\n";
}

void UFMLogger::addReport(const string& roll,    const string& subCode,
                          const string& room,    const string& facId,
                          const string& itype,   const string& details) {
    UFM u;

    u.uid = "UFM" + to_string(records.size() + 1001);

    u.roll    = roll;
    u.subCode = subCode;
    u.room    = room;
    u.facId   = facId;
    u.itype   = itype;
    u.details = details;
    u.status  = "PENDING"; 

    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    u.timestamp = string(buf);

    idIndex[u.uid] = (int)records.size();
    records.push_back(u);

    cout << "[UFM] Report filed successfully. ID: " << u.uid << "\n";
}

bool UFMLogger::updateStatus(const string& uid, const string& newStatus) {
    if (newStatus != "PENDING" && newStatus != "UNDER_REVIEW" &&
        newStatus != "PENALIZED" && newStatus != "DISMISSED") {
        cout << "[UFM] Invalid status '" << newStatus << "'.\n";
        cout << "      Use: PENDING / UNDER_REVIEW / PENALIZED / DISMISSED\n";
        return false;
    }

    auto it = idIndex.find(uid);
    if (it == idIndex.end()) {
        cout << "[UFM] Record not found: " << uid << "\n";
        return false;
    }

    records[it->second].status = newStatus;
    cout << "[UFM] Status of " << uid << " updated to " << newStatus << ".\n";
    return true;
}

vector<UFM> UFMLogger::getByStudent(const string& roll) {
    vector<UFM> result;
    for (auto& u : records) {
        if (u.roll == roll) result.push_back(u);
    }
    return result;
}

vector<UFM> UFMLogger::getByFaculty(const string& facId) {
    vector<UFM> result;
    for (auto& u : records) {
        if (u.facId == facId) result.push_back(u);
    }
    return result;
}

vector<UFM> UFMLogger::getPending() {
    vector<UFM> result;
    for (auto& u : records) {
        if (u.status == "PENDING") result.push_back(u);
    }
    return result;
}

void UFMLogger::printRecord(const UFM& u) {
    cout << "+------------------------------------------+\n";
    cout << "| UFM ID   : " << left << setw(30) << u.uid      << "|\n";
    cout << "| Student  : " << left << setw(30) << u.roll     << "|\n";
    cout << "| Subject  : " << left << setw(30) << u.subCode  << "|\n";
    cout << "| Room     : " << left << setw(30) << u.room     << "|\n";
    cout << "| Reported : " << left << setw(30) << u.facId    << "|\n";
    cout << "| Type     : " << left << setw(30) << u.itype    << "|\n";
    cout << "| Details  : " << left << setw(30) << u.details  << "|\n";
    cout << "| Time     : " << left << setw(30) << u.timestamp<< "|\n";
    cout << "| Status   : " << left << setw(30) << u.status   << "|\n";
    cout << "+------------------------------------------+\n";
}

void UFMLogger::saveToFile(const string& fname) {
    ofstream fout(fname);
    if (!fout.is_open()) {
        cout << "[UFM] ERROR: Cannot open file for saving: " << fname << "\n";
        return;
    }

    fout << "uid,roll,subCode,room,facId,timestamp,itype,details,status\n";

    for (auto& u : records) {
        fout << u.uid       << ","
             << u.roll      << ","
             << u.subCode   << ","
             << u.room      << ","
             << u.facId     << ","
             << u.timestamp << ","
             << u.itype     << ","
             << u.details   << ","
             << u.status    << "\n";
    }
    cout << "[UFM] Saved " << records.size() << " records to " << fname << ".\n";
}
