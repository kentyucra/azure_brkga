# Compiler binary:
CPP= g++

# Recommended compiler flags for speed:
#	OpenMP enabled
#	full binary code optimization
#	full error and warning reports
#	no range checking within BRKGA:
# CFLAGS= -O3 -fopenmp -Wextra -Wall -Weffc++ -ansi -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith
CFLAGS= -g -Wextra -Wall -Weffc++ -ansi -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith

# Compiler flags for debugging; uncomment if needed:
#	range checking enabled in the BRKGA API
#	OpenMP disabled
#	no binary code optimization
#CFLAGS= -DRANGECHECK -Wextra -Wall -Weffc++ -ansi -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith

OBJECTS= TSPInstance.o TSPDecoder.o Population.o BrkgaConfig.o brkga-tsp.o

# Targets:
all: brkga-tsp

brkga-tsp: $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -o brkga-tsp

brkga-tsp.o:
	$(CPP) $(CFLAGS) -c brkga-tsp.cpp

TSPInstance.o:
	$(CPP) $(CFLAGS) -c TSPInstance.cpp

BrkgaConfig.o:
	$(CPP) $(CFLAGS) -c BrkgaConfig.cpp

TSPDecoder.o:
	$(CPP) $(CFLAGS) -c TSPDecoder.cpp

Population.o:
	$(CPP) $(CFLAGS) -c brkgaAPI/Population.cpp

# Remove:
clean:
	rm -f brkga-stp $(OBJECTS)
