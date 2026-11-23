#pragma once
#include "structs.h"
#include "plan.h"
#include "ufm.h"
#include "auth.h"
#include "algo.h"
#include "loader.h"
#include <iostream>
#include <unordered_map>

// shared data loaded from CSV (hash maps + sorted array); loaded once per command
struct Ctx {
    unordered_map<string, Student> stu;   // hash table: roll -> student
    vector<Student> sorted;               // merge-sorted by roll, used by binary search
    unordered_map<string, Faculty> fac;   // hash table: faculty id -> faculty
    vector<Room> rooms;
    vector<Exam> exams;
    Plan plan;
    UFMLogger ufm;
    string dir = "data/";

    void load() {
        streambuf* old = cout.rdbuf(nullptr);          // loaders print to cout: mute them
        loadStudents(dir + "students.csv", stu);
        loadFaculty(dir + "faculty.csv", fac);
        loadRooms(dir + "rooms.csv", rooms);
        loadExams(dir + "exam_schedule.csv", exams);
        ufm.load(dir + "ufm_records.csv");
        cout.rdbuf(old);
        for (auto& kv : stu) sorted.push_back(kv.second);
        twoWayMergeSort(sorted, [](const Student& a, const Student& b) { return a.roll < b.roll; });
        plan.load(dir + "seating_plan.csv");
    }
};

string cmdSeating(Ctx& c, const vector<string>& a);   // dates, run, reset, rooms, room, student
string cmdOther(Ctx& c, const vector<string>& a);     // login, duty, ufm, faculty

