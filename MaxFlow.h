#ifndef MAXFLOW_H
#define MAXFLOW_H

#include <vector>
#include <queue>
#include <algorithm>
#include <climits>

class AllocationFlow {
private:
    int n;
    std::vector<std::vector<int>> capacity;
    std::vector<std::vector<int>> adj;
    std::vector<std::pair<int, int>> originalEdges;

    bool bfs(int s, int t, std::vector<int>& parent) {
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
                    if (next == t) return true;
                    q.push({next, new_flow});
                }
            }
        }
        return false;
    }

public:
    AllocationFlow(int nodes) : n(nodes), capacity(nodes, std::vector<int>(nodes, 0)), adj(nodes) {}

    void addEdge(int u, int v, int cap) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        capacity[u][v] += cap;
        originalEdges.push_back({u, v});
    }

    // Edmonds-Karp Max Flow
    int getMaxMatches(int s, int t, std::vector<std::pair<int, int>>& matchedPairs) {
        int flow = 0;
        std::vector<int> parent(n);

        while (bfs(s, t, parent)) {
            int current = t;
            while (current != s) {
                int prev = parent[current];
                capacity[prev][current] -= 1;
                capacity[current][prev] += 1;
                current = prev;
            }
            flow += 1;
        }

        // Extract assignments from residual graph
        for (const auto& edge : originalEdges) {
            int u = edge.first;
            int v = edge.second;
            // For bipartite matching, we ignore edges from source and to sink
            if (u != s && v != t && capacity[u][v] == 0) {
                matchedPairs.push_back({u, v}); 
            }
        }
        return flow;
    }
};

#endif