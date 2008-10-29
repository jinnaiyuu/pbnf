/* -*- mode:linux -*- */
/**
 * \file nblock.cc
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-10-21
 */

#include <assert.h>

#include <vector>

#include "nblock.h"
#include "../open_list.h"
#include "../queue_open_list.h"
#include "../closed_list.h"

using namespace std;
using namespace PBNF;

/**
 * Create a new NBlock structure.
 */
NBlock::NBlock(unsigned int id, vector<unsigned int> preds,
	       vector<unsigned int> succs)
	: id(id),
	  sigma(0),
	  preds(preds),
	  succs(succs) {}


/**
 * Destroy an NBlock and all of its states.
 */
NBlock::~NBlock(void)
{
	open.delete_all_states();
	closed.delete_all_states();
}

bool NBlock::operator<(NBlock *b)
{
	float fa = open.peek()->get_f();
	float fb = b->open.peek()->get_f();

	if (fa == fb)
		return open.peek()->get_g() < b->open.peek()->get_g();

	return fa > fb;
}


/**
 * Print an NBlock to the given stream.
 */
void NBlock::print(ostream &o) const
{
	vector<unsigned int>::const_iterator iter;

	o << "nblock " << id << endl;
	o << "\tsigma: " << sigma << endl;
	o << "\tpreds: ";
	for (iter = preds.begin(); iter != preds.end(); iter++)
		o << *iter << " ";
	o << endl;

	o << "\tsuccs: ";
	for (iter = succs.begin(); iter != succs.end(); iter++)
		o << *iter << " ";
	o << endl;

}
