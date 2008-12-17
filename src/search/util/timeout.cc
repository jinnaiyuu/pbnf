/* -*- mode:linux -*- */
/**
 * \file timeout.cc
 *
 *
 *
 * \author Ethan Burns
 * \date 2008-12-16
 */

#include <iostream>

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

void alarm_action(int sig)
{
	cout << "No Solution" << endl
	     << "cost: infinity" << endl
	     << "length: infinity" << endl
	     << "wall_time: infinity" << endl
	     << "CPU_time: infinity" << endl
	     << "generated: infinity" << endl
	     << "expanded: infinity" << endl;

	exit(EXIT_SUCCESS);
}

void timeout(unsigned int sec)
{
	struct sigaction sa;

	sa.sa_handler = alarm_action;
	sigaction(SIGALRM, &sa, NULL);

	alarm(sec);
}