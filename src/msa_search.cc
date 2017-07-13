// © 2014 the PBNF Authors under the MIT license. See AUTHORS for the list of authors.

/**
 * \file msa_search.cc
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
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "get_search.h"
#include "state.h"
#include "search.h"
#include "h_zero.h"
#include "msa/msa_world.h"
#include "util/timer.h"
#include "util/timeout.h"
#include "util/fixed_point.h"

using namespace std;

int main(int argc, char *argv[])
{
	unsigned int timelimit = 5000;	// seconds
	vector<State *> *path;
	Search *search = get_search(argc, argv);

	std::ifstream pam("PAM250");
	MSAWorld g(pam, cin);


	Timer timer;

//	unsigned int root = (unsigned int) sqrt((double)nblocks);
//	MSAWorld::RowModProject project(&g, nblocks);
	MSAWorld::RowModProject project(&g, nblocks);

	if (argc > 2) {
		g.set_abstraction(stoi(argv[2]));
	} else {
		g.set_abstraction(1);
	}

	g.set_projection(&project);

	MSAWorld::Pairwise p(&g);
	p.set_weight(1);
	g.set_heuristic(&p);

	printf("start\n");

#if defined(NDEBUG)
	timeout(timelimit);
#endif	// NDEBUG

	timer.start();
	path = search->search(&timer, g.initial_state());
	timer.stop();

#if defined(NDEBUG)
	timeout_stop();
#endif	// NDEBUG

	search->output_stats();

	/* Print the graph to the terminal */
//	g.print(cout, path);

	if (path) {
		printf("cost: %f\n", (double) path->at(0)->get_g() / fp_one);
		cout << "length: " << path->size() << endl;

		// Make sure that the heuristic was actually admissible!
		for (unsigned int i = path->size() - 1; i >= 0; i -= 1) {
#if !defined(NDEBUG)
			State *s = path->at(i);
			fp_type togo = path->at(0)->get_g() - s->get_g();
			assert(s->get_h() <= togo);
#endif
			if (i > 0)
				assert(s->get_h() > 0);
			if (i == 0)
				break;
		}

		for (unsigned int i = 0; i < path->size(); i += 1)
			delete path->at(i);
		delete path;
	} else {
		cout << "# No Solution" << endl;
	}
	cout << "time-limit: " << timelimit << endl;
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