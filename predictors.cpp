#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

enum SaturatingCounter{T = 0, t, n, N}; //2-bit saturating counter values for bimodal and gshare
enum SaturatingChooser{G = 0, g, b, B};
struct S{
	unsigned int b : 19; // Default 19 bits history lengths, in later stages it varied by bit-mask
};

void always_taken(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV);
void always_not_taken(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV);

void bimodal_predict(std::string line, int table_size, SaturatingCounter *table, long &correct, bool& bCorrect, int *isTournament);
void bimodal(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV);

void gshare_predict(std::string line, int table_size, unsigned long bitMask, S &globalHistory, SaturatingCounter *table, long &correct, bool& gCorrect, int *isTournament);
void gShare(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV);
void gshare_c2(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV);
void gshare_c3(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV);

void tournament_predict(std::string line, int table_size, SaturatingChooser *table, long &correct, bool& gCorrect, bool& bCorrect);
void tournament(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV);
void tournament_c2(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV);

void writeF(long correct, long total, float misPredictRate, std::fstream &outFile, std::fstream &outFileCSV, long *predictorSize);
bool NotTournamnet = false;


void writeF(long correct, long total, float misPredictRate, std::fstream &outFile, std::fstream &outFileCSV, long *predictorSize)
{
	if(predictorSize!=NULL) {
	std::cout << "\n\nPredictor with size " << *predictorSize << ".\n";
	outFile << "\n\nPredictor with size " << *predictorSize << ".\n";}
	std::cout << "Correct predictions = "<<correct << ", " << "Total predictions = "<< total << ", " <<"Mis-predictions Rate = "<< misPredictRate << std::endl;
	outFile << "Correct predictions = "<<correct << ", " << "Total predictions = "<< total << ", " <<"Mis-predictions Rate = "<< misPredictRate << std::endl;
	outFileCSV <<correct << "," <<total << "," <<misPredictRate << std::endl;

}

void always_taken(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV)
{
	std::cout << "\nStatic always taken\n"<< std::endl;
	outFile << "\nStatic always taken\n"<< std::endl;
	long total, correct;
	float misPredictRate;
	total = correct = 0;

	std::vector<std::string>::iterator it;
	
	std::string tmp;
	for(it = list.begin(); it != list.end(); it++)
	{
		total++;
		tmp = *it;
		if(tmp[(tmp.length()-1)] == 84) //Checking for Taken (T) ASCII value is 84 for T
			correct++;
	}
	misPredictRate = (((float)total-correct)/total) * 100;
	writeF(correct,total,misPredictRate,outFile,outFileCSV,NULL);
}

void always_not_taken(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV)
{
	std::cout << "\n\nStatic always not taken\n"<< std::endl;
	outFile << "\n\nStatic always not taken\n"<< std::endl;
	long total, correct;
	float misPredictRate;
	total = correct = 0;
	
	std::vector<std::string>::iterator it;
	
	std::string tmp;
	for(it = list.begin(); it != list.end(); it++)
	{
		total++;
		tmp = *it;
		if(tmp[(tmp.length()-1)] == 78) //ASCII value is 78 for N
			correct++;
	}
	misPredictRate = (((float)total-correct)/total) * 100;  //Calculation of Mis-Prediction Rate
        writeF(correct,total,misPredictRate,outFile,outFileCSV,NULL);
	outFile <<"*******************************************************************************\n"<< std::endl;
}

void bimodal_predict(std::string line, int table_size, SaturatingCounter *table, long &correct, bool& bCorrect, int *isTournament)
{
	long addr;
	int strLength = line.length(); //Finding length of line string
        std::string line_long = line.substr(0,(strLength-2)); //Getting address from the line string
	addr = std::stoul(line_long, nullptr, 10); //converting address string into long
	addr = addr % table_size; //calculating index value for prediction
	SaturatingCounter *h = &(table[addr]); //prediction made
	
	
	if(*h == T)
	{
		if(line[strLength-1] == 84)
			if(isTournament != NULL) bCorrect = true; //these type of conditons are generally used by tournament functions only
			else correct++;
		else
		{
			*h = t;
		}
	}else if(*h == t)
	{
		if(line[strLength-1] == 84)
		{	
			if(isTournament != NULL) bCorrect = true;
			else correct++;
			*h = T;
		}else
			*h = n;
	}else if(*h == n)
	{
		if(line[strLength-1] == 78)
		{
			if(isTournament != NULL) bCorrect = true;
			else correct++;
			*h = N;
		}else
			*h = t;
	}else
	{
		if(line[strLength-1] == 78)
			if(isTournament != NULL) bCorrect = true;
			else correct++;
		else
			*h = n;
	}
}

void bimodal(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV)
{
	std::cout << "\nBimodal with variable counter size\n"<< std::endl;
	outFile << "\nBimodal with variable counter size\n"<< std::endl;
	long correct, total;
	float misPredictRate;
	long i = 4; //Initializing with predictor size of 4
	std::vector<std::string>::iterator it;
	
	while(i <= 1048576)
	{
		SaturatingCounter table[i];
		for(int j = 0; j<i; j++)
			table[j] = N; // Initializing table each time with Strongly Not taken
		
		total = correct = 0;
		for(it = list.begin(); it != list.end(); it++)
		{
			bimodal_predict(*it, i, table, correct, NotTournamnet, NULL); 
			total++;
		}
		misPredictRate = (((float)total-correct)/total) * 100; 
		//if(i != 1048576) writeF(correct,total,misPredictRate,outFile,outFileCSV,&i);
		writeF(correct,total,misPredictRate,outFile,outFileCSV,&i);
			
		i = i*2; //Increasing table size

	}
	
	outFile <<"*******************************************************************************\n"<< std::endl;
}


void gshare_predict(std::string line, int table_size, unsigned long bitMask, S &globalHistory, 
	SaturatingCounter *table, long &correct, bool& gCorrect, int *isTournament)
{	
	int strLength = line.length();
        std::string line_long = line.substr(0,(strLength-2)); //Getting address from the line string
	long addr, index;
	unsigned long ghr;
	SaturatingCounter *h;
	
	addr = std::stoul(line_long, nullptr, 10); //converting address string into long type
	ghr = globalHistory.b & bitMask; //Masking
	index = addr ^ ghr;  // Ex-OR operation between GHR and ADDR for indexing of table
	index = index % table_size; //choosing lower order bits of index
	h = &(table[index]); //prediction for upcoming branch
	
	
	if(*h == T)
	{
		if(line[strLength-1] == 84)
		{
			if(isTournament != NULL) gCorrect = true;
			else correct++;
			globalHistory.b <<= 1;              //These two instrunction are mainly for left shift then changing the LSB position of history bit register to 1
			globalHistory.b |= 1UL << 0;
	
		}
		else
		{
			*h = t;
			globalHistory.b <<= 1;
			globalHistory.b &= ~(1UL << 0); //These two instrunction are mainly for left shift then changing the LSB position of history bits to 0
			
		}
	}else if(*h == t)
	{
		if(line[strLength-1] == 84)
		{	
			if(isTournament != NULL) gCorrect = true;
			else correct++;
			*h = T;
			globalHistory.b <<= 1;		//These two instrunction are mainly for left shift then changing the LSB position of history bit register to 1
			globalHistory.b |= 1UL << 0;
			
		}else
		{
			*h = n;
			globalHistory.b <<= 1;
			globalHistory.b &= ~(1UL << 0); //These two instrunction are mainly for left shift then changing the LSB position of history bits to 0
			
		}
	}else if(*h == n)
	{
		if(line[strLength-1] == 78)
		{
			if(isTournament != NULL) gCorrect = true;
			else correct++;
			*h = N;
			globalHistory.b <<= 1;
			globalHistory.b &= ~(1UL << 0); //These two instrunction are mainly for left shift then changing the LSB position of history bits to 0
			
		}else
		{
			*h = t;
			globalHistory.b <<= 1;  //These two instrunction are mainly for left shift then changing the LSB position of history bit register to 1
			globalHistory.b |= 1UL << 0;
			
		}
	}else
	{
		if(line[strLength-1] == 78)
		{
			if(isTournament != NULL) gCorrect = true;
			else correct++;
			globalHistory.b <<= 1;
			globalHistory.b &= ~(1UL << 0); //These two instrunction are mainly for left shift then changing the LSB position of history bits to 0
			
		}
		else
		{
			*h = n;
			globalHistory.b <<= 1;	//These two instrunction are mainly for left shift then changing the LSB position of history bit register to 1
			globalHistory.b |= 1UL << 0;
			
		}
	}
}

void gShare(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV)
{
	std::cout << "\nGshare Case 1\nGshare with variable counter size and fixed history bits 8\n"<< std::endl;
	outFile << "\nGshare Case 1\nGshare with variable counter size and fixed history bits 8\n"<< std::endl;
	long correct, total;
	float misPredictRate;
        SaturatingCounter *h;
	unsigned long bitMask = 0xFF; //Bit-Mask used to vary history lengths, Constant mask of 8 bit is kept here.
        long i = 4;              //Statrting with counter table size (or Predictor size) of 4
	while(i <= 1048576)
	{
		SaturatingCounter pTable[i];
		for(int j = 0; j < i; j++)
			pTable[j] = N;    // Initializing table each time with Strongly Not taken
		
                std::vector<std::string>::iterator it;
		S globalHistory = {0}; // Global History Bit register initializes with all bits set to zero.
		total = correct = 0;
		for(it = list.begin(); it != list.end(); it++)
		{
			gshare_predict(*it, i, bitMask, globalHistory, pTable, correct, NotTournamnet, NULL);
			total++;
		}
		misPredictRate = (((float)total-correct)/total) * 100; //Mis-Prediction Calculation
		//if(i != 1048576) writeF(correct,total,misPredictRate,outFile,outFileCSV,&i);
		//else 
		writeF(correct,total,misPredictRate,outFile,outFileCSV,&i);
		i = i*2; //increasing size of predictor after each iteration
		
	}
	
	outFile <<"*******************************************************************************\n"<< std::endl;
}

void gshare_c2(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV)
{
	std::cout << "\nGshare Case 3\nGshare with varying history bits from 0 to 19 and table sizes 2^10 and 2^16\n"<< std::endl;
	outFile << "\nGshare Case 3\nGshare with varying history bits from 0 to 19 and table sizes 2^10 and 2^16\n"<< std::endl;
	long correct, total;
	float misPredictRate;
        SaturatingCounter *h;
	int historybits=0;                 
	unsigned long bitMask=0x0; //Initializing Bit Mask with 0 bit (0 bit histrory is equivalent to bimodal only), But this value increases in while loop
        long i = 1024; //Statrting with counter table size (or Predictor size) of 1024


	  while(bitMask <= 0x7ffff) //upto 19 history length
	{

	while(i <= 65536) // In this case, data series is generated particularly for 1024 abd 65536 sizes only
	{
		
		SaturatingCounter pTable[i];
		for(int j = 0; j < i; j++)
			pTable[j] = N;    // Initializing table each time with Strongly Not taken
		
                std::vector<std::string>::iterator it;
		S globalHistory = {0}; // Global History Bit register initializes with all bits set to zero.
		total = correct = 0;
		for(it = list.begin(); it != list.end(); it++)
		{
			gshare_predict(*it, i, bitMask, globalHistory, pTable, correct, NotTournamnet, NULL); 
			total++;
		}
		misPredictRate = (((float)total-correct)/total) * 100; //Mis-Prediction Calculation
		//if(i != 65536)
		//{ 
			std::cout << "\n\nPredictor with size " << i << " with index & history bits "<< historybits << ".\n";
			outFile << "\n\nPredictor with size " << i << " with index & history bits "<< historybits << ".\n";
			writeF(correct,total,misPredictRate,outFile,outFileCSV,NULL);
			

		i = i*64; //increasing size of predictor up to 65536 
		
	}
	bitMask = (bitMask * 2) + 1; //increment in bits of bit mask for next iteration
	i=1024; 
	historybits++;
	}
	outFile <<"*******************************************************************************\n"<< std::endl;
	
}
void gshare_c3(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV)
{
	std::cout << "\nGshare Case 3\nGshare with varying with same index and history bits as from 0 to 19\n"<< std::endl;
	outFile << "\nGshare Case 3\nGshare with varying with same index and history bits as from 0 to 19\n"<< std::endl;
	long correct, total;
	float misPredictRate;
        SaturatingCounter *h;
	int historybits=0; 
	unsigned long bitMask = 0x0; //Initializing Bit Mask with 0 bit (0 bit histrory is equivalent to bimodal only), But this value increases in while loop
	long i = 1;

       while(bitMask <= 0x7ffff) //upto 19 history length
	{	
		SaturatingCounter pTable[i];
		for(int j = 0; j < i; j++)
			pTable[j] = N;  // Initializing table each time with Strongly Not taken
		
                std::vector<std::string>::iterator it;
		S globalHistory = {0}; // Global History Bit register initializes with all bits set to zero.
		total = correct = 0;
		for(it = list.begin(); it != list.end(); it++)
		{
			gshare_predict(*it, i, bitMask, globalHistory, pTable, correct, NotTournamnet, NULL);
			total++;
		}
		misPredictRate = (((float)total-correct)/total) * 100; //Mis-Prediction Calculation
		
			std::cout << "\n\nPredictor with size " << i << " with index & history bits "<< historybits << ".\n";
			outFile << "\n\nPredictor with size " << i << " with index & history bits "<< historybits << ".\n";
	                writeF(correct,total,misPredictRate,outFile,outFileCSV,NULL);
			
		
	historybits++;
	bitMask = (bitMask * 2) + 1; //increment in bits of bit mask for next iteration
	i = i*2;	//increasing size of predictor 
	
	}
	outFile <<"*******************************************************************************\n"<< std::endl;
	
}
void tournament_predict(std::string line, int table_size, SaturatingChooser *table, long &correct, bool& gCorrect, bool& bCorrect)
{
	long addr;
	int strLength = line.length(); 
	SaturatingChooser *c;
        std::string line_long = line.substr(0,(strLength-2)); //Getting address from the line string
	addr = std::stoul(line_long, nullptr, 10); //Converting address string into long value
	addr = addr % table_size; //Finding lower order bits of address
	c = &(table[addr]); //prediction made
	
	if((bCorrect && gCorrect) || (!bCorrect && !gCorrect))
		{
			if(bCorrect && gCorrect)   // No updation in table when both are coorect or both are incorrect
				correct++;            
		}else if(*c == G)
		{
			if(gCorrect)
			{
				correct++;
			}else
			{
				*c = g;
			}
		}else if(*c == g)
		{
			if(gCorrect)
			{
				correct++;
				*c = G;
			}else
			{
				*c = b;
			}
		}else if(*c == b)
		{
			if(bCorrect)
			{
				correct++;
				*c = B;
			}else
			{
				*c = g;
			}
		}else
		{
			if(bCorrect)
			{
				correct++;
			}else
			{
				*c = b;
			}
		}
}
void tournament(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV)
{
	std::cout << "\nTournament Case 1\nTournament Predictor Accuracy with equal table sizes of Gshare(history bits = index bits), Bimoda and Tournament tables\n"<< std::endl;
	outFile << "\nTournament Case 1\nTournament Predictor Accuracy with equal table sizes of Gshare(history bits = index bits), Bimoda and Tournament tables\n"<< std::endl;
	long correct, total;
	int  isTournament=1; //based on this argument bimodal_predict and gshare_predict will work according to tournamet predictor
	bool bCorrect, gCorrect;
	long table_size =4;


	unsigned long bitMask = 0x3; //Initializing Bit Mask with 2 bits (0 bit histrory is equivalent to bimodal only), But this value increases in while loop
	float misPredictRate;
	
	while(bitMask <= 0x7ffff) { 			//upto 19 history length
	SaturatingCounter bTable[table_size];
	SaturatingCounter gTable[table_size];
	SaturatingChooser sTable[table_size];
	
	S globalHistory = {0};   // Global History Bit register initializes with all bits set to zero.
	for(int i = 0; i < table_size; i++)	
	{
		gTable[i] = bTable[i] = N; // Initializing tables each time with Strongly Not taken
		sTable[i] = B; // Initializing SaturatingChooser table each time with Strongly Bimodal
	}
	
	std::vector<std::string>::iterator it;
	
	total = correct = bCorrect = gCorrect = 0;
	for(it = list.begin(); it != list.end(); it++)
	{
		bCorrect = false;  //gcorrect and bcorrect will be used to get results from gshare_predict and bimodal_predict
		gCorrect = false;  // then these will be passed to tournament_predict function which decides which type of prediction scheme to prefer
		gshare_predict(*it, table_size, bitMask, globalHistory, gTable, correct, gCorrect, &isTournament);  
		bimodal_predict(*it, table_size, bTable, correct, bCorrect, &isTournament);
		tournament_predict(*it, table_size, sTable, correct, gCorrect, bCorrect);
		total++;
	}
	misPredictRate = (((float)total-correct)/total) * 100;
	writeF(correct,total,misPredictRate,outFile,outFileCSV,&table_size);
	
	bitMask = (bitMask * 2) + 1;
	table_size = table_size*2;
	}
	outFile <<"*******************************************************************************\n"<< std::endl;

	}
void tournament_c2(std::vector<std::string> &list, std::fstream &outFile, std::fstream &outFileCSV)
{
	std::cout << "\nTournament Case 2\nTournament Predictor Accuracy with 2^n storage (i.e. 2^n-2 SaturatingChooser table, 2^n-2 Bimodal table and 2^n-1 Gshare table)\n"<< std::endl;
	outFile << "\nTournament Case 2\nTournament Predictor Accuracy with 2^n storage (i.e. 2^n-2 SaturatingChooser table, 2^n-2 Bimodal table and 2^n-1 Gshare table)\n"<< std::endl;
	long correct, total;
	int isTournament=1;
	bool bCorrect, gCorrect;
	long table_size =4;

	unsigned long bitMask = 0x3; //Initializing Bit Mask with 2 bit (0 bit histrory is equivalent to bimodal only), But this value increases in while loop
	float misPredictRate;
	
	while(bitMask <= 0x7ffff){   //upto 19 history length
	SaturatingCounter bTable[table_size/4];
	SaturatingChooser sTable[table_size/4];
	SaturatingCounter gTable[table_size/2];
	
	
	S globalHistory = {0};
	for(int i = 0; i < table_size; i++)	
	{
		if(i<=table_size/2) gTable[i] = N;   // Here we are managing sizes of table (i.e. 2^n-2 SaturatingChooser table, 2^n-2 Bimodal table and 2^n-1 Gshare table)
		if(i<=table_size/4){ bTable[i] = N; sTable[i] = B;}
	}
	
	std::vector<std::string>::iterator it;
	
	total = correct = bCorrect = gCorrect = 0;
	for(it = list.begin(); it != list.end(); it++)
	{	
		bCorrect = false;	//gcorrect and bcorrect will be used to get results from gshare_predict and bimodal_predict
		gCorrect = false;  // then these will be passed to tournament_predict function which decides which type of prediction scheme to prefer
		gshare_predict(*it, (table_size/2), bitMask, globalHistory, gTable, correct, gCorrect, &isTournament);
		bimodal_predict(*it, (table_size/4), bTable, correct, bCorrect, &isTournament);
		tournament_predict(*it, (table_size/4), sTable, correct, gCorrect, bCorrect);
		total++;
	}
	misPredictRate = (((float)total-correct)/total) * 100;
	writeF(correct,total,misPredictRate,outFile,outFileCSV,&table_size);
	
	bitMask = (bitMask * 2) + 1;
	table_size = table_size*2;
	}
	
	outFile <<"*******************************************************************************\n"<< std::endl;
	}


int main(int argc, char const* argv[])
{
	
	
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); 
	
	std::vector<std::string> list;
	
	std::fstream inFile, outFile, outFileCSV;
	inFile.open(argv[1], std::ios::in);   //Input trace file
	outFile.open("output.txt", std::ios::out | std::ios::app);  //For text 
	outFileCSV.open("output.csv", std::ios::out | std::ios::app); // For CSV
	
	std::string line;
	while(std::getline(inFile, line))
	{
		list.push_back(line);
	}
	inFile.close();


	always_taken(list, outFile, outFileCSV);
	always_not_taken(list, outFile, outFileCSV);
	bimodal(list, outFile, outFileCSV);
	gShare(list, outFile, outFileCSV);
        gshare_c2(list, outFile, outFileCSV);
	gshare_c3(list, outFile, outFileCSV);
	tournament(list, outFile, outFileCSV);
	tournament_c2(list, outFile, outFileCSV);
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	 
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "sec" << std::endl; 
	outFile.close();
	outFileCSV.close();
	
	return 0;
}

