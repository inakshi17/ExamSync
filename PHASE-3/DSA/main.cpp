#include "portal.h"
#include <iostream>
using namespace std;

// application entry point
int main() {
    Portal p;
    // load all csv files from data directory
    p.loadAll("data/");

    // launch interactive portal loop
    p.run();

    return 0;
}
