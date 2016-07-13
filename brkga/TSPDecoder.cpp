/*
 * TSPDecoder.cpp
 */

#include <algorithm>

#include "TSPDecoder.h"

typedef std::pair< double, unsigned > ValueKeyPair;

TSPDecoder::TSPDecoder(const TSPInstance& _instance) : instance(_instance) {
}

TSPDecoder::~TSPDecoder() {
}

double TSPDecoder::decode(const std::vector< double >& chromosome) const {
	// 1) Solve the problem (i.e., create a tour out of this chromosome):
	// Avoids race conditions by making sure we have a single TSPSolver for each thread calling
	// ::decode (as long as TSPSolver does not make use of 'static' or other gimmicks):

	return solver(chromosome);
	
}

double TSPDecoder::naiveSolver(const std::vector<double>& chromosome) const{
	double distance = 0;

	for(unsigned i = 0; i < chromosome.size(); i++){
		distance += chromosome[i];
	}

	return double(distance); 
}

double TSPDecoder::solver(const std::vector<double>& chromosome) const{
	unsigned distance = 0;
	std::vector< ValueKeyPair > tour(instance.getNumNodes());
	//return double(distance1);

	for(unsigned i = 0; i < chromosome.size(); ++i) { 
		//tour.push_back( ValueKeyPair(chromosome[i], i)); 
		tour[i] = std::make_pair(chromosome[i], i); 
	}

	

	// Here we sort 'rank', which will produce a permutation of [n] stored in ValueKeyPair::second:
	std::sort(tour.begin(), tour.end());

	// 2) Compute the distance of the tour given by the permutation:
	for(unsigned i = 1; i < tour.size(); ++i) {
		// Compute distance(i-1, i) in the permutation:
		const unsigned& source = tour[i-1].second;
		const unsigned& destination = tour[i].second;

		distance += instance.getDistance(source, destination);

	}

	// Close the tour:
	const unsigned& last = tour.back().second;
	const unsigned& first = tour.front().second;
	distance += instance.getDistance(last, first);

	const double fitness = distance;
	// 3) Return:
	return double(fitness);
}