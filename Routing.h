#ifndef ROUTING_H
#define ROUTING_H

#include "Models.h"
#include <vector>
#include <queue>
#include <climits>
#include <functional>
#include <iostream>

class RoutingNetwork {
private:
    int numHospitals;
    std::vector<std::vector<Edge>> adj;

public:
    RoutingNetwork(int n) : numHospitals(n), adj(n) {}

    void addRoute(int from, int to, int time) {
        adj[from].push_back({to, time});
        adj[to].push_back({from, time});
    }

    void printNetwork() const {
        std::cout << "Hospital network (travel times in minutes):\n";
        for (int u = 0; u < numHospitals; ++u) {
            std::cout << "  Hospital " << u << " -> ";
            for (const auto& edge : adj[u]) {
                std::cout << "[" << edge.to << ": " << edge.travelTime << " min] ";
            }
            std::cout << "\n";
        }
    }

    // Dijkstra's Algorithm
    std::vector<int> getShortestPaths(int sourceHospital, bool verbose = false) {
        std::vector<int> dist(numHospitals, INT_MAX);
        std::priority_queue<
            std::pair<int, int>,
            std::vector<std::pair<int, int>>,
            std::greater<std::pair<int, int>>
        > pq;

        dist[sourceHospital] = 0;
        pq.push({0, sourceHospital});

        if (verbose) {
            std::cout << "\n[Dijkstra] Starting from donor Hospital "
                      << sourceHospital << "\n";
        }

        while (!pq.empty()) {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (d > dist[u]) continue;

            if (verbose) {
                std::cout << "  Visiting Hospital " << u
                          << " with current shortest time " << d << " min\n";
            }

            for (const auto& edge : adj[u]) {
                int v = edge.to;
                int weight = edge.travelTime;

                if (dist[u] != INT_MAX &&
                    dist[u] + weight < dist[v]) {

                    int oldDist = dist[v];
                    dist[v] = dist[u] + weight;
                    pq.push({dist[v], v});

                    if (verbose) {
                        std::cout << "    Relax edge " << u << " -> " << v
                                  << " (" << weight << " min): ";
                        if (oldDist == INT_MAX)
                            std::cout << "new distance = " << dist[v] << " min\n";
                        else
                            std::cout << "updated distance = " << dist[v] << " min\n";
                    }
                }
            }
        }

        if (verbose) {
            std::cout << "\n  Shortest travel times from Hospital "
                      << sourceHospital << ":\n";
            for (int i = 0; i < numHospitals; ++i) {
                std::cout << "    Hospital " << i << ": ";
                if (dist[i] == INT_MAX)
                    std::cout << "unreachable\n";
                else
                    std::cout << dist[i] << " min\n";
            }
        }

        return dist;
    }
};

#endif
