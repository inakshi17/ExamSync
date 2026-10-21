#pragma once
#include "structs.h"
#include <unordered_map>
#include <vector>
using namespace std;

// ─────────────────────────────────────────────
//  loader.h  –  Declarations for all CSV
//  loading functions used in ExamSync.
//
//  Each function opens one CSV file, skips the
//  header row, and fills the given container.
// ─────────────────────────────────────────────


// Load Authentication.csv  →  map: username → User
void loadUsers(const string& file, unordered_map<string, User>& umap);

// Load students.csv  →  map: roll → Student
void loadStudents(const string& file, unordered_map<string, Student>& smap);

// Load faculty.csv  →  map: fid → Faculty
void loadFaculty(const string& file, unordered_map<string, Faculty>& fmap);

// Load rooms.csv  →  vector of Room structs
void loadRooms(const string& file, vector<Room>& rooms);

// Load subject_codes.csv  →  vector of Subject structs
void loadSubjects(const string& file, vector<Subject>& subs);

// Load exam_schedule.csv  →  vector of Exam structs
void loadExams(const string& file, vector<Exam>& exams);

// Load ufm_records.csv  →  vector of UFM structs
void loadUFM(const string& file, vector<UFM>& ufms);
