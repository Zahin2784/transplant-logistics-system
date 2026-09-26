#ifndef KNAPSACK_H
#define KNAPSACK_H

#include "Models.h"
#include <vector>
#include <algorithm>
#include <iostream>

class TransportLogistics {
public:
    // 0/1 Knapsack Algorithm
    static int optimizeHelicopterLoad(
        int maxCapacity,
        const std::vector<Organ>& matchedOrgans,
        const std::vector<int>& assignedPatientScores) {

        int numItems = static_cast<int>(matchedOrgans.size());
        std::vector<std::vector<int>> dp(
            numItems + 1,
            std::vector<int>(maxCapacity + 1, 0)
        );

        std::cout << "\n[0/1 Knapsack - Helicopter Logistics]\n";
        std::cout << "  Helicopter capacity = " << maxCapacity << "\n";
        std::cout << "  Each matched organ is an item.\n";
        std::cout << "  Weight = organ transport weight, "
                     "Value = patient's medical urgency.\n\n";

        for (int i = 1; i <= numItems; i++) {
            int weight = matchedOrgans[i - 1].weight;
            int value = assignedPatientScores[i - 1];

            std::cout << "  Considering Organ " << matchedOrgans[i - 1].id
                      << " (weight " << weight
                      << ", urgency value " << value << ")\n";

            for (int w = 0; w <= maxCapacity; w++) {
                if (weight <= w) {
                    dp[i][w] = std::max(
                        dp[i - 1][w],
                        dp[i - 1][w - weight] + value
                    );
                } else {
                    dp[i][w] = dp[i - 1][w];
                }
            }
        }

        std::cout << "\n  DP table (rows = organs, columns = capacity):\n";
        std::cout << "      ";
        for (int w = 0; w <= maxCapacity; ++w) {
            if (w % 5 == 0) std::cout << w << " ";
        }
        std::cout << "\n";

        // Print only every 5th capacity to keep the demo readable.
        for (int i = 0; i <= numItems; ++i) {
            std::cout << "  row " << i << ": ";
            for (int w = 0; w <= maxCapacity; ++w) {
                if (w % 5 == 0) std::cout << dp[i][w] << " ";
            }
            std::cout << "\n";
        }

        // Reconstruct the selected subset.
        std::vector<int> selected;
        int w = maxCapacity;

        for (int i = numItems; i > 0; --i) {
            if (dp[i][w] != dp[i - 1][w]) {
                selected.push_back(i - 1);
                w -= matchedOrgans[i - 1].weight;
            }
        }

        std::cout << "\n  Selected for the initial helicopter trip:\n";

        if (selected.empty()) {
            std::cout << "    No organ can be transported within capacity.\n";
        } else {
            int totalWeight = 0;
            int totalValue = 0;

            for (auto it = selected.rbegin(); it != selected.rend(); ++it) {
                int index = *it;
                totalWeight += matchedOrgans[index].weight;
                totalValue += assignedPatientScores[index];

                std::cout << "    Organ " << matchedOrgans[index].id
                          << " -> weight " << matchedOrgans[index].weight
                          << ", urgency value "
                          << assignedPatientScores[index] << "\n";
            }

            std::cout << "  Total selected weight = " << totalWeight
                      << " / " << maxCapacity << "\n";
            std::cout << "  Maximum urgency value secured = "
                      << totalValue << "\n";
        }

        return dp[numItems][maxCapacity];
    }
};

#endif
