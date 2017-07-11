
# BRKGA

The objective of this project was to port the BRKGA (see Gonçalves and Resende [7]) to execute in several parallel machines in a cloud environment. The BRKGA is a general search metaheuristic for finding optimal or near-optimal solutions to hard combinatorial optimization problems, and a C++ implementation of this metaheuristics was presented by Toso and Resende [1] which is avaiable in [2].

Our work consisted in adapting the BRKGA implementation of [1] to minimize the development effort of a user interested in solving hard combinatorial optimization problems in large scale. We modified the BRKGA implementation such that a user needs only to write 2 classes, which is dependent on the problem being solved:

* Decoder class.- This class must has a function decode, that given a BRKGA vector of values between 0s and 1s, maps this vector to a solution of the problem being considered, and returns the value of this solution (the BRKGA default is to maximize the objective function).
* Instance class.- This class has the methods to read an instance of the problem being solved, and perhaps some auxiliary methods needed  to process the instance (for example, that returns the cost between a pair of vertices in a TSP instance). This auxiliary methods are defined and used by the user, probably while implementing the Decoder class.

The main class is named BrkgaConfig that creates a decoder from the TSPDecoder class and an object named instance from the TSPInstance class, which are our examples of implementation of the Decoder and Instance classes mentioned above. The BrkgaConfig class controls all the execution of the BRKGA search method. Several parameters are used by the BRKGA and we provided a config file where these parameters can be set. 
## Config File
The config file must have the following parameters:
```
numbergenerations=30          
input directory=TSPInput
output directory=Output
flagconfiginstances=yes
p=500
pe=0.10
pm=0.10
rhoe=0.70
K=3
MAXT=2
```

The parameters of this config file has the following meaning:

* numbergenerations: the number of generations that the brkga will execute.
* input directory: the directory containing the input instances of the problem.
* output directory: the directory where the the program generates the outputs. IMPORTANT.- if you are running the BRKGA in the Azure cloud, then you must not change this name, because the script that executes the BRKGA in the Azure expects to read the solutions from this directory).
* flagconfiginstances: if you want to assign specific values to the BRKGA search method you need to put the value yes, and otherwise if you want to use default values you need to put the value no. The specific values to the BRKGA are the ones below.
* p: the size of the population.
* pe: the fraction of the population to be included in the elite-set.
* pm: the fraction of population to be replaced by mutants.
* rhoe: the probability that an offspring inherit an allele from an elite parent.
* K: the number of independent populations.
* MAXT: the number of threads for parallel decoding. 

# Azure Script

We created a Python script to run the BRKGA over a same set of instances, but in differents virtual machines created on the Microsoft Cloud Azure. Each virtual machine executes a BRKGA implementation with a different seed, such that hopefully different parts of the search space are explored. After the execution in all virtual machines, the best results are collected by the Python script and copied to a local directory.

In order to use this script you need to install the Python Azure SDK (see more information here [4]).
The script is responsible for the execution of the following steps: 

1.- Create a number virtual machines (this number is specified in a config file, see below).
2.- Copy the BRKGA containing the user implementation of the Decoder and Instance classes to the virtual machines in the cloud.
3.- Execute the BRKGA in each virtual machine in the cloud.
4.- Copy back the outputs of the execution of the BRKGA on each machine to the local host. The outputs are copied to a local directory named Outputs in the current directory.

To run the script one needs to rewrite the config file azure_config.json with the correct parameters:
```
{
"subscription_id" : "fc7b8dd2-ae77-4a62-a508-07f641fa720b",
"certificate_path" : "/home/kent/mycert.pem",
"number_machines" : "3",
"brkga_path" : "/home/kent/Documents/microsoftproject/brkgaAPI-master/examples/brkga-tsp.tar.xz"
}
```

Now we describe how to rewrite the config file azure_config.json. First you need an account in the Microsoft Azure. Then you can set the config file parameters as follows:

* subscription_id: this can be retrieve as explained in [3].
* certificate_path: we have to generate 2 files, “mycert.pem” and “mycert.cer”. These * certificate files can be generated as explained in [5]. These certificate files are used to do an automatic login process in Azure. Finally we have to upload the file “mycert.cer” in to the Azure (see [6]).
* number_machines: the number of machines that are used to run the BRKGA.
* brkga_path: the path where is your brkga implementation compressed in the form of .tar.xz file.

In order to use the azure script we need to install the python sdk for azure (see [8]). We also need to install the necessary libraries used as import from the python script: json, paramiko, scp, shutil, os and random.

# REFERENCES

[1] http://www.optimization-online.org/DB_FILE/2011/10/3200.pdf

[2] https://github.com/rfrancotoso/brkgaAPI

[3] https://blogs.msdn.microsoft.com/mschray/2015/05/13/getting-your-azure-guid-subscription-id/

[4] https://azure.microsoft.com/en-us/documentation/articles/python-how-to-install/

[5] https://azure.microsoft.com/en-us/documentation/articles/cloud-services-python-how-to-use-service-management/

[6] https://drive.google.com/file/d/0B3IDGkTJuIvFbVl4aHotcWhtZFE/view?usp=sharing

[7] JF Gonçalves, MGC Resende, Journal of Heuristics 17 (5), 487-525
[8] https://azure.microsoft.com/en-us/documentation/articles/python-how-to-install/
