#ifndef ALGOCORE_H
#define ALGOCORE_H

#include <vector>
#include <string>
#include <unordered_map>

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
class AlgoCore
{
public:
    //---------------------------------------------------------------------------------------
    // Greedy algorithm, fast but non-optimal for some cases.
    // These cases will be ones where a subset of the payments can be solved in isolation.
    // These could be common with larger sets, whole number prices, and/or a large margin
    //      n = debts.size()
    //      Time -> O(n)
    //      Space (solution vector) -> O(n)
    //      Space (other required to solve) -> O(1)
    //---------------------------------------------------------------------------------------
    static std::vector<std::tuple<std::string, std::string, double> >
        SolveGreedy(std::unordered_map<std::string, double> debts);

    //---------------------------------------------------------------------------------------
    // Brute force recursive tree algorithm, slow but optimal
    // TODO: Note about how solution requires solution to the ____ problem which is open
    //---------------------------------------------------------------------------------------
    std::vector<std::tuple<std::string, std::string, double> >
        SolveOptimal(std::unordered_map<std::string, double> debts) const;

private:

    bool SolveOptimalRecurse(
        std::size_t pcount,
        std::size_t ncount,
        int numT) const;

private:

    struct ITransaction {
        std::size_t from;
        std::size_t to;
        double cost;
    };

    // TODO: Rename: mst_

    // Values between the margins (inclusive) are considered essentially zero
    // (a settled transaction) for the purpose of the algorithm
    static constexpr double mst_pmargin = 0.01;
    static constexpr double mst_nmargin = mst_pmargin * -1;

    // The nature of this class is to keep this data for caching and
    // preallocation of data for the optimal solution.  Therefore we keep it mutable.
    mutable int mst_solnNumT = 0;
    mutable std::vector<std::vector<double> > mst_psets;
    mutable std::vector<std::vector<double> > mst_nsets;
    mutable std::vector<ITransaction> mst_finalSoln;
    mutable std::vector<ITransaction> mst_currSoln;
};

#endif // ALGOCORE_H
