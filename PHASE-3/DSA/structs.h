#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

// user login credentials
struct User {
    string uname; 
    string pwd;   
    string role;  
    string refId; 
};

// abstract base class for person (OOP concept)
class Person {
protected:
    string id;
    string name;

public:
    Person() = default;
    Person(const string& idVal, const string& nameVal) : id(idVal), name(nameVal) {}
    virtual ~Person() = default;

    virtual string getId() const { return id; }
    virtual string getName() const { return name; }
    virtual void setId(const string& newId) { id = newId; }
    virtual void setName(const string& newName) { name = newName; }

    // pure virtual functions for polymorphism
    virtual void displayDetails() const = 0;
    virtual string getRole() const = 0;
};

// student class inheriting from Person
class Student : public Person {
public:
    string roll;    
    string course;  
    string branch;  
    int    sem;     

    using Person::name;

    Student() : Person("", ""), roll(""), course(""), branch(""), sem(0) {}
    Student(const string& r, const string& n, const string& c, const string& b, int s)
        : Person(r, n), roll(r), course(c), branch(b), sem(s) {}

    string getId() const override { return roll.empty() ? id : roll; }
    string getName() const override { return name; }

    void displayDetails() const override {
        std::cout << "============================================\n";
        std::cout << "  Name   : " << name   << "\n";
        std::cout << "  Roll   : " << roll   << "\n";
        std::cout << "  Course : " << course << "\n";
        std::cout << "  Branch : " << branch << "\n";
        std::cout << "  Sem    : " << sem    << "\n";
        std::cout << "============================================\n";
    }

    string getRole() const override {
        return "STUDENT";
    }
};

// faculty class inheriting from Person
class Faculty : public Person {
public:
    string fid;    
    string fname;  
    string dept;   
    int    duties; 

    using Person::name;

    Faculty() : Person("", ""), fid(""), fname(""), dept(""), duties(0) {}
    Faculty(const string& idVal, const string& nameVal, const string& d, int dt = 0)
        : Person(idVal, nameVal), fid(idVal), fname(nameVal), dept(d), duties(dt) {}

    string getId() const override { return fid.empty() ? id : fid; }
    string getName() const override { return fname.empty() ? name : fname; }

    void displayDetails() const override {
        std::cout << "============================================\n";
        std::cout << "  Name   : " << (fname.empty() ? name : fname) << "\n";
        std::cout << "  Fac ID : " << (fid.empty() ? id : fid)       << "\n";
        std::cout << "  Dept   : " << dept                           << "\n";
        std::cout << "  Duties : " << duties                         << "\n";
        std::cout << "============================================\n";
    }

    string getRole() const override {
        return "FACULTY";
    }
};

// exam room info
struct Room {
    string rid;       
    string rname;     
    string building;  
    string floor;     
    string rtype;     
    int    rowGrp;    
    int    cols;      
    int    perCol;    
    int    cap;       
};

// exam schedule entry
struct Exam {
    string eid;      
    string subCode;  
    string date;     
    string slot;     
    int    sem;      
    string branch;   
};

// ufm disciplinary incident record
struct UFM {
    string uid;       
    string roll;      
    string subCode;   
    string room;      
    string facId;     
    string timestamp; 
    string itype;     
    string details;   
    string status;    
};

// single desk seat in room grid
struct Seat {
    int    r, c;    
    string roll;    
    string branch;  
};

// final student allotment result
struct Allotment {
    string roll;
    string roomName;
    string building;
    string floor;
    int    row     = 0;
    int    col     = 0;
    int    seatNo  = 0;
    string session;
    string code;     // paper code (e.g. TCS3, MCA1)
    string examDate; // the specific exam date this seat is for
};

// faculty invigilation duty assignment
struct Duty {
    string fid;       
    string fname;     
    string date;      
    string time;      
    string roomName;  
    string building;  
    string floor;     
    string role = "Room Invigilator"; // "Room Invigilator" or "Flying Squad"
};

// converts floor string like "1" to "1st Floor"
inline string formatFloorName(const string& flr) {
    if (flr == "Ground" || flr == "0") return "Ground Floor";
    if (flr == "1") return "1st Floor";
    if (flr == "2") return "2nd Floor";
    if (flr == "3") return "3rd Floor";
    if (flr == "4") return "4th Floor";
    if (flr.find("Floor") != string::npos) return flr;
    return flr + " Floor";
}

// maps course and branch to standardized 3-letter academic code prefix
inline string getCoursePrefix(const string& course, const string& branch) {
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

// returns full exam paper code for a student (e.g. TCS3, MCA1, BBA3)
inline string getStudentCode(const Student& s) {
    string pfx = getCoursePrefix(s.course, s.branch);
    if (pfx.empty()) {
        pfx = (s.branch.size() >= 3) ? s.branch.substr(0, 3) : s.branch;
    }
    return pfx + to_string(s.sem);
}
