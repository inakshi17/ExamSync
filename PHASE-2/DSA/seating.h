#pragma once

#include "structs.h"
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class SeatingEngine {
public:
    vector<vector<Seat>> grid;

    int rows, cols; 
    string roomName; 
    string building; 
    string floor; 
    int capacity; 

    void init(const Room& rm);

    bool canPlace(int r, int c, const string& branch);

    bool placeStu(const Student& stu);

    void printGrid();

    int emptySeats();
};

class ISeatingStrategy {
public:
    virtual ~ISeatingStrategy() = default;

    virtual string getStrategyName() const = 0;

    virtual unordered_map<string, Allotment> allocate(
        const vector<Student>& stus,
        const vector<Room>& rooms,
        bool printSummary
    ) = 0;
};

class OptimalInterleavingStrategy : public ISeatingStrategy {
public:
    string getStrategyName() const override {
        return "Optimal Multi-Branch Round-Robin Interleaving";
    }

    unordered_map<string, Allotment> allocate(
        const vector<Student>& stus,
        const vector<Room>& rooms,
        bool printSummary
    ) override;
};

class SeatingManager {
private:
    ISeatingStrategy* strategy; 

public:
    SeatingManager(ISeatingStrategy* strat) : strategy(strat) {}

    void setStrategy(ISeatingStrategy* newStrat) {
        strategy = newStrat;
    }

    string getCurrentStrategy() const {
        return strategy ? strategy->getStrategyName() : "None";
    }

    unordered_map<string, Allotment> execute(
        const vector<Student>& stus,
        const vector<Room>& rooms,
        bool printSummary
    ) {
        if (!strategy) return {};
        return strategy->allocate(stus, rooms, printSummary);
    }
};

unordered_map<string, Allotment>
allocateSeats(const vector<Student>& stus, const vector<Room>& rooms, bool printSummary = false);
