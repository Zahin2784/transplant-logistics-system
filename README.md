# Transplant Logistics System

## Overview
The **Transplant Logistics System** is a C++ application designed to optimize the allocation and transportation of donated organs to patients in need. It evaluates biological compatibility, geographic distance, and cold ischemia time (CIT) constraints to maximize the number of successful transplants, while also optimizing the logistics of helicopter payload weight to prioritize patients with the highest medical urgency.

## Algorithms Used
This project utilizes several core graph and dynamic programming algorithms:

1. **Dijkstra's Algorithm**: 
   - **Purpose:** Computes the shortest travel times between hospitals in the routing network.
   - **Usage:** Ensures that an organ can be transported to a patient's hospital within its maximum Cold Ischemia Time (CIT).

2. **Edmonds-Karp Max Flow Algorithm (Bipartite Matching)**:
   - **Purpose:** Finds the optimal allocation of available organs to waitlisted patients.
   - **Usage:** Constructs a flow network with a source and sink, where edges between organs and patients represent biological and geographic compatibility. It computes the maximum cardinality matching to save the highest possible number of lives.

3. **0/1 Knapsack Algorithm**:
   - **Purpose:** Optimizes the logistics of helicopter transportation.
   - **Usage:** Given a helicopter with a maximum weight capacity, it selects the optimal subset of matched organs to transport on the initial flight, maximizing the total medical urgency score of the patients receiving those organs.

## Compilation and Execution
To compile and run the project, use a C++ compiler supporting at least C++11.

```powershell
# Compile the project
g++ main.cpp -o transplant_system

# Run the executable
./transplant_system
```

## Project Structure
- `main.cpp`: Entry point orchestrating the network routing, max flow matching, and knapsack logistics.
- `Models.h`: Defines the core data structures (`Patient`, `Organ`, `Edge`).
- `Routing.h`: Implements the `RoutingNetwork` class and Dijkstra's algorithm.
- `MaxFlow.h`: Implements the `AllocationFlow` class and the Edmonds-Karp algorithm for bipartite matching.
- `Knapsack.h`: Implements the `TransportLogistics` class and the 0/1 Knapsack algorithm.

## Output
When run, the system will output the successfully allocated organs matched to specific patients, along with the optimized logistics load plan for the helicopter transport.