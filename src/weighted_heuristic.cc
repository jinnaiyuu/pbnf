/**
 * \file weighted_heuristic.cc
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-10-27
 */

#include "weighted_heuristic.h"
#include "state.h"

WeightedHeuristic::WeightedHeuristic(const SearchDomain *d,
				     const Heuristic *h,
				     float w)
	: Heuristic(d), weight(w), heuristic(h) {}


WeightedHeuristic::~WeightedHeuristic(void) {}


float WeightedHeuristic::compute(State *s) const
{
	return weight * heuristic->compute(s);
}