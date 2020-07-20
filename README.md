# BranchPredictors
Implementation of branch prediction schemes

Implementation of Global dynamic branch predictors (Bimodal, Gshare, Tournament) and Static predictors (always taken and always not taken) with a C++ program "predictors.cpp".

The aim is to compare accuracy of these schemes by plotting graph between mis-prediction rate and varying parameters like predictor sizes and size of history bits register(in case of gshare). This helps us to find effectiveness of various branch predictor schemes.

A trace file of 16M conditional branch outcomes is given as input to this program. This trace file is generated from an execution of the program GCC (Gnu C Compiler) from the SPECint2000 benchmark suite.

Trace format:3086629644 T
	           3086629604 N
     	       3086629616 N
  	         3086629644 T

The program consist of 8 different functions implementing branch predictor schemes. Here gshare and bimodal functions are reused by tournament function as well.It prints all the results in a text and csv file.


Static predictors (always taken and always not taken) : Program to read in the trace and calculate the mis-prediction rate based on static prediction.

Dynamic predictors

1) Bimodal: This program simulates bimodal predictor with varying sizes of predictors. It generates data for bimodal predictors with 2^2, 2^3, 2^4, 2^5 ... 2^20 counters. These sizes correspond to predictor index size of 2 bits, 3 bits, 4 bits, 5 bits, ... 20 bits. 

2) Gshare: Ghsare is implemented with three different cases therefore three different functions are written in the program.

case1: simulation of gshare predictor with 8 history bits and varying sizes (the same sizes as in bimodal).
case2: This is to determine "how much history should a predictor use?". In this case, gshare is simulated with varying history lengths (zero bits of history through 19 bits of history) with two different predictor sizes: 2^10 and 2^16 predictor sizes.
case3: This case generates a new series of data in which the gshare history length is the same as the number of index bits.

2)Tournament: It is a hybrid predictor that tries to capture the best of both style of predicts- bimodal and gshare. The bimodal and gshare code was written in a modular way in order to re-use them for tournament implementation and thus  re-writing or replicating of code is avoided here. Tournamnet is implemented with 2 different cases.
case1:  A tournament predictor consists of three tables. The first and second tables are just normal bimodal and gshare predictors. The third table is a "chooser" table that predicts whether the bimodal or gshare predictor will be more accurate. In this case tournament is implemented with three equal sizes of 2^n-counter tables.
This turns out to be best as far as accuracy is concerned on the cost of larger storage size as we are using three tables of same sizes here, which is three times the size of bimodal and gshare.
case2: This case for giving fair advantage to gshare and bimodal schemes asin this case we use same storage as bimodal and gshare for tournament scheme. For implementating this case different tables sizes of bimodal and ghsare are taken in order to get total storage equivalent to 2^n. 

Chooser table: 2n-2 counters
Bimodal table: 2n-2 counters
Gshare table: 2n-1 counters

As 2n-2 + 2n-2 + 2n-1 is equal to 2n, this becomes a fair "same size" comparison

Another similar program "predictors_check.cpp" is written to check the correctness of the implementation. This program takes a small trace file which consist only first 200 entries of original trace file used in main program, it prints contents of pattern history table, history register and prediction result for corresponding to each branch address. This program generates three text files each for bimodal, gshare and tournament with specific history registers and predictor sizes. The correctness of the implementation can be manually checked from these output files. 




###############Instructions to run this project###################

Commands To run the project:

#To compile without running:
```
make
```
#To compile and run with the included trace file:
```
make run
```
#To clean all generated output files and executables:
```
make clean
````
#To run after compiling with any other trace file:
```
./predictors filename.txt
./predictors_check filename.txt
```


Here predictors_check.cpp file is used to debug the logics used in main file (predictors.cpp). Here, predictors_check.cpp generates some output txt file which can be checked by user to check correctness of logic.
	

###################### Files included to run this project ######################################################
* branch-trace-gcc.txt: Tarce file with 16 million entries.
* branch-trace-gcc.txt_small: Tarce file with 201 entries.Which is used by predictors_check.cpp to generate results to compare available reference output files.
* gshare4-3.output, tournament3-bimodal3-gshare4-4.output, bimodal3.output are the reference files to check the algorithm, these files are used by compare.cpp to compare these files with output files generated by predictors_check.cpp.

##
Each line in output represents the respective predictor results.






   


