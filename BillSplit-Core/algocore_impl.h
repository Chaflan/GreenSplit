#ifndef ALGOCORE_IMPL_H
#define ALGOCORE_IMPL_H

#include <vector>
#include <string>
#include <unordered_map>

class AlgoCore_Impl
{
public:
    AlgoCore_Impl(unsigned int decimalPlaces);

    std::vector<std::tuple<std::string, std::string, double> >
        SolveGreedy(const std::unordered_map<std::string, double>& credits) const;

    std::vector<std::tuple<std::string, std::string, double> >
        SolveFewestTransfers(const std::unordered_map<std::string, double>& credits) const;

    void Validate(const std::unordered_map<std::string, double>& credits) const;

private:
    // Validate and alter credits to have rounded costs
    std::unordered_map<std::string, double>
        Prepare(std::unordered_map<std::string, double> credits) const;

    // Round d to m_decimalPlaces.  Throw if this results in overload.
    double Round(double d) const;

    // Helper methods that assume credits is rounded and validated.
    std::vector<std::tuple<std::string, std::string, double> >
        SolveGreedyValidated(const std::unordered_map<std::string, double>& credits) const;
    std::vector<std::tuple<std::string, std::string, double> >
        SolveFewestTransfersValidated(const std::unordered_map<std::string, double>& credits) const;

private:
    // Values between the margins (exclusive) are considered essentially zero
    // (a settled transaction) for the purpose of the algorithm.
    double m_pMargin;
    double m_nMargin;

    // Inputs are rounded to the nearest decimalPlaces
    unsigned int m_decimalPlaces;
};

#endif // ALGOCORE_IMPL_H
