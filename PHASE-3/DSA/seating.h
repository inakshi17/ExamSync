#pragma once

#include "structs.h"
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

// 2D seating grid engine for a single classroom
class SeatingEngine {
public:
    vector<vector<Seat>> grid;

    int rows, cols; 
    string roomName; 
    string building; 
    string floor; 
    int capacity; 

    // initialize grid dimensions based on room capacity
    void init(const Room& rm);

    // check if student can sit at (r, c): vertical neighbor in same column is permitted,
    // horizontal and diagonal neighbors MUST NOT share the same paper code
    bool canPlace(int r, int c, const string& code);

    // place student into the first available valid seat
    bool placeStu(const Student& stu);

    // print classroom 2D seating layout
    void printGrid();

    // count remaining empty desks
    int emptySeats();
};

// strategy interface for seating algorithms (OOP concept)
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

// optimal vertical column interleaving strategy (max 4 paper codes per room)
class OptimalInterleavingStrategy : public ISeatingStrategy {
public:
    string getStrategyName() const override {
        return "Optimal Vertical Column Interleaving (Max 4 Codes/Room)";
    }

    unordered_map<string, Allotment> allocate(
        const vector<Student>& stus,
        const vector<Room>& rooms,
        bool printSummary
    ) override;
};

// context manager to execute seating strategy
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

// Allocate seats for students.
// preAssigned: students already in seatMap — their seats are pre-marked as
// occupied so new students don't collide with them on the same date.
// Students in preAssigned are copied directly to result (seat unchanged).
unordered_map<string, Allotment>
allocateSeats(const vector<Student>& stus,
              const vector<Room>& rooms,
              const unordered_map<string, Allotment>& preAssigned,
              bool printSummary = false);

// Convenience overload — no pre-assigned seats (first-time full allocation)
unordered_map<string, Allotment>
allocateSeats(const vector<Student>& stus, const vector<Room>& rooms, bool printSummary = false);
