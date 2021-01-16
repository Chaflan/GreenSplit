#include "algocore_impl.h"
#include <queue>
#include <cmath>

AlgoCore_Impl::AlgoCore_Impl(unsigned int decimalPlaces)
    : m_decimalPlaces(decimalPlaces)
{
    // Margins need one extra level of significance.  Otherwise, rounding errors in a double mean that
    // 0.01 = 0.010000000000000007 or similar causing us to treat a cent as zero.
    const unsigned int marginPlaces = m_decimalPlaces + 1;

    if (marginPlaces > std::numeric_limits<double>::digits10) {
        throw std::invalid_argument("Decimal places too large.  Overflow will occur.");
    }

    m_pMargin = std::pow(static_cast<double>(10), -1 * static_cast<int>(marginPlaces));
    m_nMargin = -1 * m_pMargin;
}

std::vector<std::tuple<std::string, std::string, double> >
    AlgoCore_Impl::SolveGreedy(const std::unordered_map<std::string, double>& credits) const
{
    return SolveGreedyValidated(Prepare(credits));
}

std::vector<std::tuple<std::string, std::string, double> >
    AlgoCore_Impl::SolveFewestTransfers(const std::unordered_map<std::string, double>& credits) const
{
    return SolveFewestTransfersValidated(Prepare(credits));
}

void AlgoCore_Impl::Validate(const std::unordered_map<std::string, double>& credits) const
{
    double sum = 0;
    for (const auto& [ name, debt ]  : credits) {
        sum += Round(debt);
    }

    if(sum < m_nMargin || m_pMargin < sum) {
        throw std::invalid_argument("impossible to solve debts");
    }
}

double AlgoCore_Impl::Round(double d) const
{
    static const double tens = std::pow(10, m_decimalPlaces);
    if (std::numeric_limits<double>::max() / tens < d) {
        throw std::invalid_argument(
            std::string("value %f overflows upon rounding to %d decimal places", d, m_decimalPlaces));
    }

    return std::round(d * tens) / tens;
}

std::vector<std::tuple<std::string, std::string, double> >
    AlgoCore_Impl::SolveGreedyValidated(const std::unordered_map<std::string, double>& credits) const
{
    std::vector<std::tuple<std::string, std::string, double> > res;

    // Create two priority queues; one for negative costs, and one for positive.
    // Pair costs with indexes that yield the associated names

    const auto pairLessClosure =
        [](const std::pair<double, std::size_t>& l, const std::pair<double, std::size_t>& r){
            return l.first < r.first;
        };
    std::priority_queue<
        std::pair<double, std::size_t>,
        std::vector<std::pair<double, std::size_t> >,
        decltype(pairLessClosure) > pQueue(pairLessClosure);

    const auto pairGreaterClosure =
        [](const std::pair<double, std::size_t>& l, const std::pair<double, std::size_t>& r){
            return l.first > r.first;
        };
    std::priority_queue<
        std::pair<double, std::size_t>,
        std::vector<std::pair<double, std::size_t> >,
        decltype(pairGreaterClosure) > nQueue(pairGreaterClosure);

    std::vector<std::string> pNameLookup;
    std::vector<std::string> nNameLookup;

    for (const auto& [name, cost] : credits) {
        if (cost < m_nMargin) {
            nQueue.emplace(cost, nNameLookup.size());
            nNameLookup.push_back(name);
        } else if (cost > m_pMargin) {
            pQueue.emplace(cost, pNameLookup.size());
            pNameLookup.push_back(name);
        }
    }

    // Solve the problem by pairing largest positive with largest negative, push remainders
    while(!pQueue.empty()) {
        const double payment = std::min(pQueue.top().first, -1 * nQueue.top().first);
        const double remainder = pQueue.top().first + nQueue.top().first;
        const size_t pIndex = pQueue.top().second;
        const size_t nIndex = nQueue.top().second;

        pQueue.pop();
        nQueue.pop();

        if (remainder < m_nMargin) {
            nQueue.emplace(remainder, nIndex);
        } else if (remainder > m_pMargin) {
            pQueue.emplace(remainder, pIndex);
        }

        res.emplace_back(nNameLookup[nIndex], pNameLookup[pIndex], payment);
    }

    return res;
}

std::vector<std::tuple<std::string, std::string, double> >
    AlgoCore_Impl::SolveFewestTransfersValidated(const std::unordered_map<std::string, double>& credits) const
{
    // Prime the final solution using the greedy solution.  The optimal tree solution
    // will attempt to improve upon this, but won't be able to in most cases.
    std::vector<std::tuple<std::string, std::string, double> > solnFinalStr = SolveGreedyValidated(credits);
    int numTransFinal = static_cast<int>(solnFinalStr.size());

    // Fewer than 5 transactions is guaranteed optimal already.  I have a proof in my notebook.
    // Example of a 5 that can be optimized: 4,3,-3,-2,-2
    if (numTransFinal >= 5) {
        const int numTransOriginal = numTransFinal;

        // Sets of positive and negative costs in the debts object.
        std::vector<double> pFirstSet;
        std::vector<double> nFirstSet;

        // To save on copying strings around we set up an indexing system.
        std::vector<std::string> pNameLookup;
        std::vector<std::string> nNameLookup;
        std::vector<std::tuple<std::size_t, std::size_t, double> > solnFinalIdx(numTransOriginal - 1);
        std::vector<std::tuple<std::size_t, std::size_t, double> > solnCurrIdx(numTransOriginal - 1);
        for (const auto& [name, cost] : credits) {
            if (cost < m_nMargin) {
                nNameLookup.push_back(name);
                nFirstSet.push_back(cost);
            } else if (cost > m_pMargin) {
                pNameLookup.push_back(name);
                pFirstSet.push_back(cost);
            }
        }

        // The algorithm is intuitively recursive but can be solved iteratively with for loops and by preserving
        // the below information in each stack frame.  Doing it this way allows us to preallocate for this data
        // since we know the depth of the stack can never exceed the size of the greedy solution.  Depth first
        // traversal preserves frame information higher up the tree until it is no longer needed.
        struct StackFrame {
            std::size_t pix;
            std::size_t nix;
            std::vector<double> pset;
            std::vector<double> nset;
            std::size_t pcount;
            std::size_t ncount;
        };
        const size_t pSetSize = pFirstSet.size();
        const size_t nSetSize = nFirstSet.size();
        std::vector<StackFrame> st(numTransOriginal, { 0, 0, pFirstSet, nFirstSet, pSetSize, nSetSize});

        // Note that tree depth == number of transactions
        for(int depth = 0; depth >= 0;) {

            if (static_cast<int>(std::max(st[depth].pcount, st[depth].ncount)) + depth >= numTransFinal) {
                // Base Case: Can't possibly improve upon final solution
                --depth;
                continue;
            }

            if (st[depth].pcount == 0 && st[depth].ncount == 0) {
                // Base case: Solution found
                numTransFinal = depth;
                solnFinalIdx = solnCurrIdx;
                --depth;
                continue;
            }

            // We use this as a sentinel to break out of the two for loops below and then descend the tree.
            // We break on p but continue on n because descent happens after we are finished with the current n index.
            bool descend = false;

            for (auto& p = st[depth].pix; p < pSetSize; ++p) {
                if (st[depth].pset[p] <= m_pMargin) {
                    continue;
                }

                for (auto& n = st[depth].nix; n < nSetSize && !descend; ++n) {
                    if (st[depth].nset[n] >= m_nMargin) {
                        continue;
                    }

                    const double payment = std::min(st[depth].pset[p], -1 * st[depth].nset[n]);
                    const double remainder = st[depth].pset[p] + st[depth].nset[n];

                    // Add this transaction to the current solution
                    std::get<0>(solnCurrIdx[depth]) = n;
                    std::get<1>(solnCurrIdx[depth]) = p;
                    std::get<2>(solnCurrIdx[depth]) = payment;

                    // Prime next recurse/descent, curr stack frame is intact to continuse the loop
                    // when we come back.
                    const int depthNext = depth + 1;
                    st[depthNext].pset = st[depth].pset;
                    st[depthNext].nset = st[depth].nset;
                    st[depthNext].pcount = st[depth].pcount;
                    st[depthNext].ncount = st[depth].ncount;
                    if (remainder < 0) {
                        st[depthNext].nset[n] = remainder;
                        st[depthNext].pset[p] = 0;
                        st[depthNext].pcount--;
                        if (remainder >= m_nMargin) {
                            st[depthNext].ncount--;
                        }
                    } else {
                        st[depthNext].nset[n] = 0;
                        st[depthNext].pset[p] = remainder;
                        st[depthNext].ncount--;
                        if (remainder <= m_pMargin) {
                            st[depthNext].pcount--;
                        }
                    }

                    descend = true;
                }

                if (descend) {
                    break;
                }
            }

            if (descend) {;
                ++depth;
            } else {
                // No solutions in this branch of the tree, ascend
                --depth;
            }
        }

        // If numTrans didn't change, then the final solution was unchanged from greedy and we can just return it.
        // Otherwise we need to convert the indexed solution to a string solution using the lookups.
        if (numTransOriginal != numTransFinal) {
            solnFinalStr.clear();
            for (const auto& iTrans : solnFinalIdx) {
                solnFinalStr.emplace_back(
                    nNameLookup[std::get<0>(iTrans)],
                    pNameLookup[std::get<1>(iTrans)],
                    std::get<2>(iTrans));
            }
        }
    }

    return solnFinalStr;
}

std::unordered_map<std::string, double> AlgoCore_Impl::Prepare(std::unordered_map<std::string, double> credits) const
{
    double sum = 0;
    for (auto& [ name, debt ]  : credits) {
        debt = Round(debt);
        sum += debt;
    }

    if(sum < m_nMargin || m_pMargin < sum) {
        throw std::invalid_argument("impossible to solve debts");
    }

    return credits;
}
