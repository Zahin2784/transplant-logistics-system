#include <iostream>
#include <vector>
#include <queue>
#include "Models.h"
#include "Routing.h"
#include "MaxFlow.h"
#include "Knapsack.h"

// Comparator for Priority Queue (Sorting candidates by Urgency)
struct PatientCompare {
    bool operator()(const Patient& a, const Patient& b) {
        return a.urgencyScore < b.urgencyScore; // Max-Heap
    }
};

int main() {
    std::cout << "--- Transplant Logistics System Started ---\n\n";

    // 1. Initialize Network & Compute Shortest Paths (Dijkstra)
    RoutingNetwork network(5); // 5 Hospitals (0 to 4)
    network.addRoute(0, 1, 30); // 30 mins
    network.addRoute(0, 2, 120); 
    network.addRoute(1, 3, 45);
    network.addRoute(2, 4, 60);
    network.addRoute(3, 4, 25);

    int donorHospital = 0;
    std::vector<int> transitTimes = network.getShortestPaths(donorHospital);

    // 2. Initialize Organs and Waitlist (STRICT BACK-EDGE TEST CASE)
    std::vector<Organ> availableOrgans = {
        {1, "O", donorHospital, 240, 15}, // Organ 1: Type O (Universal - Matches Pat 101 & 102)
        {2, "B", donorHospital, 240, 20}  // Organ 2: Type B (Matches ONLY Pat 101)
    };

    std::vector<Patient> registry = {
        {101, "B", 40, 1}, // Pat 101: Type B (Urgency 40). Matches Organ 1 (O) AND Organ 2 (B)
        {102, "A", 30, 2}  // Pat 102: Type A (Urgency 30). Matches ONLY Organ 1 (O)
    };

    // 3. Triage & Sorting (Priority Queue)
    std::priority_queue<Patient, std::vector<Patient>, PatientCompare> waitlist;
    for (const auto& p : registry) {
        waitlist.push(p);
    }

    // 4. Build Flow Network for Allocation (Max Flow)
    int source = 0;
    int sink = 200; // Arbitrary high number for Sink ID
    AllocationFlow flowGraph(205); 

    // Add edges from Source to Organs
    for (const auto& organ : availableOrgans) {
        flowGraph.addEdge(source, organ.id, 1);
    }

    std::vector<Patient> triagedPatients;
    while (!waitlist.empty()) {
        triagedPatients.push_back(waitlist.top());
        waitlist.pop();
    }

    // Evaluate biological and geographic constraints
    for (const auto& organ : availableOrgans) {
        for (const auto& patient : triagedPatients) {
            // Blood compatibility (Simplified: exact match or O donor)
            bool isCompatible = (organ.bloodType == "O" || organ.bloodType == patient.bloodType);
            
            // CIT geographic check
            int travelTime = transitTimes[patient.hospitalId];
            bool inTime = (travelTime <= organ.maxCIT);

            if (isCompatible && inTime) {
                // Add bipartite edge
                flowGraph.addEdge(organ.id, patient.id, 1);
            }
        }
    }

    // Connect Patients to Sink
    for (const auto& patient : triagedPatients) {
        flowGraph.addEdge(patient.id, sink, 1);
    }

    // Execute Max Flow
    std::vector<std::pair<int, int>> matches;
    int totalTransplants = flowGraph.getMaxMatches(source, sink, matches);
    
    std::cout << "Optimal Allocation Resolved:\n";
    std::cout << "Total Successful Transplants: " << totalTransplants << "\n";
    
    std::vector<Organ> securedOrgans;
    std::vector<int> securedScores;
    
    for (const auto& match : matches) {
        std::cout << " -> Organ ID " << match.first << " assigned to Patient ID " << match.second << "\n";
        // Map back to objects for DP
        auto orgIt = std::find_if(availableOrgans.begin(), availableOrgans.end(), [&](const Organ& o){ return o.id == match.first; });
        auto patIt = std::find_if(triagedPatients.begin(), triagedPatients.end(), [&](const Patient& p){ return p.id == match.second; });
        if (orgIt != availableOrgans.end() && patIt != triagedPatients.end()) {
            securedOrgans.push_back(*orgIt);
            securedScores.push_back(patIt->urgencyScore);
        }
    }

    // 5. Run Logistics (DP Knapsack)
    int helicopterCapacity = 30; // Max payload
    int optimizedLoadValue = TransportLogistics::optimizeHelicopterLoad(helicopterCapacity, securedOrgans, securedScores);

    std::cout << "\nLogistics Load Plan (Helicopter Capacity: " << helicopterCapacity << "):\n";
    std::cout << "Maximum medical urgency value secured for initial transport: " << optimizedLoadValue << "\n";

    return 0;
}