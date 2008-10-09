/* -*- mode:linux -*- */
/**
 * \file closed_list.cc
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-10-09
 */

#include <map>

#include "state.h"
#include "closed_list.h"

void ClosedList::add(const State *s)
{
	m[s->hash()] = s;
}

const State *ClosedList::lookup(const State *c) const
{
	map<int, const State *>::const_iterator iter;

	iter = m.find(c->hash());

	if (iter == m.end())
		return NULL;

	return iter->second;
}

/**
 * Delete (free up the memory for) all states in the closed list.
 */
void ClosedList::delete_all_states(void)
{
	map<int, const State *>::iterator iter;

	for (iter = m.begin(); iter != m.end(); iter++)
		delete iter->second;
}
