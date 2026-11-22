#pragma once
#include "structs.h"
#include <unordered_map>
#include <vector>
using namespace std;

// csv loader helper functions
void loadUsers(const string& file, unordered_map<string, User>& umap);
void loadStudents(const string& file, unordered_map<string, Student>& smap);
void loadFaculty(const string& file, unordered_map<string, Faculty>& fmap);
void loadRooms(const string& file, vector<Room>& rooms);
void loadExams(const string& file, vector<Exam>& exams);
void loadUFM(const string& file, vector<UFM>& ufms);
