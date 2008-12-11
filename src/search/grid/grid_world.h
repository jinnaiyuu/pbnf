/* -*- mode:linux -*- */
/**
 * \file grid_world.h
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-10-08
 */

#if !defined(_GRID_WORLD_H_)
#define _GRID_WORLD_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../util/atomic_int.h"

#include "../state.h"
#include "../search_domain.h"
#include "../projection.h"

class GridState;

using namespace std;



class GridWorld : public SearchDomain {
public:
	enum cost_type { UNIT_COST, LIFE_COST };

	GridWorld(istream &s);

	virtual const State *initial_state(void);
	virtual vector<const State*> *expand(const State *s);

	virtual int get_goal_x(void) const;
	virtual int get_goal_y(void) const;
	virtual int get_width(void) const;
	virtual int get_height(void) const;
	virtual enum cost_type get_cost_type(void) const;
	virtual void print(ostream &o, const vector<const State *> *path) const;
#if defined(ENABLE_IMAGES)
	void export_eps(string file) const;
#endif	/* ENABLE_IMAGES */

	/* The Manhattan Distance heuristic. */
	class ManhattanDist : public Heuristic {
	public:
		ManhattanDist(const SearchDomain *d);
		float compute_up_over(int x, int y,
				      int gx, int gy) const;
		float compute_up_over_down(int x, int y,
					   int gx, int gy) const;
		virtual float compute(const State *s) const;
	};

	/* Projection function that uses the row number mod a value. */
	class RowModProject : public Projection {
	public:
		RowModProject(const SearchDomain *d, unsigned int mod_val);
		virtual ~RowModProject();
		virtual unsigned int project(const State *s) const ;
		virtual unsigned int get_num_nblocks(void) const ;
		virtual vector<unsigned int> get_successors(unsigned int b) const;
		virtual vector<unsigned int> get_predecessors(unsigned int b) const;
	private:
		vector<unsigned int> get_neighbors(unsigned int b) const;
		unsigned int mod_val;
		unsigned int max_row;
	};

	class CoarseProject : public Projection {
	public:
		CoarseProject(const SearchDomain *d, unsigned int cols, unsigned int rows);
		virtual ~CoarseProject();
		virtual unsigned int project(const State *s) const ;
		virtual unsigned int get_num_nblocks(void) const ;
		virtual vector<unsigned int> get_successors(unsigned int b) const;
		virtual vector<unsigned int> get_predecessors(unsigned int b) const;
	private:
		unsigned int get_id(unsigned int x, unsigned int y) const;
		vector<unsigned int> get_neighbors(unsigned int b) const;
		unsigned int cols, cols_div;
		unsigned int rows, rows_div;
	};

private:
	bool on_path(const vector<const State *> *path, int x, int y) const;
	bool is_obstacle(int x, int y) const;

	enum cost_type cost_type;

	int width, height;
	int start_x, start_y;
	int goal_x, goal_y;
	map<int, bool> obstacles;

#if defined(ENABLE_IMAGES)
	void expanded_state(const GridState *s);

	AtomicInt expanded;
	vector<AtomicInt> states;
#endif	/* ENABLE_IMAGES */

};

#endif	/* !_GRID_WORLD_H_ */
