#include <limits>

#include "BrkgaConfig.h"


/*

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



TSPDecoder decoder;
TSPInstance instance;
MTRand rng;				// initialize the random number generator

//BRKGA< TSPDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
BRKGA< TSPDecoder ,   MTRand> *algorithm; 
*/
brkgaConfig::brkgaConfig(std::vector<std::string>& fileInstance, int seed) : n(0), p(0), pe(0.0), pm(0.0), rhoe(0.0),
																K(0), MAXT(0), X_INTVL(0), X_NUMBER(0), MAX_GENS(0),
																instance(constructInstance(fileInstance[1])),
																decoder(constructDecoder(instance)),	
																rng(constructRng(seed))
{

	std::cout << "the numer of nodes is: " << instance.getNumNodes() << std::endl;

	n = instance.getNumNodes();
	//read number of generations
	readNG(fileInstance[0]);
	///default values
	defaultValues();	

	try{
		
		if(readFlag(fileInstance[3])){
			//std::cout << "entra cuando flag is YES" << std::endl;
			readP(fileInstance[4]);
			readPE(fileInstance[5]); 
			readPM(fileInstance[6]);
			readRHOE(fileInstance[7]);
			readK(fileInstance[8]);
			readMAXT(fileInstance[9]);
		}

	}catch(const Error& error) { 
		throw error; 
	}
	
	algorithm = new BRKGA< TSPDecoder, MTRand>(n, p, pe, pm, rhoe, decoder, rng, K, MAXT );
}

brkgaConfig::~brkgaConfig(){ }

TSPInstance brkgaConfig::constructInstance(const std::string& instanceFile){
	return TSPInstance(instanceFile);
}

TSPDecoder brkgaConfig::constructDecoder(const TSPInstance& instance1){
	return TSPDecoder(instance1);
}

MTRand brkgaConfig::constructRng(int seed){
	return MTRand(seed);
}

std::pair<std::vector<double> , double > brkgaConfig::brkgaEvolution(){
	// Print info about multi-threading:
	#ifdef _OPENMP
		std::cout << "Running for " << MAX_GENS << " generations using " << MAXT
				<< " out of " << omp_get_max_threads()
				<< " available thread units..." << std::endl;
	#endif
	#ifndef _OPENMP
		std::cout << "Running for " << MAX_GENS
				<< " generations without multi-threading..." << std::endl;
	#endif

	// BRKGA evolution configuration: restart strategy
	unsigned relevantGeneration = 1;	// last relevant generation: best updated or reset called
	const unsigned RESET_AFTER = 2000;
	std::vector< double > bestChromosome;
	double bestFitness = std::numeric_limits< double >::max();
	std::cout << "begin the training" << std::endl;
	// Run the evolution loop:
	unsigned generation = 1;		// current generation
	do {
		algorithm->evolve();	// evolve the population for one generation

		// Bookeeping: has the best solution thus far improved?
		if(algorithm->getBestFitness() < bestFitness) {
			// Save the best solution to be used after the evolution chain:
			relevantGeneration = generation;
			bestFitness = algorithm->getBestFitness();
			bestChromosome = algorithm->getBestChromosome();
			
			std::cout << "\t" << generation
					<< ") Improved best solution thus far: "
					<< bestFitness << std::endl;
		}

		//  Evolution strategy: restart
		if(generation - relevantGeneration > RESET_AFTER) {
			algorithm->reset();	// restart the algorithm with random keys
			relevantGeneration = generation;
			
			std::cout << "\t" << generation << ") Reset at generation "
					<< generation << std::endl;
		}

		// Evolution strategy: exchange top individuals among the populations
		if(generation % X_INTVL == 0 && relevantGeneration != generation) {
			algorithm->exchangeElite(X_NUMBER);
			
			std::cout << "\t" << generation
					<< ") Exchanged top individuals." << std::endl;
		}

		// Next generation?
		++generation;
	} while (generation < MAX_GENS);

	return make_pair(bestChromosome , bestFitness);
}

void brkgaConfig::brkgaPrintTop10(){

	std::cout << "Fitness of the top 10 individuals of each population:" << std::endl;
	const unsigned bound = std::min(p, unsigned(10));	// makes sure we have 10 individuals
	for(unsigned i = 0; i < K; ++i) {
		std::cout << "Population #" << i << ":" << std::endl;
		for(unsigned j = 0; j < bound; ++j) {
			std::cout << "\t" << j << ") "
					<< algorithm->getPopulation(i).getFitness(j) << std::endl;
		}
	}
}

//read parameter of the brkga
bool brkgaConfig::readFlag(const std::string& line) throw(brkgaConfig::Error) {
	//std::cout<<"entra"<<std::endl;
	if(line.find("flagconfiginstances") == std::string::npos) {
		throw Error("Cannot recognize instance: FLAG not where it should be.");
	}

	std::string flag = line.substr(line.find("=") + 1);
	return (flag == "yes")?1:0;
}

void brkgaConfig::readNG(const std::string& line)throw(brkgaConfig::Error) {
	if(line.find("numbergenerations") == std::string::npos) {
		throw Error("Cannot recognize instance: numbergenerations not where it should be.");
	}
	std::string sng = line.substr(line.find("=") + 1);
	std::istringstream sin(sng);
	sin>>MAX_GENS;
	std::cout<<"numero de generaciones es = "<<MAX_GENS<<std::endl;
}

void brkgaConfig::readP(const std::string& line) throw(brkgaConfig::Error) {
	if(line.find("p") == std::string::npos) {
		throw Error("Cannot recognize instance: P not where it should be.");
	}
	std::string sp = line.substr(line.find("=") + 1);
	std::istringstream sin(sp);
	sin>>p;
}

void brkgaConfig::readPE(const std::string& line) throw(brkgaConfig::Error) {
	if(line.find("pe") == std::string::npos) {
		throw Error("Cannot recognize instance: PE not where it should be.");
	}

	std::string spe = line.substr(line.find("=") + 1);
	std::istringstream sin(spe);
	sin>>pe;
}

void brkgaConfig::readPM(const std::string& line) throw(brkgaConfig::Error) {
	if(line.find("pm") == std::string::npos) {
		throw Error("Cannot recognize instance: PM not where it should be.");
	}

	std::string spm = line.substr(line.find("=") + 1);
	std::istringstream sin(spm);
	sin>>pm;
}

void brkgaConfig::readRHOE(const std::string& line) throw(brkgaConfig::Error) {
	if(line.find("rhoe") == std::string::npos) {
		throw Error("Cannot recognize instance: RHOE not where it should be.");
	}

	std::string srhoe = line.substr(line.find("=") + 1);
	std::istringstream sin(srhoe);
	sin>>rhoe;
}

void brkgaConfig::readK(const std::string& line) throw(brkgaConfig::Error) {
	if(line.find("K") == std::string::npos) {
		throw Error("Cannot recognize instance: K not where it should be.");
	}

	std::string sK = line.substr(line.find("=") + 1);
	std::istringstream sin(sK);
	sin>>K;
}

void brkgaConfig::readMAXT(const std::string& line) throw(brkgaConfig::Error) {
	if(line.find("MAXT") == std::string::npos) {
		throw Error("Cannot recognize instance: MAXT not where it should be.");
	}

	std::string sMAXT = line.substr(line.find("=") + 1);
	std::istringstream sin(sMAXT);
	sin>>MAXT;
}

void brkgaConfig::printAttributes(){
	std::cout<<"n = "<<n<<std::endl;
	std::cout<<"p = "<<p<<std::endl;
	std::cout<<"pe = "<<pe<<std::endl;
	std::cout<<"pm = "<<pm<<std::endl;
	std::cout<<"rhoe = "<<rhoe<<std::endl;
	std::cout<<"K = "<<K<<std::endl;
	std::cout<<"MAXT = "<<MAXT<<std::endl;
}

void brkgaConfig::defaultValues(){
	p = 256;
	pe = 0.10;
	pm = 0.10;
	rhoe = 0.70;
	K = 3;		
	MAXT = 4;	
	X_INTVL = 100;	
	X_NUMBER = 2;	
	//MAX_GENS = 10;	
}