/**
 * \file heuristic.h
 *
 * A heuristic function.
 *
 * \author Ethan Burns
 * \date 2008-10-08
 */

#if !defined(_HEURISTIC_H_)
#define _HEURISTIC_H_

class SearchDomain;
class State;

/**
 * An abstract heuristic function.
 */
class Heuristic {
public:
	Heuristic(const SearchDomain *d);
	virtual ~Heuristic();

	virtual float compute(State *s) const = 0;
protected:
	const SearchDomain *domain;
};

#endif	/* !_HEURISTIC_H_ */
