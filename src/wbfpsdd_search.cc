/**
 * \file wfbfpsdd_search.cc
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-11-20
 */


#include <pthread.h>

#include <limits>
#include <iostream>
#include <vector>

#include "wbfpsdd_search.h"
#include "wbfpsdd/nblock_graph.h"
#include "wbfpsdd/nblock.h"
#include "util/thread.h"
#include "util/timer.h"
#include "projection.h"
#include "open_list.h"
#include "closed_list.h"

using namespace std;
using namespace WBFPSDD;

/**
 * Create a new PSDD Search thread.
 */
WBFPSDDSearch::WBFPSDDThread::WBFPSDDThread(NBlockGraph *graph,
					    WBFPSDDSearch *search)
	: graph(graph), search(search) {}


WBFPSDDSearch::WBFPSDDThread::~WBFPSDDThread() {}


/**
 * The thread work method for a PSDD search.
 */
void WBFPSDDSearch::WBFPSDDThread::run(void)
{
	vector<State *> *path;
	NBlock *n = NULL;

	do {
		n = graph->next_nblock(n);

		if ((search->weight * graph->get_f_min()) > search->bound.read())
			break;

		if (n) {
			exp_this_block = 0;
			path = search_nblock(n);

			if (path)
				search->set_path(path);
			ave_exp_per_nblock.add_val(exp_this_block);
		}

	} while(!search->done && n);

	search->done = true;
	graph->set_done();
}

fp_type WBFPSDDSearch::WBFPSDDThread::get_ave_exp_per_nblock(void)
{
	return ave_exp_per_nblock.read();
}

/**
 * Search a single NBlock.
 * \param n The NBlock.
 * \return NULL, or a path to a goal.
 */
vector<State *> *WBFPSDDSearch::WBFPSDDThread::search_nblock(NBlock *n)
{
	vector<State *> *path = NULL;

	while (!search->done && !n->open_fp.empty()) {
		if (exp_this_block > search->min_expansions
		    && n->open_fp.get_best_val() > graph->get_layer_value())
			break;

		if (search->weight * n->open_f.get_best_val() >= search->bound.read()) {
			n->open_f.prune();
			n->open_fp.prune();
			break;
		}

		State *s = n->open_fp.take();
		n->open_f.remove(s);

		if (s->get_f_prime() >= search->bound.read()) {
			n->open_fp.prune();
			n->open_f.prune();
			break;
		}

		if (search->weight * s->get_f() >= search->bound.read())
			continue;

		if (s->is_goal()) {
			path = s->get_path();
			break;
		}

		exp_this_block += 1;

		vector<State *> *children = search->expand(s);
		vector<State *>::iterator iter;
		for (iter = children->begin();
		     iter != children->end();
		     iter++) {
			unsigned int block = search->project->project(*iter);
			NBlock *b = graph->get_nblock(block);
			PQOpenList<State::PQOpsF> *next_open_f = &b->open_f;
			PQOpenList<State::PQOpsFPrime> *next_open_fp = &b->open_fp;
			ClosedList *next_closed = &graph->get_nblock(block)->closed;
			if (search->weight * (*iter)->get_f() >= search->bound.read()) {
				delete *iter;
				continue;
			}
			State *dup = next_closed->lookup(*iter);
			if (dup) {
				if (dup->get_g() > (*iter)->get_g()) {
					dup->update((*iter)->get_parent(),
						    (*iter)->get_g());
					if (dup->is_open()) {
						next_open_f->see_update(dup);
						next_open_fp->see_update(dup);
					} else {
						next_open_f->add(dup);
						next_open_fp->add(dup);
					}
				}
				delete *iter;
			} else {
				next_closed->add(*iter);
				if ((*iter)->is_goal()) {
					path = (*iter)->get_path();
					delete children;
					return path;
				}
				next_open_f->add(*iter);
				next_open_fp->add(*iter);
			}
		}
		delete children;

	}

	return path;
}


/**********************************************************************/


/**
 * Create a new Parallel Structured Duplicate Detection search.
 */
WBFPSDDSearch::WBFPSDDSearch(unsigned int n_threads, fp_type mult, unsigned int min_expansions)
	: bound(fp_infinity),
	  n_threads(n_threads),
	  project(NULL),
	  path(NULL),
	  graph(NULL),
	  min_expansions(min_expansions),
	  multiplier(mult)
{
	pthread_mutex_init(&path_mutex, NULL);
}

/**
 * Create a new Parallel Structured Duplicate Detection search with a
 * given bound.
 */
WBFPSDDSearch::WBFPSDDSearch(unsigned int n_threads, fp_type mult, unsigned int min_expansions, fp_type bound)
	: bound(bound),
	  n_threads(n_threads),
	  project(NULL),
	  path(NULL),
	  graph(NULL),
	  min_expansions(min_expansions),
	  multiplier(mult),
	  done(false)
{
	pthread_mutex_init(&path_mutex, NULL);
}


/**
 * Destructor.
 */
WBFPSDDSearch::~WBFPSDDSearch(void)
{
	if (graph)
		delete graph;
}


/**
 * Set the path to the goal.
 */
void WBFPSDDSearch::set_path(vector<State *> *p)
{
	pthread_mutex_lock(&path_mutex);
	assert(!p || p->at(0)->get_g() == p->at(0)->get_f());
	if (p && bound.read() >= p->at(0)->get_g()) {
		this->path = p;
		bound.set(p->at(0)->get_g());

		if ((weight * graph->get_f_min()) > p->at(0)->get_g())
			done = true;
	}
	pthread_mutex_unlock(&path_mutex);
}


/**
 * Test if there has been a path to the goal found yet.
 */
bool WBFPSDDSearch::path_found(void) const
{
	return path != NULL;
}

/**
 * Perform the search.
 */
vector<State *> *WBFPSDDSearch::search(State *initial)
{
	project = initial->get_domain()->get_projection();

	vector<WBFPSDDThread *> threads;
	vector<WBFPSDDThread *>::iterator iter;
	fp_type sum = 0.0;
	unsigned int num = 0;
	Timer t;

	t.start();
	graph = new NBlockGraph(project,
				n_threads,
				multiplier,
				initial);
	t.stop();

	weight = initial->get_domain()->get_heuristic()->get_weight();

	for (unsigned int i = 0; i < n_threads; i += 1) {
		WBFPSDDThread *t = new WBFPSDDThread(graph, this);
		threads.push_back(t);
		t->start();
	}

	for (iter = threads.begin(); iter != threads.end(); iter++) {
		(*iter)->join();

		fp_type ave = (*iter)->get_ave_exp_per_nblock();
		if (ave != 0.0) {
			sum += ave;
			num += 1;
		}

		delete *iter;
	}

	if (num == 0)
		cout << "expansions-per-nblock: -1" << endl;
	else
		cout << "expansions-per-nblock: " << sum / num << endl;

	cout << "nblock-graph-creation-time: " << t.get_wall_time() << endl;

	cout << "total-nblocks: " << project->get_num_nblocks() << endl;
	cout << "created-nblocks: " << graph->get_ncreated_nblocks() << endl;

	return path;
}

/**
 * Set the bound.
 */
void WBFPSDDSearch::set_bound(fp_type b)
{
	this->bound.set(b);
}