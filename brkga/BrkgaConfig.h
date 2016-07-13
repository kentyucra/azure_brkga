#ifndef BRKGACONFIG_H
#define BRKGACONFIG_H

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>


#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"

//import classes
#include "TSPInstance.h"
#include "TSPDecoder.h"


class brkgaConfig{
public:
	typedef std::runtime_error Error;

	brkgaConfig(std::vector<std::string>& fileInstancem, int seed);/// I have to read all the values of brkga
	virtual ~brkgaConfig();

	void printAttributes();
	void defaultValues();

	TSPDecoder constructDecoder(const TSPInstance& instance1);//YOU NEED IMPLEMENT IT		
	TSPInstance constructInstance(const std::string& instanceFile);//YOU NEED IMPLEMENT IT
	MTRand constructRng(int seed);
	std::pair<std::vector<double>, double> brkgaEvolution();
	void brkgaPrintTop10();


private:
	unsigned n;				// size of chromosome
	unsigned p;				// size of population
	double pe;				// fraction of population to be the elite-set
	double pm;				// fraction of population to be replaced by mutants
	double rhoe;			// probability that offspring inherit an allele from elite parent
	unsigned K;				// number of independent populations
	unsigned MAXT;			// number of threads for parallel decoding

	unsigned X_INTVL; 		// = 100;	// exchange best individuals at every 100 generations
	unsigned X_NUMBER; 		//= 2;	// exchange top 2 best
	unsigned MAX_GENS; 		//= 1000;	// run for 1000 gens


	TSPInstance instance;
	TSPDecoder decoder;
	MTRand rng;				// initialize the random number generator

	//BRKGA< TSPDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
	BRKGA< TSPDecoder ,   MTRand> *algorithm; 


	bool readFlag(const std::string& line) throw(Error);
	void readNG(const std::string& line) throw(Error);
	void readP(const std::string& line) throw(Error);
	void readPE(const std::string& line) throw(Error);
	void readPM(const std::string& line) throw(Error);
	void readRHOE(const std::string& line) throw(Error);
	void readK(const std::string& line) throw(Error);
	void readMAXT(const std::string& line) throw(Error);

};

#endif