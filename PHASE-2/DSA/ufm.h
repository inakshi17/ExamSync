#pragma once
#include "structs.h"
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class UFMLogger {
public:
    vector<UFM> records;
    unordered_map<string, int> idIndex;

    void load(const string& file);

    void addReport(const string& roll, const string& subCode,
                   const string& room,  const string& facId,
                   const string& itype, const string& details);

    bool updateStatus(const string& uid, const string& newStatus);

    vector<UFM> getByStudent(const string& roll);

    vector<UFM> getByFaculty(const string& facId);

    vector<UFM> getPending();

    void printRecord(const UFM& u);

    void saveToFile(const string& fname);
};
