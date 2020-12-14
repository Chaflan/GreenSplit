#ifndef ALGOCORE_H
#define ALGOCORE_H

#include <vector>
#include <string>
#include <unordered_map>

// TODO: its possible to set your margins as one thing and to have a solution problem
// TODO: need a fewest transaction and lowest transfer cost algo
// TODO: review data structure choice here.  Order is irrelevant with return value.

//---------------------------------------------------------------------------------------
// A namespace of functions to solve the fewest transactions problem.
//
// Input (credits): a mapping of names (string) to amount of money owed to them (double).
//      This amount can be negative indicating that it is a debt; money they owe.
//      To whom is irrelevant.
// Output (return value): A sequence of payments/transactions to be made to solve the
//      problem.  Order is arbitrary. Tuple elements are <from, to, amount>.
//      e.g. <"Mary", "Tom", 50> is a transaction where Mary pays Tom 50 units of currency.
//---------------------------------------------------------------------------------------
namespace AlgoCore
{
    //---------------------------------------------------------------------------------------
    // Greedy algorithm, fast but non-optimal for some cases.
    // These cases will be ones where a subset of the payments can be solved in isolation.
    // These could be common with larger sets, whole number prices, and/or a large margin
    //      n = debts.size()
    //      Time -> O(n)
    //      Space -> O(n)
    //---------------------------------------------------------------------------------------
    std::vector<std::tuple<std::string, std::string, double> >
        SolveGreedy(const std::unordered_map<std::string, double>& credits);

    //---------------------------------------------------------------------------------------
    // Solves the problem such that you have the fewest number of transactions possible.
    // It is always slower than greedy.
    //      n = debts.size()
    //      Time -> O(n)
    //      Space -> O(n)
    //---------------------------------------------------------------------------------------
    std::vector<std::tuple<std::string, std::string, double> >
        SolveFewestTransfers(const std::unordered_map<std::string, double>& credits);

    //---------------------------------------------------------------------------------------
    // Throws an invalid argument exception if its impossible to solve the passed credits.
    //---------------------------------------------------------------------------------------
    void Validate(const std::unordered_map<std::string, double>& credits);

    // Values between the margins (inclusive) are considered essentially zero
    // (a settled transaction) for the purpose of the algorithm
    static constexpr double pMargin = 0.01;
    static constexpr double nMargin = pMargin * -1;
};

#endif // ALGOCORE_H
