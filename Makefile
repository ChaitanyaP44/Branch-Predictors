compile: predictors.cpp compare.cpp predictors_check.cpp
	g++ predictors.cpp -o predictors
	g++ predictors_check.cpp -o predictors_check

	

run: compile
	./predictors branch-trace-gcc.txt
	./predictors_check branch-trace-gcc_small.txt


clean:
	rm -rf predictors
	rm -rf predictors_check
	rm -rf output.txt
	rm -rf output.csv
	rm -rf outGshareCheck.txt
	rm -rf outBimodalCheck.txt
	rm -rf outTournamentCheck.txt
