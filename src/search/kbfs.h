/* -*- mode:linux -*- */
/**
 * \file kbfs.h
 *
 * Contains the KBFS class.
 *
 * \author Seth Lemons
 * \date 2008-10-09
 */

#if !defined(_KBFS_H_)
#define _KBFS_H_

#include "state.h"
#include "search.h"
#include "synch_pq_olist.h"
#include "synch_closed_list.h"

/**
 * A KBFS search class.
 */
class KBFS : public Search {
public:
	virtual vector<const State *> *search(const State *);
private:
	SynchPQOList open;
	SynchClosedList closed;
        friend class KBFSThread;
};

#endif	/* !_KBFS_H_ */
