#ifndef MAXFLOW_H
#define MAXFLOW_H

#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <iostream>

class AllocationFlow {
private:
    int n;
    std::vector<std::vector<int>> capacity;
    std::vector<std::vector<int>> adj;
    std::vector<std::pair<int, int>> originalEdges;

    bool bfs(int s, int t, std::vector<int>& parent, bool verbose) {
        std::fill(parent.begin(), parent.end(), -1);
        parent[s] = -2;

        std::queue<std::pair<int, int>> q;
        q.push({s, INT_MAX});

        while (!q.empty()) {
            int cur = q.front().first;
            int flow = q.front().second;
            q.pop();

            for (int next : adj[cur]) {
                if (parent[next] == -1 && capacity[cur][next] > 0) {
                    parent[next] = cur;
                    int new_flow = std::min(flow, capacity[cur][next]);

                    if (next == t) {
                        if (verbose) {
                            std::cout << "    Augmenting path found: ";
                            std::vector<int> path;
                            int node = t;
                            while (node != -2) {
                                path.push_back(node);
                                if (node == s) break;
                                node = parent[node];
                            }
                            for (int i = static_cast<int>(path.size()) - 1; i >= 0; --i) {
                                std::cout << path[i];
                                if (i > 0) std::cout << " -> ";
                            }
                            std::cout << " (flow +1)\n";
                        }
                        return true;
                    }

                    q.push({next, new_flow});
                }
            }
        }
        return false;
    }

public:
    AllocationFlow(int nodes)
        : n(nodes),
          capacity(nodes, std::vector<int>(nodes, 0)),
          adj(nodes) {}

    void addEdge(int u, int v, int cap) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        capacity[u][v] += cap;
        originalEdges.push_back({u, v});
    }

    // Edmonds-Karp Max Flow
    int getMaxMatches(int s, int t,
                      std::vector<std::pair<int, int>>& matchedPairs,
                      bool verbose = false) {

        int flow = 0;
        std::vector<int> parent(n);

        if (verbose) {
            std::cout << "\n[Edmonds-Karp / Max Flow]\n";
            std::cout << "  Source -> Organ nodes -> Patient nodes -> Sink\n";
            std::cout << "  Each capacity-1 path represents one possible transplant.\n";
        }

        while (bfs(s, t, parent, verbose)) {
            int current = t;

            while (current != s) {
                int prev = parent[current];
                capacity[prev][current] -= 1;
                capacity[current][prev] += 1;
                current = prev;
            }

            flow++;

            if (verbose) {
                std::cout << "    Current maximum matching = "
                          << flow << "\n";
            }
        }

        // Extract assignments from residual graph.
        for (const auto& edge : originalEdges) {
            int u = edge.first;
            int v = edge.second;

            // Ignore source->organ and patient->sink edges.
            if (u != s && v != t && capacity[u][v] == 0) {
                matchedPairs.push_back({u, v});
            }
        }

        if (verbose) {
            std::cout << "  No more augmenting paths.\n";
            std::cout << "  Final maximum number of successful transplants = "
                      << flow << "\n";
        }

        return flow;
    }
};

#endif
