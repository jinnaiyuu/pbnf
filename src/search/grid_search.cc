/* -*- mode:linux -*- */
/**
 * \file grid_search.cc
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-10-08
 */

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <vector>

#include "get_search.h"
#include "state.h"
#include "search.h"
#include "h_zero.h"
#include "grid/grid_world.h"
#include "util/timer.h"

using namespace std;

int main(int argc, char *argv[])
{
	vector<const State *> *path;
	Search *search = get_search(argc, argv);
	GridWorld g(cin);
	Timer timer;

	if (ratio == 0)
		ratio = 1.0;
	int denom = g.get_height() / ((int) ratio * threads);
	unsigned int nblocks = g.get_height() / denom;
	GridWorld::RowModProject project(&g, nblocks);
	g.set_projection(&project);

//	HZero hzero(&g);
//	g.set_heuristic(&hzero);
	GridWorld::ManhattanDist manhattan(&g);
	g.set_heuristic(&manhattan);

	timer.start();
	path = search->search(g.initial_state());
	timer.stop();

	/* Print the graph to the terminal */
//	g.print(cout, path);

	if (path) {
		cout << "cost: " << (int) path->at(0)->get_g() << endl;
		cout << "length: " << path->size() << endl;

		// Make sure that the heuristic was actually admissible!
		for (unsigned int i = path->size() - 1; i >= 0; i -= 1) {
			const State *s = path->at(i);
			float togo = path->at(0)->get_g() - s->get_g();
			assert(s->get_h() <= togo);
			if (i > 0)
				assert(s->get_h() > 0);
			if (i == 0)
				break;
		}

		for (unsigned int i = 0; i < path->size(); i += 1)
			delete path->at(i);
		delete path;
	} else {
		cout << "No Solution" << endl;
	}
	cout << "wall_time: " << timer.get_wall_time() << endl;
	cout << "CPU_time: " << timer.get_processor_time() << endl;
	cout << "generated: " << search->get_generated() << endl;
	cout << "expanded: " << search->get_expanded() << endl;

	delete search;

#if ENABLE_IMAGES
	g.export_eps("output.eps");
#endif	// ENABLE_IMAGES

	return EXIT_SUCCESS;
}
