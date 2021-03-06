/* © 2014 the PBNF Authors under the MIT license. See AUTHORS for the list of authors.*/

/**
 * \file search.h
 *
 * Abstract search classe
 *
 * \author Ethan Burns
 * \date 2008-10-09
 */

#if !defined(_SEARCH_H_)
#define _SEARCH_H_

#include <vector>

#include "util/atomic_int.h"
#include "util/timer.h"
#include "state.h"

using namespace std;

void output_search_stats_on_timeout(void);

/**
 * An abstract search class that collects some statistics.
 */
class Search {
public:
	Search(void);
	virtual ~Search() {}

	virtual vector<State *> *search(Timer *t, State *) = 0;
	virtual void output_stats(void);

	void clear_counts(void);
	unsigned long get_expanded(void) const;
  unsigned long get_generated(void) const;

  void set_delay(int delay_) {delay = delay_;};
  int get_delay(void) {return delay;};
	int get_useless(void);
	void set_overrun(int overrun_) {overrun = overrun_;};
	int get_overrun(void) {return overrun;};

protected:
      	vector<State *> *expand(State *);
       	vector<State *> *expand(State *, int thread_id);
	void set_expanded(unsigned long e);
	void set_generated(unsigned long g);
        int useless_calc(int useless);
	int overrun = 0;

private:
	AtomicInt expanded;
	AtomicInt generated;
	int delay = 0;
	int useless_counter;
};

#endif	/* !_SEARCH_H_ */
