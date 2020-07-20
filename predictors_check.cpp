#include <iostream>	
#include <bitset>	
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <bits/stdc++.h> 

enum SaturatingCounter{T = 0, t, n, N}; //2-bit saturating counter values for bimodal and gshare
enum SaturatingChooser{G = 0, g, b, B};
struct S{
	unsigned int b : 19; // Default 19 bits history lengths, in later stages it varied by bit-mask
};
//This function is used to convert gshare, bimodal, chooser table, contents into string (e.g NNNnnNNN, TNTNTtnTNT. BNnbBBBB)
std::string ToString(SaturatingCounter *table, SaturatingChooser *sTable, int type); 

//This function is used to convert History bits register contents into string (e.g TTTT)
std::string ToString2(unsigned int num, int isTournament); 

//This function is called by function "ToString2(unsigned int num)"
void findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr); 

//This function is used to conver long address value into hex value
std::string long2hex(long num); 

void bimodal_predict(std::string line, int table_size, SaturatingCounter *table, long &correct, std::fstream &outFile2, bool& bCorrect, int *isTournament);
void bimodal(std::vector<std::string> &list, std::fstream &outFile2);
void gShare_predict(std::string line, int table_size, unsigned long bitMask, S &globalHistory, SaturatingCounter *gTable, long &correct, std::fstream &outFile, bool& gCorrect, int *isTournament);
void gShare(std::vector<std::string> &list, std::fstream &outFile);
void tournament_predict(std::string line, int table_size, SaturatingChooser *sTable, S &globalHistory, SaturatingCounter *gTable, SaturatingCounter *bTable, long &correct, bool& gCorrect, bool& bCorrect, std::fstream &outFile);
void tournament(std::vector<std::string> &list, std::fstream &outFile);

//Global declaration
int numIncorrect = 0;
int numIncorrect2 = 0;
int numIncorrect3 = 0;
std::string isCorrect = "NA";
std::string isCorrectG = "NA";
std::string tableB = "NA";
std::string tableG = "NA";
std::string tableT = "NA";
std::string Historybits = "NA";
bool NotTournamnet = false;


void bimodal_predict(std::string line, int table_size, SaturatingCounter *table, long &correct, std::fstream &outFile2, bool& bCorrect, int *isTournament)
{
	long addr, addr2, type=1;
	int strLength = line.length(); //Finding length of line string
	std::string line_long = line.substr(0,(strLength-2)); //Getting address from the line string
	addr = std::stoul(line_long, nullptr, 10); //converting address string into long
	addr2 = std::stoul(line_long, nullptr, 10);
	addr = addr % table_size; //calculating index value for prediction
	SaturatingCounter *h = &(table[addr]); //prediction made
	
	if(*h == T)
	{
		if(line[strLength-1] == 84)
		{	
			if(isTournament != NULL) {bCorrect = true; isCorrect="  T | T  correct    "; tableB=ToString(table, NULL, type); }
			
			else {correct++;
			outFile2 << ToString(table, NULL, type) << " | "<<long2hex(addr2)<< "  T | T  correct    "<<numIncorrect2<<std::endl;}
		}
		else
		{
			if(isTournament == NULL){
			numIncorrect2++;
			outFile2 << ToString(table, NULL, type) << " | "<<long2hex(addr2)<< "  N | T  incorrect  "<<numIncorrect2<<std::endl;}
			else{ isCorrect="  N | T  incorrect  "; tableB=ToString(table, NULL, type); }
			*h = t;
		}
	}
	else if(*h == t)
	{
		if(line[strLength-1] == 84)
		{	
			if(isTournament != NULL) {bCorrect = true; isCorrect="  T | T  correct    "; tableB=ToString(table, NULL, type);}
			else {correct++;
			outFile2 << ToString(table, NULL, type) << " | "<<long2hex(addr2)<< "  T | T  correct    "<<numIncorrect2<<std::endl;}
			*h = T;
		}
		else
		{
			if(isTournament == NULL){
			numIncorrect2++;
			outFile2 << ToString(table, NULL, type) << " | "<<long2hex(addr2)<< "  N | T  incorrect  "<<numIncorrect2<<std::endl;}
			else {isCorrect="  N | T  incorrect  ";tableB=ToString(table, NULL, type); }
			*h = n;
		}
	}
	else if(*h == n)
	{
		if(line[strLength-1] == 78)
		{
			if(isTournament != NULL) {bCorrect = true; isCorrect="  N | N  correct    ";tableB=ToString(table, NULL, type); }
			else {correct++;
			outFile2 << ToString(table, NULL, type) << " | "<<long2hex(addr2)<< "  N | N  correct    "<<numIncorrect2<<std::endl;}
			*h = N;
		}
		else
		{
			if(isTournament == NULL){
			numIncorrect2++;
			outFile2 << ToString(table, NULL, type) << " | "<<long2hex(addr2)<< "  T | N  incorrect  "<<numIncorrect2<<std::endl;}
			else {isCorrect="  T | N  incorrect  ";tableB=ToString(table, NULL, type); }
			*h = t;
		}
	}
	else
	{
		if(line[strLength-1] == 78){
			if(isTournament != NULL) {bCorrect = true; isCorrect="  N | N  correct    ";tableB=ToString(table, NULL, type); }
			else{
			correct++;
			outFile2 << ToString(table, NULL, type) << " | "<<long2hex(addr2)<< "  N | N  correct    "<<numIncorrect2<<std::endl;
			}
			
			}
		else{	
			if(isTournament == NULL){
			numIncorrect2++;
			outFile2 << ToString(table, NULL, type) << " | "<<long2hex(addr2)<< "  T | N  incorrect  "<<numIncorrect2<<std::endl;}
			else {isCorrect="  T | N  incorrect  "; tableB=ToString(table, NULL, type); }
			*h = n;
		}
	}
}

void bimodal(std::vector<std::string> &list, std::fstream &outFile2)
{
	std::cout << "\nBimodal with variable counter size\n"<< std::endl;

	long correct, total;
	float misPredictRate;
	int i = 8;	//Initializing with predictor size of 4
	
	
	std::vector<std::string>::iterator it;
	
	while(i <= 8)
	{
		SaturatingCounter table[i];
		for(int j = 0; j<i; j++)
			table[j] = N; // Initializing table each time with Strongly Not taken
		
		total = correct = 0;
		for(it = list.begin(); it != list.end(); it++)
		{
			bimodal_predict(*it, i, table, correct, outFile2, NotTournamnet, NULL);
			total++;
		}
		misPredictRate = (((float)total-correct)/total) * 100;
		if(i != 8)
		{
	
			std::cout << "\n\nPredictor with " << i << " predictor size\n";
			std::cout << "Correct predictions = "<<correct << ", " << "Total predictions = "<< total << ", " <<"Mis-predictions Rate = "<< misPredictRate << std::endl;
		}else
		{
			
			
			
			std::cout << "\n\nPredictor with " << i << " predictor size\n";
			std::cout << "Correct predictions = "<<correct << ", " << "Total predictions = "<< total << ", " <<"Mis-predictions Rate = "<< misPredictRate << std::endl;
		}
		i = i*4;

	}
	
	
}


std::string ToString(SaturatingCounter *table, SaturatingChooser *sTable, int type){
SaturatingCounter *h;
SaturatingChooser *c;
std::string s;
int index=0;
switch (type){
case 1: for(index = 0; index < 8; index++)
		{
		h = &(table[index]);
		if(*h == T) s.push_back('T');
		if(*h == t) s.push_back('t');
		if(*h == n) s.push_back('n');
		if(*h == N) s.push_back('N');
		
		}
	        return s;
case 2: for(index = 0; index < 16; index++)
		{
		h = &(table[index]);
		if(*h == T) s.push_back('T');
		if(*h == t) s.push_back('t');
		if(*h == n) s.push_back('n');
		if(*h == N) s.push_back('N');
		
		}
	        return s;
case 3: for(index = 0; index < 8; index++)
		{
		c = &(sTable[index]);
		if(*c == G) s.push_back('G');
		if(*c == g) s.push_back('g');
		if(*c == b) s.push_back('b');
		if(*c == B) s.push_back('B');
		
		}
		return s;
}
}
void findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr)
{

	size_t pos = data.find(toSearch);
 

	while( pos != std::string::npos)
	{
		data.replace(pos, toSearch.size(), replaceStr);
		pos =data.find(toSearch, pos + replaceStr.size());
	}
}

std::string ToString2(unsigned int num, int **isTournament){
std::string binary;
if(*isTournament != NULL) binary = std::bitset<4>(num).to_string(); //to binary
else binary = std::bitset<3>(num).to_string();
findAndReplaceAll(binary, "0", "N");
findAndReplaceAll(binary, "1", "T");
return binary;
}

std::string long2hex(long num){
std::string s;
std::stringstream ss; 
    ss << std::hex << num; 
    s.append(ss.str()); 
    
return s;
}


void gShare_predict(std::string line, int table_size, unsigned long bitMask, S &globalHistory, 
	SaturatingCounter *gTable, long &correct, std::fstream &outFile, bool& gCorrect, int *isTournament)
{
	int strLength = line.length();
	std::string line_long = line.substr(0,(strLength-2)); //Getting address from the line string
	long addr, index;
	unsigned long ghr;
	SaturatingCounter *h;
	
	addr = std::stoul(line_long, nullptr, 10); //converting address string into long type
	ghr = globalHistory.b & bitMask; //Masking
	index = addr ^ ghr; // Ex-OR operation between GHR and ADDR for indexing of table
	index = index % table_size; //choosing lower order bits of index
	h = &(gTable[index]); //prediction for branch
	int type = 2;
	
	
	if(*h == T)
	{
		if(line[strLength-1] == 84)
		{
			if(isTournament != NULL) {gCorrect = true; isCorrectG="  T | T  correct    "; tableG=ToString(gTable, NULL, type); Historybits=ToString2(globalHistory.b, &isTournament);}
			else {
			outFile << ToString(gTable, NULL, type)<<"  "<<ToString2(globalHistory.b, &isTournament)<< " | "<<long2hex(addr)<< "  T | T  correct    "<<numIncorrect<<std::endl; 
			correct++;}
			globalHistory.b <<= 1; //These two instrunction are mainly for left shift then changing the LSB position of history bit register to 1
			globalHistory.b |= 1UL << 0;
			
	
		}
		else
		{	
			if(isTournament == NULL){
			numIncorrect++;	
			outFile << ToString(gTable, NULL, type)<<"  "<<ToString2(globalHistory.b, &isTournament)<< " | "<<long2hex(addr)<< "  N | T  incorrect  "<<numIncorrect<<std::endl;}
			else { isCorrectG="  N | T  incorrect  "; tableG=ToString(gTable, NULL, type); Historybits=ToString2(globalHistory.b, &isTournament);}
			*h = t;
			globalHistory.b <<= 1;
			globalHistory.b &= ~(1UL << 0); //These two instrunction are mainly for left shift then changing the LSB position of history bits to 0
			
		}
	}else if(*h == t)
	{
		if(line[strLength-1] == 84)
		{	
			if(isTournament != NULL) {gCorrect = true; isCorrectG="  T | T  correct    "; tableG=ToString(gTable, NULL, type); Historybits=ToString2(globalHistory.b, &isTournament);}
			else {
			outFile << ToString(gTable, NULL, type)<<"  "<<ToString2(globalHistory.b, &isTournament)<< " | "<<long2hex(addr)<< "  T | T  correct    "<<numIncorrect<<std::endl;
			correct++;}
			*h = T;
			globalHistory.b <<= 1; //These two instrunction are mainly for left shift then changing the LSB position of history bit register to 1
			globalHistory.b |= 1UL << 0;
			
		}else
		{	
			if(isTournament == NULL){
			numIncorrect++;
			outFile << ToString(gTable, NULL, type)<<"  "<<ToString2(globalHistory.b, &isTournament)<< " | "<<long2hex(addr)<< "  N | T  incorrect  "<<numIncorrect<<std::endl;}
			else{ isCorrectG="  N | T  incorrect  "; tableG=ToString(gTable, NULL, type); Historybits=ToString2(globalHistory.b, &isTournament);}
			*h = n;
			globalHistory.b <<= 1;
			globalHistory.b &= ~(1UL << 0); //These two instrunction are mainly for left shift then changing the LSB position of history bits to 0
			
		}
	}else if(*h == n)
	{
		if(line[strLength-1] == 78)
		{
			if(isTournament != NULL) {gCorrect = true; isCorrectG="  N | N  correct    "; tableG=ToString(gTable, NULL, type); Historybits=ToString2(globalHistory.b, &isTournament);}
			else {
			outFile << ToString(gTable, NULL, type)<<"  "<<ToString2(globalHistory.b, &isTournament)<< " | "<<long2hex(addr)<< "  N | N  correct    "<<numIncorrect<<std::endl;
			correct++;}
			*h = N;
			globalHistory.b <<= 1; //These two instrunction are mainly for left shift then changing the LSB position of history bits to 0
			globalHistory.b &= ~(1UL << 0);
			
		}else
		{	
			if(isTournament == NULL){
			numIncorrect++;
			outFile << ToString(gTable, NULL, type)<<"  "<<ToString2(globalHistory.b, &isTournament)<< " | "<<long2hex(addr)<< "  T | N  incorrect  "<<numIncorrect<<std::endl;}
			else {isCorrectG="  T | N  incorrect  "; tableG=ToString(gTable, NULL, type); Historybits=ToString2(globalHistory.b, &isTournament);}
			*h = t;
			globalHistory.b <<= 1; //These two instrunction are mainly for left shift then changing the LSB position of history bit register to 1
			globalHistory.b |= 1UL << 0;
			
		}
	}else
	{
		if(line[strLength-1] == 78)
		{	if(isTournament != NULL) {gCorrect = true; isCorrectG="  N | N  correct    "; tableG=ToString(gTable, NULL, type); Historybits=ToString2(globalHistory.b, &isTournament);}
			else {
			outFile << ToString(gTable, NULL, type)<<"  "<<ToString2(globalHistory.b, &isTournament)<< " | "<<long2hex(addr)<< "  N | N  correct    "<<numIncorrect<<std::endl;
			correct++;}
			globalHistory.b <<= 1; //These two instrunction are mainly for left shift then changing the LSB position of history bits to 0
			globalHistory.b &= ~(1UL << 0);
			
		}
		else
		{
			if(isTournament == NULL){
			numIncorrect++;
			outFile << ToString(gTable, NULL, type) <<"  "<<ToString2(globalHistory.b, &isTournament)<< " | "<<long2hex(addr)<< "  T | N  incorrect  "<<numIncorrect<<std::endl;}
			else { isCorrectG="  T | N  incorrect  "; tableG=ToString(gTable, NULL, type); Historybits=ToString2(globalHistory.b, &isTournament);}
			*h = n;
			globalHistory.b <<= 1; //These two instrunction are mainly for left shift then changing the LSB position of history bit register to 1
			globalHistory.b |= 1UL << 0;
			
		}
	}
}

void gShare(std::vector<std::string> &list, std::fstream &outFile)
{

	long correct, total;
	float misPredictRate;
        SaturatingCounter *h;
	unsigned long bitMask = 0x7; //Bit-Mask used to vary history lengths, Constant mask of 3 bit is kept here.
        long i = 16;
	
	

	while(i <= 16) //this loop runs only once as we want data for predictor size of 16 only
	{
		SaturatingCounter pTable[i];
		for(int j = 0; j < i; j++)
			pTable[j] = N; // Initializing tables each time with Strongly Not taken
		
                std::vector<std::string>::iterator it;
		S globalHistory = {0};
		total = correct = 0;
		for(it = list.begin(); it != list.end(); it++)
		{
			gShare_predict(*it, i, bitMask, globalHistory, pTable, correct, outFile, NotTournamnet, NULL);
			total++;
		}
		misPredictRate = (((float)total-correct)/total) * 100;
		if(i != 16)
		{ 
			std::cout << "\n\nPredictor with " << i << " predictor size\n";
			std::cout << "Correct predictions = "<<correct << ", " << "Total predictions = "<< total << ", " <<"Mis-predictions Rate = "<< misPredictRate << std::endl;
	
			
		}else
		{
			std::cout << "\n\nPredictor with " << i << " predictor size\n";
			std::cout << "Correct predictions = "<<correct << ", " << "Total predictions = "<< total << ", " <<"Mis-predictions Rate = "<< misPredictRate << std::endl;
	
			
		}
		i = i*4;
		
	}
	
	//outFile <<"*******************************************************************************"<< std::endl;
}


void tournament_predict(std::string line, int table_size, SaturatingChooser *sTable, S &globalHistory, SaturatingCounter *gTable, SaturatingCounter *bTable, long &correct, bool& gCorrect, bool& bCorrect, std::fstream &outFile)
{
	long addr, addr2;
	SaturatingChooser *c;
	int type =3;
	int strLength = line.length();
	std::string line_long = line.substr(0,(strLength-2)); //Getting address from the line string
	addr2 = std::stoul(line_long, nullptr, 10); //Converting address string into long value
	addr = addr2 % table_size; //Finding lower order bits of address
	c = &(sTable[addr]); //prediction for branch
	
	if((bCorrect && gCorrect) || (!bCorrect && !gCorrect)) // No updation in table when both are coorect or both are incorrect
		{
			if(bCorrect && gCorrect) correct++; else numIncorrect3++;
			outFile << ToString(NULL, sTable, type)<<" "<< tableB <<" "<< tableG <<"  "<<Historybits<< " | "<<long2hex(addr2)<< isCorrect<<numIncorrect3<<std::endl;
			
				
		}else if(*c == G)
		{
			if(gCorrect)
			{
				correct++;
				outFile << ToString(NULL, sTable, type)<<" "<< tableB<<" "<< tableG<<"  "<<Historybits<< " | "<<long2hex(addr2)<< isCorrectG<<numIncorrect3<<std::endl;

			}else
			{	
				numIncorrect3++;
				outFile << ToString(NULL, sTable, type)<<" "<< tableB<<" "<< tableG<<"  "<<Historybits<< " | "<<long2hex(addr2)<< isCorrectG<<numIncorrect3<<std::endl;
				*c = g;
			}
		}else if(*c == g)
		{
			if(gCorrect)
			{
				correct++;
				outFile << ToString(NULL, sTable, type)<<" "<< tableB<<" "<< tableG<<"  "<<Historybits<< " | "<<long2hex(addr2)<< isCorrectG<<numIncorrect3<<std::endl;
				*c = G;
			}else
			{	
				numIncorrect3++;
				outFile << ToString(NULL, sTable, type)<<" "<< tableB<<" "<< tableG<<"  "<<Historybits<< " | "<<long2hex(addr2)<< isCorrectG<<numIncorrect3<<std::endl;
				*c = b;
			}
		}else if(*c == b)
		{
			if(bCorrect)
			{
				correct++;
				outFile << ToString(NULL, sTable, type)<<" "<< tableB<<" "<< tableG<<"  "<<Historybits<< " | "<<long2hex(addr2)<< isCorrect<<numIncorrect3<<std::endl;
				*c = B;
			}else
			{	
				numIncorrect3++;
				outFile << ToString(NULL, sTable, type)<<" "<< tableB<<" "<< tableG<<"  "<<Historybits<< " | "<<long2hex(addr2)<< isCorrect<<numIncorrect3<<std::endl;
				*c = g;
			}
		}else
		{
			if(bCorrect)
			{
				correct++;
				outFile << ToString(NULL, sTable, type)<<" "<< tableB<<" "<< tableG<<"  "<<Historybits<< " | "<<long2hex(addr2)<< isCorrect<<numIncorrect3<<std::endl;
			}else
			{	numIncorrect3++;
				outFile << ToString(NULL, sTable, type)<<" "<< tableB<<" "<< tableG<<"  "<<Historybits<< " | "<<long2hex(addr2)<< isCorrect<<numIncorrect3<<std::endl;
				*c = b;
			}
		}
}
void tournament(std::vector<std::string> &list, std::fstream &outFile)
{
	long correct, total;
	bool bCorrect, gCorrect;
	int isTournament = 1;
	long table_size1 =8, table_size2 =16;
	SaturatingCounter *h;
	float misPredictRate;
	SaturatingChooser *c;
	unsigned long bitMask = 0xf; //Initializing Bit Mask with 4 bits (0 bit histrory is equivalent to bimodal only)
	SaturatingCounter gTable[table_size2];
	SaturatingCounter bTable[table_size1];
	
	SaturatingChooser sTable[table_size1];
	std::string s;

	
	S globalHistory = {0}; // Global History Bit register initializes with all bits set to zero.
	
	for(int i = 0; i < table_size2; i++)	
	{	
		if(i<8){      //table size for bimodal and chooser is 8
		bTable[i] = N; // Initializing tables each time with Strongly Not taken
		sTable[i] = B;
		}
		gTable[i] = N; // Initializing tables each time with Strongly Not taken
		
	}
	std::vector<std::string>::iterator it;
	
	total = correct = bCorrect = gCorrect = 0;
	for(it = list.begin(); it != list.end(); it++)
	{
		bCorrect = false;//gcorrect and bcorrect will be used to get results from gshare_predict and bimodal _predict
		gCorrect = false;// then these will be passed to tournament_predict function which decides which type of prediction scheme to prefer
		gShare_predict(*it, table_size2, bitMask, globalHistory, gTable, correct, outFile, gCorrect, &isTournament);
		bimodal_predict(*it, table_size1, bTable, correct, outFile, bCorrect, &isTournament);
		tournament_predict(*it, table_size1, sTable, globalHistory, gTable, bTable, correct, gCorrect, bCorrect, outFile);
		total++;
	}
	misPredictRate = (((float)total-correct)/total) * 100;
	std::cout << "\n\nPredictor with " << table_size1 << " predictor size\n";
	std::cout << "Correct predictions = "<<correct << ", " << "Total predictions = "<< total << ", " <<"Mis-predictions Rate = "<< misPredictRate << std::endl;
	//outFile << correct << "," << total << ";" << std::endl;
}


int main(int argc, char const* argv[])
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); 
	
	std::vector<std::string> list;
	
	std::fstream inFile, outFile, outFile2, outFile3;
	inFile.open(argv[1], std::ios::in);
	outFile.open("outGshareCheck.txt", std::ios::out | std::ios::app); //Three output files for bimodal, gshare and tournamnet
	outFile2.open("outBimodalCheck.txt", std::ios::out | std::ios::app);
	outFile3.open("outTournamentCheck.txt", std::ios::out | std::ios::app);
	
	std::string line;
	while(std::getline(inFile, line))
	{
		list.push_back(line);
	}
	inFile.close();

	bimodal(list, outFile2);
	gShare(list, outFile);
    	tournament(list, outFile3);
	
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	 
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms" << std::endl; 
	
	outFile.close();
	outFile2.close();
	outFile3.close();
	
	
	return 0;
}

