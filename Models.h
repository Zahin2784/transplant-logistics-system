#ifndef MODELS_H
#define MODELS_H

#include <vector>
#include <string>

struct Patient {
    int id;
    std::string bloodType;
    int urgencyScore;  // Composite score (MELD, etc.)
    int hospitalId;
};

struct Organ {
    int id;
    std::string bloodType;
    int sourceHospitalId;
    int maxCIT;        // Cold Ischemia Time in minutes
    int weight;        // Physical weight/volume for transport
};

struct Edge {
    int to;
    int travelTime;
};

#endif