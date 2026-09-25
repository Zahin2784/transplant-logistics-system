#ifndef KNAPSACK_H
#define KNAPSACK_H

#include "Models.h"
#include <vector>
#include <algorithm>

class TransportLogistics {
public:
    // 0/1 Knapsack Algorithm
    static int optimizeHelicopterLoad(int maxCapacity, const std::vector<Organ>& matchedOrgans, const std::vector<int>& assignedPatientScores) {
        int numItems = matchedOrgans.size();
        std::vector<std::vector<int>> dp(numItems + 1, std::vector<int>(maxCapacity + 1, 0));

        for (int i = 1; i <= numItems; i++) {
            int weight = matchedOrgans[i - 1].weight;
            int value = assignedPatientScores[i - 1]; // Value is the saved patient's urgency score

            for (int w = 0; w <= maxCapacity; w++) {
                if (weight <= w) {
                    dp[i][w] = std::max(dp[i - 1][w], dp[i - 1][w - weight] + value);
                } else {
                    dp[i][w] = dp[i - 1][w];
                }
            }
        }
        return dp[numItems][maxCapacity];
    }
};

#endif