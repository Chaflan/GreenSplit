#ifndef ALGOCORE_H
#define ALGOCORE_H

#include <vector>
#include <string>
#include <unordered_map>

// TODO: from, to, cost explain in comments
namespace AlgoCore
{
    //---------------------------------------------------------------------------------------
    // Greedy algorithm, fast but non-optimal for some cases.
    // These cases will be ones where a subset of the payments can be solved in isolation.
    // These could be common with larger sets, whole number prices, and/or a large margin
    //      n = debts.size()
    //      Time -> O(n)
    //      Space (solution vector) -> O(n)
    //      Space (other required to solve) -> O(1)
    //---------------------------------------------------------------------------------------
    std::vector<std::tuple<std::string, std::string, double> >
        SolveGreedy(std::unordered_map<std::string, double> debts);

    //---------------------------------------------------------------------------------------
    // Brute force recursive tree algorithm, slow but optimal
    // TODO: Note about how solution requires solution to the ____ problem which is open
    //---------------------------------------------------------------------------------------
    std::vector<std::tuple<std::string, std::string, double> >
        SolveOptimal(std::unordered_map<std::string, double> debts);

    // Values between the margins (inclusive) are considered essentially zero
    // (a settled transaction) for the purpose of the algorithm
    static constexpr double pMargin = 0.01;
    static constexpr double nMargin = pMargin * -1;
};

#endif // ALGOCORE_H
