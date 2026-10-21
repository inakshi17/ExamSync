#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct User {
    string uname; 
    string pwd;   
    string role;  
    string refId; 
};

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

    virtual void displayDetails() const = 0;
    virtual string getRole() const = 0;
};

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

struct Subject {
    string code;    
    int    sem;     
    string branch;  
};

struct Exam {
    string eid;      
    string subCode;  
    string date;     
    string slot;     
    int    sem;      
    string branch;   
};

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

struct Seat {
    int    r, c;    
    string roll;    
    string branch;  
};

struct Allotment {
    string roll;
    string roomName;
    string building;
    string floor;
    int row;
    int col;
    int seatNo;
    string session;
};

struct Duty {
    string fid;       
    string fname;     
    string date;      
    string time;      
    string roomName;  
    string building;  
    string floor;     
};

inline string formatFloorName(const string& flr) {
    if (flr == "Ground" || flr == "0") return "Ground Floor";
    if (flr == "1") return "1st Floor";
    if (flr == "2") return "2nd Floor";
    if (flr == "3") return "3rd Floor";
    if (flr == "4") return "4th Floor";
    if (flr.find("Floor") != string::npos) return flr;
    return flr + " Floor";
}
