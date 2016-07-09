/**
 * brkga-tsp.cpp
 *
 * Driver class with a simple example of how to instantiate and use the BRKGA API to find solutions
 * to the symmetric traveling salesman problem (TSP) on TSPLIB instances.
 *
 */

#include <iostream>
#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include "BrkgaConfig.h"

/*
OBSERVATIONS.-
1) I will pass every time the same parameter, but 
I think it is a reference we dont have problems 
*/

//Read the files of a folder
int getdir (std::string dir, std::vector<std::string>& files)
{

   	DIR *dp;
   	struct dirent *dirp;
   	if((dp  = opendir(dir.c_str())) == NULL) {
       	std::cout << "Error(" << errno << ") opening " << dir << std::endl;
       	return errno;
   	}

   	while ((dirp = readdir(dp)) != NULL) {
       	std::string file = std::string(dirp->d_name);
    	if(file != "." && file != "..") files.push_back(file);
   	}
   	closedir(dp);
   	return 0;
}

std::vector<std::string> fileToConfigFile(const std::string& inputFile){

	std::ifstream fin(inputFile.c_str());
	//if(! fin) { throw Error("brkga-tsp: Cannot open the config file."); }

	std::vector<std::string> inputStrings;

	std::string line;

	while(getline(fin, line)){
		inputStrings.push_back(line);
	}

	return inputStrings;
}

int main(int argc, char* argv[]){
	if(argc < 3){
		std::cerr << "usage: <TSPLIB-files> and  insert a seed" << std::endl;
		return -1;
	}

	std::cout << "Welcome to the BRKGA API sample driver." << std::endl;
	std::cout << "Finding a (heuristic) minimizer for the TSP" << std::endl; 

	const std::string instanceFile = std::string(argv[1]);
  int seed = atoi(argv[2]);
  std::cout<<"el seed es = "<<seed<<std::endl;
	std::vector<std::string> configFile = fileToConfigFile(instanceFile);	

	std::string dir = std::string("./");
	//Name of the folder where are the inputs
  	std::string folderInput = configFile[1].substr(16);
    std::string folderOutput = configFile[2].substr(17);
  	std::cout << "Name of the Inputs Folder: " << folderInput << std::endl;
  	dir += folderInput;

  	std::vector<std::string> files = std::vector<std::string>();
  	//get all the files of the directory dir
  	getdir(dir, files);

  	sort(files.begin() , files.end() );

  	for(int i = 0 ; i < (int)files.size() ; i++){
  		configFile[1] = folderInput + "/" + files[i];
  		brkgaConfig brkgaTspI(configFile, seed);
  		std::cout << "for " << folderInput + "/" + files[i] << " the ouput is: "  << std::endl;
      std::pair<std::vector<double>, double> answer= brkgaTspI.brkgaEvolution();
      std::vector<double> best_chromosome = answer.first;
      long long best_Fitness = answer.second;
      std::string fileout = folderOutput + '/';
      fileout = fileout + files[i];
      fileout = fileout + ".out";
      //STRING TO CHAR
      std::string str = fileout;
      char * writable = new char[str.size() + 1];
      std::copy(str.begin(), str.end(), writable);
      writable[str.size()] = '\0';
      //PRINT OUT THE OUTPUT
      std::ofstream myfile;
      myfile.open(writable);
      //freopen("output.txt", "w", stdout);
      for(int j = 0 ; j < (int)best_chromosome.size() ; j++){
          myfile << best_chromosome[j];
          myfile << " ";

          //std::cout<<best_chromosome[j]<<" ";
      }
      myfile << "\n";
      myfile << best_Fitness;
      myfile << "\n";
      //std::cout<<std::endl;
      //std::cout << best_Fitness << std::endl;
      //fclose(stdout);
      myfile.close();
  	}
}

