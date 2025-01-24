#include "Sector.h"
#include <cmath>
#include <iostream>

using namespace std;

// Constructor implementation

Sector::Sector(int x, int y, int z) : x(x), y(y), z(z), left(nullptr), right(nullptr), parent(nullptr), color(RED) {
    // TODO: Calculate the distance to the Earth, and generate the sector code
    distance_from_earth = sqrt(x*x + y*y + z*z);
    sector_code = to_string(static_cast<int>((floor(distance_from_earth))));
    if (x==0) {
        sector_code+='S';
    } else if (x>0) {
        sector_code+='R';
    } else {
        sector_code+='L';
    }
    if (y==0) {
        sector_code+='S';
    } else if (y>0) {
        sector_code+='U';
    } else {
        sector_code+='D';
    }
    if (z==0) {
        sector_code+='S';
    } else if (z>0) {
        sector_code+='F';
    } else {
        sector_code+='B';
    }
}

Sector::~Sector() {
    // TODO: Free any dynamically allocated memory if necessary
}

Sector& Sector::operator=(const Sector& other) {
    // TODO: Overload the assignment operator
    if (this != &other) { // Check for self-assignment
        x = other.x;
        y = other.y;
        z = other.z;
        distance_from_earth = other.distance_from_earth;
        sector_code = other.sector_code;
    }
    return *this;
}

bool Sector::operator==(const Sector& other) const {
    return (x == other.x && y == other.y && z == other.z);
}

bool Sector::operator!=(const Sector& other) const {
    return !(*this == other);
}