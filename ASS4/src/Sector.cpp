#include "Sector.h"
#include <cmath>



Sector::Sector(int x, int y, int z) : x(x), y(y), z(z), left(nullptr), right(nullptr), parent(nullptr), color(RED) {
    // TODO: Calculate the distance to the Earth, and generate the sector code
    distance_from_earth = std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));

    sector_code += std::to_string(static_cast<int>(std::floor(distance_from_earth)));

    sector_code += (x == 0) ? "S" : (x > 0) ? "R" : "L";
    sector_code += (y == 0) ? "S" : (y > 0) ? "U" : "D";
    sector_code += (z == 0) ? "S" : (z > 0) ? "F" : "B";
}

Sector::~Sector() {
    // TODO: Free any dynamically allocated memory if necessary

}

Sector& Sector::operator=(const Sector& other) {
    // TODO: Overload the assignment operator
    return *this;
}

bool Sector::operator==(const Sector& other) const {
    return (x == other.x && y == other.y && z == other.z);
}

bool Sector::operator!=(const Sector& other) const {
    return !(*this == other);
}