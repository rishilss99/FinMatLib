#pragma once

#include "stdafx.h"

/**
 *   A pipeline is a simple comunication channel between two threads,
 *   one writes to the pipeline while another reads
 */
template <typename T>
class Pipeline {
public:
	Pipeline();
	void write( T value );
	T read();
private:
	bool empty;
	T value;
	/*  Mutex to coordinate threads */
	std::mutex mtx;
	/* Condition variable to signal between threads */
	std::condition_variable cv;	
};


void testPipeline();