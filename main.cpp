#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "Models.h"
#include "Routing.h"
#include "MaxFlow.h"
#include "Knapsack.h"

// Comparator for priority queue: higher urgency gets processed first.
struct PatientCompare {
    bool operator()(const Patient& a, const Patient& b) {
        return a.urgencyScore < b.urgencyScore;
    }
};

int main() {
    std::cout << "============================================================\n";
    std::cout << "           TRANSPLANT LOGISTICS SYSTEM DEMO\n";
    std::cout << "============================================================\n";
    std::cout << "Goal: allocate organs to compatible patients and plan\n";
    std::cout << "      their initial helicopter transportation.\n";

    // ---------------------------------------------------------
    // 1. ROUTING: DIJKSTRA
    // ---------------------------------------------------------
    std::cout << "\n============================================================\n";
    std::cout << "1. ROUTING - DIJKSTRA'S SHORTEST PATH ALGORITHM\n";
    std::cout << "============================================================\n";

    RoutingNetwork network(5);

    network.addRoute(0, 1, 30);
    network.addRoute(0, 2, 120);
    network.addRoute(1, 3, 45);
    network.addRoute(2, 4, 60);
    network.addRoute(3, 4, 25);

    int donorHospital = 0;

    network.printNetwork();

    std::cout << "\nWhy routing matters:\n";
    std::cout << "An organ has a maximum Cold Ischemia Time (CIT).\n";
    std::cout << "The shortest travel time must be within that limit.\n";

    std::vector<int> transitTimes =
        network.getShortestPaths(donorHospital, true);

    // ---------------------------------------------------------
    // 2. INPUT DATA
    // ---------------------------------------------------------
    std::cout << "\n============================================================\n";
    std::cout << "2. AVAILABLE ORGANS AND PATIENT WAITLIST\n";
    std::cout << "============================================================\n";

    std::vector<Organ> availableOrgans = {
        {1, "O", donorHospital, 240, 15},
        {2, "B", donorHospital, 240, 20}
    };

    std::vector<Patient> registry = {
        {101, "B", 40, 1},
        {102, "A", 30, 2}
    };

    std::cout << "Available organs:\n";
    for (const auto& organ : availableOrgans) {
        std::cout << "  Organ " << organ.id
                  << " | Blood type: " << organ.bloodType
                  << " | Weight: " << organ.weight
                  << " | Max CIT: " << organ.maxCIT << " min\n";
    }

    std::cout << "\nPatients:\n";
    for (const auto& patient : registry) {
        std::cout << "  Patient " << patient.id
                  << " | Blood type: " << patient.bloodType
                  << " | Urgency: " << patient.urgencyScore
                  << " | Hospital: " << patient.hospitalId << "\n";
    }

    // ---------------------------------------------------------
    // 3. TRIAGE / PRIORITY QUEUE
    // ---------------------------------------------------------
    std::cout << "\n============================================================\n";
    std::cout << "3. TRIAGE - PRIORITY QUEUE\n";
    std::cout << "============================================================\n";

    std::priority_queue<Patient,
                        std::vector<Patient>,
                        PatientCompare> waitlist;

    for (const auto& p : registry) {
        waitlist.push(p);
    }

    std::vector<Patient> triagedPatients;

    std::cout << "Patients are processed from highest to lowest urgency:\n";

    while (!waitlist.empty()) {
        Patient p = waitlist.top();
        waitlist.pop();

        triagedPatients.push_back(p);

        std::cout << "  -> Patient " << p.id
                  << " selected for consideration (urgency "
                  << p.urgencyScore << ")\n";
    }

    // ---------------------------------------------------------
    // 4. BUILD FLOW NETWORK
    // ---------------------------------------------------------
    std::cout << "\n============================================================\n";
    std::cout << "4. ORGAN ALLOCATION - EDMONDS-KARP MAX FLOW\n";
    std::cout << "============================================================\n";

    int source = 0;
    int sink = 200;

    AllocationFlow flowGraph(205);

    std::cout << "\nFlow network construction:\n";
    std::cout << "  Source -> Organ -> Compatible Patient -> Sink\n";

    for (const auto& organ : availableOrgans) {
        flowGraph.addEdge(source, organ.id, 1);
        std::cout << "  Added Source -> Organ " << organ.id << "\n";
    }

    std::cout << "\nChecking compatibility and travel constraints:\n";

    for (const auto& organ : availableOrgans) {
        for (const auto& patient : triagedPatients) {

            bool isCompatible =
                (organ.bloodType == "O" ||
                 organ.bloodType == patient.bloodType);

            int travelTime = transitTimes[patient.hospitalId];
            bool inTime = (travelTime <= organ.maxCIT);

            std::cout << "  Organ " << organ.id
                      << " -> Patient " << patient.id << ": ";

            if (isCompatible && inTime) {
                flowGraph.addEdge(organ.id, patient.id, 1);

                std::cout << "COMPATIBLE";
                std::cout << " | blood OK";
                std::cout << " | travel " << travelTime
                          << " <= CIT " << organ.maxCIT;
                std::cout << " | edge ADDED\n";
            } else {
                std::cout << "REJECTED";

                if (!isCompatible)
                    std::cout << " | blood incompatible";

                if (!inTime)
                    std::cout << " | travel time " << travelTime
                              << " > CIT " << organ.maxCIT;

                std::cout << "\n";
            }
        }
    }

    for (const auto& patient : triagedPatients) {
        flowGraph.addEdge(patient.id, sink, 1);
    }

    std::cout << "\nRunning Edmonds-Karp to find the maximum number of\n";
    std::cout << "organ-patient matches...\n";

    std::vector<std::pair<int, int>> matches;

    int totalTransplants =
        flowGraph.getMaxMatches(source, sink, matches, true);

    // ---------------------------------------------------------
    // 5. FINAL ALLOCATION
    // ---------------------------------------------------------
    std::cout << "\n============================================================\n";
    std::cout << "5. ALLOCATION RESULT\n";
    std::cout << "============================================================\n";

    std::cout << "Total Successful Transplants: "
              << totalTransplants << "\n";

    std::vector<Organ> securedOrgans;
    std::vector<int> securedScores;

    for (const auto& match : matches) {
        std::cout << "  Organ ID " << match.first
                  << " assigned to Patient ID "
                  << match.second << "\n";

        auto orgIt = std::find_if(
            availableOrgans.begin(),
            availableOrgans.end(),
            [&](const Organ& o) {
                return o.id == match.first;
            });

        auto patIt = std::find_if(
            triagedPatients.begin(),
            triagedPatients.end(),
            [&](const Patient& p) {
                return p.id == match.second;
            });

        if (orgIt != availableOrgans.end() &&
            patIt != triagedPatients.end()) {

            securedOrgans.push_back(*orgIt);
            securedScores.push_back(patIt->urgencyScore);
        }
    }

    // ---------------------------------------------------------
    // 6. KNAPSACK
    // ---------------------------------------------------------
    std::cout << "\n============================================================\n";
    std::cout << "6. LOGISTICS - 0/1 KNAPSACK\n";
    std::cout << "============================================================\n";

    int helicopterCapacity = 30;

    int optimizedLoadValue =
        TransportLogistics::optimizeHelicopterLoad(
            helicopterCapacity,
            securedOrgans,
            securedScores
        );

    // ---------------------------------------------------------
    // 7. FINAL SUMMARY
    // ---------------------------------------------------------
    std::cout << "\n============================================================\n";
    std::cout << "                 FINAL SYSTEM SUMMARY\n";
    std::cout << "============================================================\n";

    std::cout << "Successful transplants : "
              << totalTransplants << "\n";

    std::cout << "Helicopter capacity    : "
              << helicopterCapacity << "\n";

    std::cout << "Maximum urgency value  : "
              << optimizedLoadValue << "\n";

    std::cout << "\nThe system demonstrates three algorithmic stages:\n";
    std::cout << "  1. Dijkstra   -> finds feasible shortest transport times.\n";
    std::cout << "  2. Max Flow   -> finds the maximum organ-patient matching.\n";
    std::cout << "  3. Knapsack   -> chooses the best initial helicopter load.\n";

    std::cout << "\n============================================================\n";
    std::cout << "                 DEMONSTRATION COMPLETE\n";
    std::cout << "============================================================\n";

    return 0;
}
