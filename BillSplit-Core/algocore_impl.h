#ifndef ALGOCORE_IMPL_H
#define ALGOCORE_IMPL_H

#include <vector>
#include <string>
#include <unordered_map>

class AlgoCore_Impl
{
public:
    std::vector<std::tuple<std::string, std::string, double> >
        SolveGreedy(const std::unordered_map<std::string, double>& credits) const;

    std::vector<std::tuple<std::string, std::string, double> >
        SolveFewestTransfers(const std::unordered_map<std::string, double>& credits) const;

    void Validate(const std::unordered_map<std::string, double>& credits) const;

private:
    // Helper methods that assume credits is rounded and validated.  We do this because
    // fewest calls greedy and there is no need to validate twice.
    std::vector<std::tuple<std::string, std::string, double> >
        SolveGreedyValidated(const std::unordered_map<std::string, double>& credits) const;
    std::vector<std::tuple<std::string, std::string, double> >
        SolveFewestTransfersValidated(const std::unordered_map<std::string, double>& credits) const;

private:
    // Values between the margins (inclusive) are considered essentially zero
    // for the purpose of the algorithm.
    static constexpr double m_pMargin =  0.000001;
    static constexpr double m_nMargin = -0.000001;

    // Set to true for debug output
    static constexpr bool DEBUG = false;
};

#endif // ALGOCORE_IMPL_H
