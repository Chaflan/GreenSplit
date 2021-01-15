#ifndef ALGOCORE_H
#define ALGOCORE_H

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

class AlgoCore_Impl;

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
class AlgoCore
{
public:
    //---------------------------------------------------------------------------------------
    // Use decimalPlaces to set the significance of the solution.  Input costs are rounded to
    // this before processing.  Not doing this can lead to solutions that pass
    // validation, but combine in such a way to stack rounding errors yielding an unsolvable
    // solution upon processing.
    //
    // An exception will be thrown if this results in exceeding the numeric limits of a double.
    //---------------------------------------------------------------------------------------
    AlgoCore(double decimalPlaces = 2);
    ~AlgoCore();

    //---------------------------------------------------------------------------------------
    // Greedy algorithm, fast but non-optimal for some cases.
    // These cases will be ones where a subset of the payments can be solved in isolation
    // and that solution is not preferred over a higher value settlement.
    // These could be common with larger sets and rounded number costs.
    //      n = debts.size()
    //      Time -> O(n)
    //      Space -> O(n)
    //---------------------------------------------------------------------------------------
    std::vector<std::tuple<std::string, std::string, double> >
        SolveGreedy(const std::unordered_map<std::string, double>& credits) const;

    //---------------------------------------------------------------------------------------
    // Solves the problem such that you have the fewest number of transactions possible.
    // It is always slower than greedy.
    //      n = debts.size()

    // TODO, really?

    //      Time -> O(n)
    //      Space -> O(n)
    //---------------------------------------------------------------------------------------
    std::vector<std::tuple<std::string, std::string, double> >
        SolveFewestTransfers(const std::unordered_map<std::string, double>& credits) const;

    //---------------------------------------------------------------------------------------
    // Check the passed credits for values that overflow upon rounding (due to too many
    // decimalPlaces or very high costs) and to ensure it is possible to solve the passed credits.
    // That is, they sum to a zero cost.  Invalid argument excepsions are thrown upon both
    // failures.  Solve functions will also validate before processing.
    //---------------------------------------------------------------------------------------
    void Validate(const std::unordered_map<std::string, double>& credits) const;

private:
    std::unique_ptr<AlgoCore_Impl> impl;
};

#endif // ALGOCORE_H
