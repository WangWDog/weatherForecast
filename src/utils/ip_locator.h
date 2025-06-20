#pragma once
#include <string>

struct Location {
    std::string city;
    double latitude;
    double longitude;
};

Location getLocationByIP();