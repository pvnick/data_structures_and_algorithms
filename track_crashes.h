#ifndef TRACK_CRASHES_H
#define TRACK_CRASHES_H

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

class _TRACK_CRASHES_H_STARTUP_CLASS
{
public:
	static void _TRACK_CRASHES_H_HANDLER(int sig) {
		void *array[10];
		size_t size;

		// get void*'s for all entries on the stack
		size = backtrace(array, 10);

		std::cerr << std::endl;
		// print out all the frames to stderr
		fprintf(stderr, "Error: signal %d:\n", sig);
		backtrace_symbols_fd(array, size, STDERR_FILENO);
		exit(1);
	}

    _TRACK_CRASHES_H_STARTUP_CLASS()
    { 
   		signal(SIGSEGV, _TRACK_CRASHES_H_HANDLER);
    }
};

static _TRACK_CRASHES_H_STARTUP_CLASS _TRACK_CRASHES_H_STARTUP_CLASS_INITIALIZATION;

#endif