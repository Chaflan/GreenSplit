#include "algocore.h"
#include <queue>

// TODO: Clean up camelCase.  Lots of nextNcount instead of nextNCount
//          maybe make them all start with p or n
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
        if (cost < nMargin) {
            nQueue.emplace(cost, nNameLookup.size());
            nNameLookup.push_back(name);
        } else if (cost > pMargin) {
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

        if (remainder < nMargin) {
            nQueue.emplace(remainder, nindex);
        } else if (remainder > pMargin) {
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
    std::cout << std::flush;
    ////////////////////

    return res;
}

// TODO: Undo this: Assumes debts can be settled and that it is non-empty
// TODO: Consider long term resizing and switching back and forth between large solutions and small ones
std::vector<std::tuple<std::string, std::string, double> >
    AlgoCore::SolveOptimal(std::unordered_map<std::string, double> debts)
{

    // TODO: Is it possible to remove the indexing system and just use string moves?
    // We don't have to worry about size anymore.

    // TODO: Is it at least possible to skip using itransactions at all?

    // Sets of positive and negative costs in the debts object
    std::vector<double> firstPSet;
    std::vector<double> firstNSet;

    // Indexing system, map index of vector to person's name
    std::vector<std::string> plookup;
    std::vector<std::string> nlookup;

    // Reverse lookup is only needed for converting greedy soln to an indexed one.
    std::unordered_map<std::string, std::size_t> prevlookup;
    std::unordered_map<std::string, std::size_t> nrevlookup;

    for (const auto& [name, cost] : debts) {
        if (cost < nMargin) {
            nrevlookup[name] = nlookup.size();
            nlookup.push_back(name);
            firstNSet.push_back(cost);
        } else if (cost > pMargin) {
            prevlookup[name] = plookup.size();
            plookup.push_back(name);
            firstPSet.push_back(cost);
        }
    }

    struct ITransaction {
        std::size_t from;
        std::size_t to;
        double cost;
    };

    // Prime the final solution using the greedy solution.  The optimal tree solution
    // will attempt to improve upon this, but won't be able to in most cases.
    std::vector<std::tuple<std::string, std::string, double> > res = SolveGreedy(debts);
    int finalSolnNumTrans = static_cast<int>(res.size());
    const int originalSolnNumTrans = finalSolnNumTrans;

    std::vector<ITransaction> finalSoln(finalSolnNumTrans - 1);
    std::vector<ITransaction> currSoln(finalSolnNumTrans - 1);  // TODO: are you sure -1?

    for (int i = 0; i < finalSolnNumTrans; ++i) {
        finalSoln[i].from = nrevlookup[std::get<0>(res[i])];
        finalSoln[i].to = nrevlookup[std::get<1>(res[i])];
        finalSoln[i].cost = std::get<2>(res[i]);
    }

    struct StackFrame {
        std::size_t pix;
        std::size_t nix;
        std::vector<double> pset;
        std::vector<double> nset;
        std::size_t pcount;
        std::size_t ncount;
    };

    // TODO: I think its actually solnNumT - 1
    // TODO: fill a first pset and nset, and move them (?)
    std::vector<StackFrame> st(finalSolnNumTrans, { 0,
                                               0,
                                               firstPSet,
                                               firstNSet,
                                               firstPSet.size(),
                                               firstNSet.size()});


    // Note that tree depth = number of transactions
    for(int depth = 0; depth >= 0;) {
        const auto& currPset = st[depth].pset;
        const auto& currNset = st[depth].nset;
        std::size_t pcount = st[depth].pcount;
        std::size_t ncount = st[depth].ncount;

        //////////// Debug output
        std::cout << std::endl;
        for (int i = 0; i < depth; ++i) { std::cout << '\t'; }
        std::cout << "pix=" << st[depth].pix << " pset{";
        bool needsComma = false;
        for (const auto& x : currPset) {
            if (needsComma) { std::cout << ","; }
            else { needsComma = true; }
            std::cout << x;
        }
        std::cout << "}, nix=" << st[depth].nix  << " nset{";
        needsComma = false;
        for (const auto& x : currNset) {
            if (needsComma) { std::cout << ","; }
            else { needsComma = true; }
            std::cout << x;
        }
        std::cout << "}, numT=" << depth << std::flush;
        ///////////////////////////

        if (static_cast<int>(std::max(pcount, ncount)) + depth >= finalSolnNumTrans) {
            // Base Case: Can't possibly improve upon final solution
            std::cout << " ascend(cant improve)";

            --depth;
            continue;
        }

        if (pcount == 0 && ncount == 0) {
            // Base case: Solution found
            finalSolnNumTrans = depth;
            finalSoln = currSoln;

            /////// Debug output
            std::cout << " SOLN=";
            int i = 0;
            for (const auto& x : finalSoln) {
                std::cout << (i >= finalSolnNumTrans ? "ignore" : "");
                std::cout << '{' << x.from << ',' << x.to << ',' << x.cost << "},";
                i++;
            }
            std::cout << "ascend";
            ////////////////////

            --depth;
            continue;
        }

        // We use this as a sentinel to break out of the two for loops below and
        // then descend the tree.  We break on p but continue on n because descent
        // happens after we are finished with the current n index.
        bool descend = false;

        for (auto& p = st[depth].pix; p < currPset.size(); ++p) {
            if (currPset[p] <= pMargin) {
                continue;
            }

            for (auto& n = st[depth].nix; n < currNset.size() && !descend; ++n) {
                if (currNset[n] >= nMargin) {
                    continue;
                }

                const double remainder = currPset[p] + currNset[n];
                const double payment = std::min(currPset[p], -1 * currNset[n]);

                // Add Solution
                currSoln[depth].from = n;
                currSoln[depth].to = p;
                currSoln[depth].cost = payment;

                // Prime next recurse
                const int nextNumT = depth + 1;
                auto& nextNset = st[nextNumT].nset;
                auto& nextPset = st[nextNumT].pset;
                nextNset = currNset;
                nextPset = currPset;
                auto& nextNcount = st[nextNumT].ncount;
                auto& nextPcount = st[nextNumT].pcount;
                nextNcount = ncount;
                nextPcount = pcount;

                if (remainder < 0) {
                    nextNset[n] = remainder;
                    nextPset[p] = 0;
                    nextPcount--;
                    if (remainder >= nMargin) {
                        nextNcount--;
                    }
                } else {
                    nextNset[n] = 0;
                    nextPset[p] = remainder;
                    nextNcount--;
                    if (remainder <= pMargin) {
                        nextPcount--;
                    }
                }

                // TODO: I'm not convinced my indexing makes sense here, it might only work because these
                // examples are nicely ordered.
                descend = true;
            }

            if (descend) {
                break;
            }
        }

        if (descend) {
            std::cout << " descend" << std::flush;
            descend = true;
            ++depth;
        } else {
            // No solutions in this branch of the tree, ascend
            std::cout << " ascend(end loop)";
            --depth;
        }
    }

    // If this didn't change, then the final solution is the result from greedy and we can just return it.
    // Otherwise we need to conver the indexed solution to a string one using the lookups.
    if (originalSolnNumTrans != finalSolnNumTrans) {
        res.clear();
        // TODO: Shouldn't need double check
        for (int i = 0; i < finalSolnNumTrans && i < static_cast<int>(finalSoln.size()); ++i) {
            const auto& iTrans = finalSoln[i];
            res.emplace_back(nlookup[iTrans.from], plookup[iTrans.to], iTrans.cost);
        }
    }

    return res;
}
