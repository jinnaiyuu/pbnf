// © 2014 the PBNF Authors under the MIT license. See AUTHORS for the list of authors.

/**
 * \file grid_state.cc
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-10-08
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>

#include "tsp.h"
#include "tsp_state.h"

using namespace std;

/**
 * Create a new grid state.
 * \param d The search domain.
 * \param parent The parent of this state.
 * \param g The g-value of this state.
 * \param x The x-coordinate of this state.
 * \param y The y-coordinate of this state.
 */
TspState::TspState(Tsp *d, State *parent, fp_type c, fp_type g,
		vector<unsigned int>* visited_) :
		State(d, parent, c, g) {

//	printf("TspState\n");
	visited = *visited_;
	if (domain->get_heuristic()) {
//		printf("compute\n");
		this->h = domain->get_heuristic()->compute(this);
	} else {
		this->h = 0;
	}
//	printf("visited_ size = %lu\n", visited_->size());
//	printf("h,g = %lu, %lu\n", this->h, this->g);

}

/**
 * Test if this state is the goal.
 * \return True if this is a goal, false if not.
 */
bool TspState::is_goal(void) {
	const Tsp* d = static_cast<const Tsp *>(domain);
	return visited.size() == d->get_number_of_cities(); // may need to make more safe
}

/**
 * Get the hash value of this state.
 * \return A unique hash value for this state.
 */
// TODO: Ad hoc: only works for little n value.
// may need to use uint128_t. Even so, would be hard.
uint64_t TspState::hash(void) const {
	const Tsp *d = static_cast<const Tsp *>(domain);
	uint64_t hash = 0;
	unsigned int cities = d->get_number_of_cities();
	for (unsigned int i = 0; i < cities; ++i) {
		if (i < visited.size()) {
			hash = hash * cities + visited[i];
		} // do nothing otherwise
	}
	return hash;
}

/**
 * Create a copy of this state.
 * \return A copy of this state.
 */
State *TspState::clone(void) const {
	Tsp *d = static_cast<Tsp *>(domain);
	// copy visited and then pass the pointer to the new node.
	vector<unsigned int> perm(visited);
	return new TspState(d, parent, c, g, &perm);
}

/**
 * Print this state.
 * \param o The ostream to print to.
 */
void TspState::print(ostream &o) const {
	/*	o << "x=" << x << ", y=" << y << ", g=" << g << ", h=" << h << ", f="
	 << g + h << endl;*/
}

/**
 * TspState equality.
 */
bool TspState::equals(State *state) const {
	TspState *s;
	s = static_cast<TspState *>(state);
	vector<unsigned int> svisited = s->get_visited();
	return equal(svisited.begin(), svisited.end(), visited.begin());
}

vector<unsigned int> TspState::get_visited(void) const {
	return visited;
}

unsigned int TspState::zbrhash(void) {
	return hash();
}