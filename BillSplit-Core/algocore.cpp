#include "algocore.h"
#include <queue>

// TODO: Remove with debug, or add debug macros
#include <iostream>
#include <cassert>

// Assumes debts can be settled
std::vector<std::tuple<std::string, std::string, double> >
    AlgoCore::SolveGreedy(std::unordered_map<std::string, double> debts)
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

    for (const auto& [name, cost] : debts) {
        if (cost < mst_nmargin) {
            nQueue.emplace(cost, nNameLookup.size());
            nNameLookup.push_back(name);
        } else if (cost > mst_pmargin) {
            pQueue.emplace(cost, pNameLookup.size());
            pNameLookup.push_back(name);
        }
    }

    // Solve the problem by pairing largest positive with largest negative, push remainders

    while(!pQueue.empty()) {
        const double remainder = pQueue.top().first + nQueue.top().first;
        const double payment = std::min(pQueue.top().first, -1 * nQueue.top().first);
        const size_t pindex = pQueue.top().second;
        const size_t nindex = nQueue.top().second;

        pQueue.pop();
        nQueue.pop();

        if (remainder < mst_nmargin) {
            nQueue.emplace(remainder, nindex);
        } else if (remainder > mst_pmargin) {
            pQueue.emplace(remainder, pindex);
        }

        res.emplace_back(nNameLookup[nindex], pNameLookup[pindex], payment);
    }

    /////// Debug output
    std::cout << "\n\nGREEDY SOLN=";
    int i = 0;
    for (const auto& x : res) {
        std::cout << '{' << std::get<0>(x) << ',' << std::get<1>(x) << ',' << std::get<2>(x) << "},";
        i++;
    }
    ////////////////////

    return res;
}

// TODO: Undo this: Assumes debts can be settled
// TODO: Consider long term resizing and switching back and forth between large solutions and small ones
std::vector<std::tuple<std::string, std::string, double> >
    AlgoCore::SolveOptimal(std::unordered_map<std::string, double> debts) const
{
    // Start with the greedy solution
    std::vector<std::tuple<std::string, std::string, double> > res = SolveGreedy(debts);

    // Now break it into components to make it faster to solve

    // Sets of positive and negative costs in the debts object, prime the first ones
    if (mst_psets.empty()) {
        mst_psets.emplace_back();
    } else {
        mst_psets[0].clear();
    }

    if (mst_nsets.empty()) {
        mst_nsets.emplace_back();
    } else {
        mst_nsets[0].clear();
    }

    // Indexing system, map index of vector to person's name
    std::vector<std::string> plookup;
    std::vector<std::string> nlookup;

    // Reverse lookup is only needed for converting settle minmax soln to an indexed one.
    std::unordered_map<std::string, std::size_t> prevlookup;
    std::unordered_map<std::string, std::size_t> nrevlookup;

    for (const auto& [name, cost] : debts) {
        if (cost < mst_nmargin) {
            nrevlookup[name] = nlookup.size();
            nlookup.push_back(name);
            mst_nsets[0].push_back(cost);
        } else if (cost > mst_pmargin) {
            prevlookup[name] = plookup.size();
            plookup.push_back(name);
            mst_psets[0].push_back(cost);
        }
    }

    // Prime the final solution using the SettleMinMax solution
    mst_finalSoln.clear();
    for (const auto& fromToCost : res) {
        mst_finalSoln.push_back({
            nrevlookup[std::get<0>(fromToCost)],
            prevlookup[std::get<1>(fromToCost)],
            std::get<2>(fromToCost)
            });
    }
    mst_solnNumT = static_cast<int>(mst_finalSoln.size());
    mst_currSoln.resize(mst_solnNumT - 1);  // TODO: are you sure -1?
    int originalsolutionNumT = mst_solnNumT;

    // Preallocate for the algorithm
    mst_psets.resize(mst_solnNumT);
    mst_nsets.resize(mst_solnNumT);

    SolveOptimalRecurse(mst_psets[0].size(), mst_nsets[0].size(), 0);

    // If this didn't change, then the final solution is the result from SettleMinMax and we can just return it.
    if (originalsolutionNumT != mst_solnNumT) {
        res.clear();
        // TODO: Shouldn't need double check
        for (int i = 0; i < mst_solnNumT && i < static_cast<int>(mst_finalSoln.size()); ++i) {
            const auto& iTrans = mst_finalSoln[i];
            res.emplace_back(nlookup[iTrans.from], plookup[iTrans.to], iTrans.cost);
        }
    }

    return res;
}

// return value true means you need to recheck your base case
// comment should explain how numT acts as an index
bool AlgoCore::SolveOptimalRecurse(std::size_t pcount, std::size_t ncount, int numT) const
{
    const auto& currPset = mst_psets[numT];
    const auto& currNset = mst_nsets[numT];

    //////////// Debug output
    std::cout << std::endl;
    for (int i = 0; i < numT; ++i) { std::cout << '\t'; }
    std::cout << "pset{";
    bool needsComma = false;
    for (const auto& x : currPset) {
        if (needsComma) { std::cout << ","; }
        else { needsComma = true; }
        std::cout << x;
    }
    std::cout << "}, nset{";
    needsComma = false;
    for (const auto& x : currNset) {
        if (needsComma) { std::cout << ","; }
        else { needsComma = true; }
        std::cout << x;
    }
    std::cout << "}, numT=" << numT << std::flush;
    ///////////////////////////

    if (pcount == 0 && ncount == 0) {
        // Base case: Solution found
        mst_solnNumT = numT;
        mst_finalSoln = mst_currSoln;

        /////// Debug output
        std::cout << " SOLN=";
        int i = 0;
        for (const auto& x : mst_finalSoln) {
            std::cout << (i >= mst_solnNumT ? "ignore" : "");
            std::cout << '{' << x.from << ',' << x.to << ',' << x.cost << "},";
            i++;
        }
        ////////////////////

        return true;
    }

    if (static_cast<int>(std::max(pcount, ncount)) + numT >= mst_solnNumT) {
        // Base Case: Can't possibly improve upon best solution
        std::cout << " rF(cant improve base)";
        return false;
    }

    for (std::size_t p = 0; p < currPset.size(); ++p) {
        if (currPset[p] <= mst_pmargin) {
            continue;
        }

        for (std::size_t n = 0; n < currNset.size(); ++n) {
            if (currNset[n] >= mst_nmargin) {
                continue;
            }

            const double remainder = currPset[p] + currNset[n];
            const double payment = std::min(currPset[p], -1 * currNset[n]);

            // Add Solution
            mst_currSoln[numT].from = n;
            mst_currSoln[numT].to = p;
            mst_currSoln[numT].cost = payment;

            // Prime next recurse
            const int nextNumT = numT + 1;
            auto& nextNset = mst_nsets[nextNumT];
            auto& nextPset = mst_psets[nextNumT];
            nextNset = currNset;
            nextPset = currPset;
            int nextNcount = ncount;
            int nextPcount = pcount;

            if (remainder < 0) {
                nextNset[n] = remainder;
                nextPset[p] = 0;
                nextPcount--;
                if (remainder >= mst_nmargin) {
                    nextNcount--;       // TODO: setting this unnecessarily?
                }
            } else {
                nextNset[n] = 0;
                nextPset[p] = remainder;
                nextNcount--;
                if (remainder <= mst_pmargin) {
                    nextPcount--;
                }
            }

            // Recursive case.  If we find a solution recheck for the base case where
            // we can't improve upon solution because mst_solnNumT will be different.
            if (SolveOptimalRecurse(
                    nextPcount,
                    nextNcount,
                    nextNumT)
                && static_cast<int>(std::max(pcount, ncount)) + numT >= mst_solnNumT)
            {
                    std::cout << " rF(recheck base)" << std::flush;
                    return true;
            }
        }
    }

    // No solutions in this branch of the tree
    std::cout << " rF(end loop)";
    return false;
}
