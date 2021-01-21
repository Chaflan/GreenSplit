#include "algocore.h"
#include "algocore_impl.h"

AlgoCore::AlgoCore()
    : impl( std::make_unique<AlgoCore_Impl>())
{ }

// Needed to satisfy unique_ptr with incomplete type
AlgoCore::~AlgoCore() = default;

std::vector<std::tuple<std::string, std::string, double> >
    AlgoCore::SolveGreedy(const std::unordered_map<std::string, double>& credits) const
{
    return impl->SolveGreedy(credits);
}

std::vector<std::tuple<std::string, std::string, double> >
    AlgoCore::SolveFewestTransfers(const std::unordered_map<std::string, double>& credits) const
{
    return impl->SolveFewestTransfers(credits);
}

void AlgoCore::Validate(const std::unordered_map<std::string, double>& credits) const
{
    return impl->Validate(credits);
}
