/* -*- mode:linux -*- */
/**
 * \file pbnf_search.h
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-10-29
 */

#if !defined(_PBNF_SEARCH_H_)
#define _PBNF_SEARCH_H_

#include <vector>

#include "pbnf/nblock_graph.h"
#include "pbnf/nblock.h"
#include "util/thread.h"
#include "util/atomic_float.h"
#include "util/cumulative_ave.h"
#include "projection.h"
#include "search.h"
#include "state.h"

namespace PBNF {}

using namespace std;
using namespace PBNF;

class PBNFSearch : public Search {
public:
	PBNFSearch(unsigned int n_threads, unsigned int min_expansions,
		   bool detect_livelocks);

	virtual ~PBNFSearch(void);

	virtual vector<const State *> *search(const State *initial);

private:
	void set_path(vector<const State *> *path);

	class PBNFThread : public Thread {
	public:
		PBNFThread(NBlockGraph *graph, PBNFSearch *search);
		~PBNFThread(void);
		void run(void);
		float get_ave_exp_per_nblock(void);
	private:
		vector<const State *> *search_nblock(NBlock *n);
		bool should_switch(NBlock *n);

		unsigned int expansions; /* expansions in 1 NBlock */
		NBlockGraph *graph;
		PBNFSearch *search;
		bool set_hot;
		CumulativeAverage ave_exp_per_nblock;
	};

	unsigned int n_threads;
	const Projection *project;
	pthread_mutex_t path_mutex;
	vector<const State *> *path;
	AtomicFloat bound;
	bool detect_livelocks;

	unsigned int min_expansions;
};

#endif	/* !_PBNF_SEARCH_H_ */
