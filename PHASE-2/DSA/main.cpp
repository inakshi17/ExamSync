#include "portal.h"
#include <iostream>
using namespace std;

int main() {
    Portal p;

    p.loadAll("data/");

    // Start the interactive portal
    p.run();

    return 0;
}
