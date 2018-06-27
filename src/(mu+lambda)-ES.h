#ifndef MPED_MU_LAMBDA_ES_H
#define MPED_MU_LAMBDA_ES_H

#include <algorithm>
#include <numeric>
#include <vector>
#include "MatchingSchema.h"
#include "EditDistance.h"
#include "AbstractSequence.h"
#include "ES_MatchingSchema.h"


class ES_mu_p_lambda {
private:


public:
    ES_mu_p_lambda() = default;

    int evolutionStrategy(const AbstractSequence &s1, AbstractSequence &s2, const unsigned &p1, const unsigned &p2,
                          const MatchingSchema& m, EditDistance& e, const unsigned& max_generations,
                          const unsigned& mu, const unsigned& lambda) {

        //Initialize stuff for the mutator swap2-E
        const unsigned* const blocksig1 = initializeBlocksSwap2E(s1.getSigma_repr(), p1);
        const unsigned* const blocksig2 = initializeBlocksSwap2E(s2.getSigma_repr(), p2);

        ES_MatchingSchema startingMS(s1.getSigma_repr(), s2.getSigma_repr());
        ES_MatchingSchema best;
        best.costValue = std::numeric_limits<unsigned int>::max();

        //Generate mu random individuals
        ES_MatchingSchema* parents = new ES_MatchingSchema[mu + lambda];
        for (unsigned i = 0; i < mu; ++i) {
            startingMS.shuffle();
            //startingMS.costValue = e.edit_distance_matching_schema_enhanced(s1, s2, s1l, s2l, startingMS.sigma1, startingMS.sigma2, sig1l, sig2l, m);
            startingMS.costValue = e.compute_edit_enhanced(s1, s2, startingMS.sigma1, startingMS.sigma2, m);
            parents[i] = startingMS;
        }

        const unsigned last = mu - 1;

        //Select the worst parent in the pool
        unsigned worstParentCostValue = parents[0].costValue;
        for (unsigned i=1; i < mu; i++)
            if (parents[i].costValue > worstParentCostValue)
                worstParentCostValue = parents[i].costValue;

        unsigned generation = 0;
        while (generation <= max_generations) {
            unsigned childrenInPool = 0;

            //Generate lambda children. Only mutation, no recombination
            for (unsigned i = 0; i < lambda; i++) {
                //Choose random parent
                const unsigned p = rand() % mu;

                //Produce child, in the case parents=1 (like this) just clone
                ES_MatchingSchema child = parents[p];

                //mutate child
                child.swap2_enhanced(blocksig1, blocksig2);

                //const int newDistance = e.edit_distance_matching_schema_enhanced_with_diagonal(s1, s2, s1l, s2l, child.sigma1, child.sigma2, sig1l, sig2l, m, worstParentCostValue);
                const int newDistance = e.compute_edit_enhanced(s1, s2, child.sigma1, child.sigma2, m);

                //if (newDistance != -1) {
                if (newDistance < child.costValue) {
                    //The child is better than the worst parent,
                    child.costValue = newDistance;

                    //so he is added to the pool
                    parents[mu + childrenInPool] = child;
                    childrenInPool++;
                }
            }

            //sorting for selecting the best mu individuals and at the same time get the worst parent
            std::sort(parents, parents + mu + childrenInPool);
            worstParentCostValue = parents[last].costValue;

            //Make a random_shuffle for keeping high entropy
            std::random_shuffle(parents, parents+mu);

            generation++;
        }

        std::sort(parents, parents+mu+lambda);
        best = parents[0];

        delete[] blocksig1;
        delete[] blocksig2;
        delete[] parents;

        return best.costValue;
    }

    /*int evolutionStrategy(const std::vector<unsigned>& s1,
                          const std::vector<unsigned>& s2, const size_t& s1l, const size_t& s2l,
                          const std::vector<unsigned>& sig1, const std::vector<unsigned>& sig2,
                          const size_t& sig1l, const size_t& sig2l,
                          const size_t& p1, const size_t& p2, matching_schema<bool>& m, edit_distance& e,
                          const unsigned max_generations, const unsigned mu,
                          const unsigned lambda) {









        unsigned generation = 0;
        while (generation <= max_generations) {
            unsigned childrenInPool = 0;

            //Generate lambda children. Only mutation, no recombination
            for (unsigned i = 0; i < lambda; i++) {
                //Choose random parent
                const unsigned p = rand() % mu;

                //Produce child, in the case parents=1 (like this) just clone
                ES_MatchingSchema child = parents[p];

                //mutate child
                child.swap2_enhanced(blocksig1, blocksig2);

                const int newDistance = e.edit_distance_matching_schema_enhanced_with_diagonal(s1, s2, s1l, s2l, child.sigma1, child.sigma2, sig1l, sig2l, m, worstParentCostValue);

                if (newDistance != -1) {
                    //The child is better than the worst parent,
                    child.costValue = newDistance;

                    //so he is added to the pool
                    parents[mu + childrenInPool] = child;
                    childrenInPool++;
                }
            }

            //sorting for selecting the best mu individuals and at the same time get the worst parent
            std::sort(parents, parents+mu+childrenInPool);

            worstParentCostValue = parents[last].costValue;

            //Make a random_shuffle for keeping high entropy
            std::random_shuffle(parents,parents+mu);

            generation++;
        }

        std::sort(parents, parents+mu+lambda);
        best = parents[0];

        delete[] blocksig1;
        delete[] blocksig2;
        delete[] parents;

        return best.costValue;
    }*/
};

#endif