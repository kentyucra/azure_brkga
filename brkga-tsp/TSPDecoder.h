/*
 * TSPDecoder.h
 */

#ifndef TSPDECODER_H
#define TSPDECODER_H

#include <vector>
#include <iostream>


#include "TSPInstance.h"

class TSPDecoder {
public:
	TSPDecoder(const TSPInstance& instance);
	virtual ~TSPDecoder();

	// Decodes a chromosome into a solution to the TSP:
	double decode(const std::vector< double >& chromosome) const;

	double naiveSolver(const std::vector<double>& chromosome) const;
	double solver(const std::vector<double>& chromosome) const;

private:
	

	const TSPInstance& instance;
};

#endif
