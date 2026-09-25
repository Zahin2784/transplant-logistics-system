#ifndef ROUTING_H
#define ROUTING_H

#include "Models.h"
#include <vector>
#include <queue>
#include <climits>

class RoutingNetwork {
private:
    int numHospitals;
    std::vector<std::vector<Edge>> adj;

public:
    RoutingNetwork(int n) : numHospitals(n), adj(n) {}

    void addRoute(int from, int to, int time) {
        adj[from].push_back({to, time});
        adj[to].push_back({from, time}); // Assuming bidirectional roads/flights
    }

    // Dijkstra's Algorithm
    std::vector<int> getShortestPaths(int sourceHospital) {
        std::vector<int> dist(numHospitals, INT_MAX);
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;

        dist[sourceHospital] = 0;
        pq.push({0, sourceHospital});

        while (!pq.empty()) {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (d > dist[u]) continue;

            for (const auto& edge : adj[u]) {
                int v = edge.to;
                int weight = edge.travelTime;

                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    pq.push({dist[v], v});
                }
            }
        }
        return dist;
    }
};

#endif