#pragma once
#include "structs.h"
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

// unfair means reporting and tracking system
class UFMLogger {
public:
    vector<UFM> records;                // list of all ufm incidents
    unordered_map<string, int> idIndex; // index map: uid -> record position in vector

    // load ufm reports from csv
    void load(const string& file);

    // file a new incident report
    void addReport(const string& roll, const string& subCode,
                   const string& room,  const string& facId,
                   const string& itype, const string& details);

    // update report status (e.g. PENDING -> RESOLVED)
    bool updateStatus(const string& uid, const string& newStatus);

    // fetch all reports for a student
    vector<UFM> getByStudent(const string& roll);

    // fetch all reports filed by a teacher
    vector<UFM> getByFaculty(const string& facId);

    // fetch pending cases
    vector<UFM> getPending();

    // print incident details to console
    void printRecord(const UFM& u);

    // save reports to csv file
    void saveToFile(const string& fname);
};
