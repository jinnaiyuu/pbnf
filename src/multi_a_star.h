/**
 * \file multi_a_star.h
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-11-07
 */

#if !defined(_MULTI_A_STAR_H_)
#define _MULTI_A_STAR_H_

#include "state.h"
#include "search.h"

/**
 * This search just performs a bunch of A* searches, each in its own
 * thread in order to get a baseline for speedup.
 */
class MultiAStar : public Search {
public:
	MultiAStar(unsigned int n_threads);
	virtual ~MultiAStar(void);
	virtual vector<State *> *search(State *);

private:
	unsigned int n_threads;
};

#endif	/* !_MULTI_A_STAR_H_ */