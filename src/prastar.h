/**
 * \file prastar.h
 *
 *
 *
 * \author Seth Lemons
 * \date 2008-11-19
 */

#if !defined(_PRASTAR_H_)
#define _PRASTAR_H_

#include <vector>

#include "state.h"
#include "search.h"
#include "pbnf/nblock_graph.h"
#include "pbnf/nblock.h"
#include "util/msg_buffer.h"
#include "util/atomic_int.h"
#include "util/thread.h"
#include "synch_pq_olist.h"
#include "synch_closed_list.h"
#include "util/completion_counter.h"


using namespace std;

class PRAStar : public Search {
public:
        PRAStar(unsigned int n_threads);

        virtual ~PRAStar(void);

        virtual vector<State *> *search(Timer *t, State *init);
        void set_done();
        bool is_done();
        void set_path(vector<State *> *path);
        bool has_path();

private:
        class PRAStarThread : public Thread {
        public:
                PRAStarThread(PRAStar *p, vector<PRAStarThread *> *threads, CompletionCounter* cc);
                virtual ~PRAStarThread(void);
                virtual void run(void);

		/**
		 * Gets a pointer to the message queue.
		 */
	        vector<State*> *get_queue(void);

		/**
		 * Gets the lock on the message queue.
		 */
		pthread_mutex_t *get_mutex(void);

		/**
		 * Should be called when the message queue has had
		 * things added to it.
		 */
		void post_send(void);

		/**
		 * Deprecated... if you call this, you will regret it.
		 */
                void add(State* c, bool self_add);

                State *take(void);

        private:
		/* Flushes the send queues. */
		void flush_sends(bool force);


		/* flushes the queue into the open list. */
                void flush_queue(void);

		/* sends the state to the appropriate thread (possibly
		 * this thread). */
		void send_state(State *c, bool force);

                PRAStar *p;

		/* List of the other threads */
                vector<PRAStarThread *> *threads;

		/* The incoming message queue. */
		vector<State *> q;
                pthread_mutex_t mutex;

		/* The outgoing message queues (allocated lazily). */
		vector<MsgBuffer<State*>* > out_qs;

		/* Marks whether or not this thread has posted completion */
                bool completed;

		/* A pointer to the completion counter. */
                CompletionCounter *cc;

                friend class PRAStar;

		/* Search queues */
                PQOpenList<State::PQOpsFPrime> open;
                ClosedList closed;
		bool q_empty;
        };

        bool done;
        pthread_cond_t cond;
        pthread_mutex_t mutex;
        const unsigned int n_threads;
	AtomicInt bound;
	const Projection *project;
        vector<State *> *path;
	vector<PRAStarThread *> threads;
	vector<PRAStarThread *>::iterator iter;
};

#endif	/* !_PRASTAR_H_ */
