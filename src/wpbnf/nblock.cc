/**
 * \file nblock.cc
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-10-21
 */

#include <assert.h>

#include <limits>
#include <vector>
#include <set>

#include "nblock.h"
#include "../open_list.h"
#include "../queue_open_list.h"
#include "../closed_list.h"

using namespace std;
using namespace WPBNF;

/**
 * Create a new NBlock structure.
 */
NBlock::NBlock(const Projection *project, unsigned int ident)
	: id(ident),
	  sigma(0),
	  closed(1000),
	  sigma_hot(0),
	  hot(false),
	  inuse(false)
{
	assert(id < project->get_num_nblocks());

	vector<unsigned int>::iterator i, j;
	vector<unsigned int> preds_vec = project->get_predecessors(id);
	vector<unsigned int> succs_vec = project->get_successors(id);
	// predecessors, successors and the predecessors of the successors.
	vector<unsigned int> interferes_vec = preds_vec;
	for (i = succs_vec.begin(); i != succs_vec.end(); i++) {
		interferes_vec.push_back(*i);
		vector<unsigned int> spreds = project->get_predecessors(*i);
		for (j = spreds.begin(); j != spreds.end(); j++) {
			interferes_vec.push_back(*j);
		}
	}
	for (i = preds_vec.begin(); i != preds_vec.end(); i++)
		if (*i != id)
			preds.insert(*i);
	for (i = succs_vec.begin(); i != succs_vec.end(); i++)
		if (*i != id)
			succs.insert(*i);
	for (i = interferes_vec.begin(); i != interferes_vec.end(); i++)
		if (*i != id)
			interferes.insert(*i);
}

/**
 * Destroy an NBlock and all of its states.
 */
NBlock::~NBlock(void)
{
	closed.delete_all_states();
}

/**
 * Print an NBlock to the given stream.
 */
void NBlock::print(ostream &o)
{
	fp_type best_fp;
	set<unsigned int>::const_iterator iter;

	best_fp = open_fp.empty() ? fp_infinity : open_fp.peek()->get_f_prime();

	o << "nblock " << id << endl;
	o << "\tsigma: " << sigma << endl;
	o << "\tsigma_hot: " << sigma_hot << endl;
	o << "\thot: " << hot << endl;
	o << "\tinuse: " << inuse << endl;
	o << "\topen: " << (open_fp.empty() ? "false" : "true") << endl;
	o << "\tbest f(n): " << best_fp << endl;

	o << "\tinterferes with: ";
	for (iter = interferes.begin(); iter != interferes.end(); iter++)
		o << (*iter) << " ";
	o << endl;

	o << "\tpreds: ";
	for (iter = preds.begin(); iter != preds.end(); iter++)
		o << (*iter) << " ";
	o << endl;

	o << "\tsuccs: ";
	for (iter = succs.begin(); iter != succs.end(); iter++)
		o << (*iter) << " ";
	o << endl;

}
