#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <random>
#include "../shared/common.hpp"
#include "../shared/patterns.hpp"
#include "../shared/timer.hpp"
#include "../shared/rank.hpp"
#include "../shared/select.hpp"

using namespace std;
using namespace shared;

ChronoStopWatch timer;

void rankSelectBch_32(const char *fileName, const char *propFileName, const char *queriesNum);
void rankSelectMPE2_32(const char *fileName, const char *propFileName, const char *queriesNum);

void getUsage(char **argv) {
	cout << "Select mixed rank&select you want to test:" << endl;
	cout << "mixed-rank-select-bch: " << argv[0] << " bch fileName propFileName queriesNum" << endl;
	cout << "mixed-rank-select-mpe2: " << argv[0] << " mpe2 fileName propFileName queriesNum" << endl;
	cout << "where:" << endl;
	cout << "fileName - name of bits file" << endl;
	cout << "propFileName - name of the file with mixed-rank-select query distribution" << endl;
	cout << "queriesNum - number of queries" << endl << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 5) {
		getUsage(argv);
		exit(1);
	}
	
	if (string(argv[1]) == "bch") {
		rankSelectBch_32(argv[3], argv[4], argv[5]);
	}
	if (string(argv[1]) == "mpe2") {
		rankSelectMPE2_32(argv[3], argv[4], argv[5]);
	}
	
	getUsage(argv);
	exit(1);
}

unsigned int *getPatternsForRank32(const char *fileName, const char *queriesNum, unsigned int textLen) {
	string ranksFileNameString = "ranks-" + string(fileName) + "-" + string(queriesNum) + ".dat";
    const char *ranksFileName = ranksFileNameString.c_str();
    
    unsigned int *patRanks = new unsigned int[atoi(queriesNum)];
    
    if (fileExists(ranksFileName)) {
		FILE *inFile;
		inFile = fopen(ranksFileName, "rb");
		size_t result;
		result = fread(patRanks, (size_t)sizeof(unsigned int), (size_t)atoi(queriesNum), inFile);
		if (result != (unsigned int)atoi(queriesNum)) {
			cout << "Error loading data from " << ranksFileName << endl;
			exit(1);
		}
		fclose(inFile);
    } else {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<unsigned int> dis(1, 8 * textLen);
		for (unsigned int i = 0; i < (unsigned int)atoi(queriesNum); ++i) patRanks[i] = dis(gen);
		FILE *outFile;
		outFile = fopen(ranksFileName, "w");
		fwrite(patRanks, (size_t)sizeof(unsigned int), (size_t)atoi(queriesNum), outFile);
        fclose(outFile);
    }
	return patRanks;
}

unsigned long long *getPatternsForRank64(const char *fileName, const char *queriesNum, unsigned int textLen) {
	string ranksFileNameString = "ranks-64-" + string(fileName) + "-" + string(queriesNum) + ".dat";
    const char *ranksFileName = ranksFileNameString.c_str();
    
    unsigned long long *patRanks = new unsigned long long[atoi(queriesNum)];
    
    if (fileExists(ranksFileName)) {
		FILE *inFile;
		inFile = fopen(ranksFileName, "rb");
		size_t result;
		result = fread(patRanks, (size_t)sizeof(unsigned long long), (size_t)atoi(queriesNum), inFile);
		if (result != (unsigned int)atoi(queriesNum)) {
			cout << "Error loading data from " << ranksFileName << endl;
			exit(1);
		}
		fclose(inFile);
    } else {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<unsigned long long> dis(1, 8 * (unsigned long long)textLen);
		for (unsigned int i = 0; i < (unsigned int)atoi(queriesNum); ++i) patRanks[i] = dis(gen);
		FILE *outFile;
		outFile = fopen(ranksFileName, "w");
		fwrite(patRanks, (size_t)sizeof(unsigned long long), (size_t)atoi(queriesNum), outFile);
        fclose(outFile);
    }
	return patRanks;
}

unsigned int *getPatternsForSelect32(const char *fileName, const char *queriesNum, unsigned char *text, unsigned int textLen) {
	string selectsFileNameString = "selects-" + string(fileName) + "-" + string(queriesNum) + ".dat";
    const char *selectsFileName = selectsFileNameString.c_str();
    
    unsigned int *patSelects = new unsigned int[atoi(queriesNum)];
    
    if (fileExists(selectsFileName)) {
		FILE *inFile;
		inFile = fopen(selectsFileName, "rb");
		size_t result;
		result = fread(patSelects, (size_t)sizeof(unsigned int), (size_t)atoi(queriesNum), inFile);
		if (result != (unsigned int)atoi(queriesNum)) {
			cout << "Error loading data from " << selectsFileName << endl;
			exit(1);
		}
		fclose(inFile);
    } else {
		unsigned int numberOfOnes = 0;
        for (unsigned int i = 0; i < textLen; ++i) numberOfOnes += __builtin_popcount(text[i]);
		random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<unsigned int> dis(1, numberOfOnes);
        for (unsigned int i = 0; i < (unsigned int)atoi(queriesNum); ++i) patSelects[i] = dis(gen);
        FILE *outFile;
		outFile = fopen(selectsFileName, "w");
		fwrite(patSelects, (size_t)sizeof(unsigned int), (size_t)atoi(queriesNum), outFile);
        fclose(outFile);
    }
	return patSelects;
}

unsigned long long *getPatternsForSelect64(const char *fileName, const char *queriesNum, unsigned char *text, unsigned int textLen) {
	string selectsFileNameString = "selects-64-" + string(fileName) + "-" + string(queriesNum) + ".dat";
    const char *selectsFileName = selectsFileNameString.c_str();
    
    unsigned long long *patSelects = new unsigned long long[atoi(queriesNum)];
    
    if (fileExists(selectsFileName)) {
		FILE *inFile;
		inFile = fopen(selectsFileName, "rb");
		size_t result;
		result = fread(patSelects, (size_t)sizeof(unsigned long long), (size_t)atoi(queriesNum), inFile);
		if (result != (unsigned int)atoi(queriesNum)) {
			cout << "Error loading data from " << selectsFileName << endl;
			exit(1);
		}
		fclose(inFile);
    } else {
		unsigned long long numberOfOnes = 0;
        for (unsigned int i = 0; i < textLen; ++i) numberOfOnes += __builtin_popcount(text[i]);
		random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<unsigned long long> dis(1, numberOfOnes);
        for (unsigned int i = 0; i < (unsigned int)atoi(queriesNum); ++i) patSelects[i] = dis(gen);
        FILE *outFile;
		outFile = fopen(selectsFileName, "w");
		fwrite(patSelects, (size_t)sizeof(unsigned long long), (size_t)atoi(queriesNum), outFile);
        fclose(outFile);
    }
	return patSelects;
}

void rankSelectBch_32(const char *fileName, const char *propFileName, const char *queriesNum) {
	RankBasic32<RANK_BASIC_COMPRESSED_HEADERS> *rank = new RankBasic32<RANK_BASIC_COMPRESSED_HEADERS>();
	string rankFileNameString = (string)fileName + "-bch-32.rank";
	const char *rankFileName = rankFileNameString.c_str();
	
	SelectBasic32<SELECT_BASIC_COMPRESSED_HEADERS, 128, 4096> *select = new SelectBasic32<SELECT_BASIC_COMPRESSED_HEADERS, 128, 4096>();
	string selectFileNameString = (string)fileName + "-bch-32-128-4096.select";
	const char *selectFileName = selectFileNameString.c_str();
	
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);
	unsigned int propLen;
	unsigned char* prop = readText(propFileName, propLen, 0);

	if (fileExists(rankFileName)) {
		FILE *inFile;
		inFile = fopen(rankFileName, "rb");
		rank->load(inFile);
		fclose(inFile);
	} else {
		rank->build(text, textLen);
		FILE *outFile;
		outFile = fopen(rankFileName, "w");
		rank->save(outFile);
		fclose(outFile);
	}
	
	if (fileExists(selectFileName)) {
		FILE *inFile;
		inFile = fopen(selectFileName, "rb");
		select->load(inFile);
		fclose(inFile);
	} else {
		select->build(text, textLen);
		FILE *outFile;
		outFile = fopen(selectFileName, "w");
		select->save(outFile);
		fclose(outFile);
	}
    
	unsigned int queriesNumInt = atoi(queriesNum);
	
    unsigned int *rankPatterns = getPatternsForRank32(fileName, queriesNum, textLen);
    unsigned int *resRank = new unsigned int[queriesNumInt];
	
	unsigned int *selectPatterns = getPatternsForSelect32(fileName, queriesNum, text, textLen);
    unsigned int *resSelect = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) {
		if (prop[i] == 1) resRank[i] = rank->rank(rankPatterns[i]);
		else resSelect[i] = select->select(selectPatterns[i]);
	}
	timer.stopTimer();
	
	string resultFileName = "results/" + string(fileName) + "_mixed_rank_select.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = ((double)rank->getSize() + (double)select->getSize()) / (double)rank->getTextSize();
	cout << "mixed-rank-select-bch-32-128-4096 " << fileName << " " << propFileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " bch-32 128 4096 " << propFileName << " " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
	
    delete[] rankPatterns;
    delete[] resRank;
    delete rank;
	
	delete[] selectPatterns;
	delete[] resSelect;
    delete select;
    exit(0);
}

void rankSelectMPE2_32(const char *fileName, const char *propFileName, const char *queriesNum) {
	RankMPE32<RANK_MPE2> *rank = new RankMPE32<RANK_MPE2>();
	string rankFileNameString = (string)fileName + "-mpe2-32.rank";
	const char *rankFileName = rankFileNameString.c_str();
	
	SelectMPE32<SELECT_MPE2, 128, 4096> *select = new SelectMPE32<SELECT_MPE2, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe2-32-128-4096.select";
	const char *selectFileName = selectFileNameString.c_str();
	
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);
	unsigned int propLen;
	unsigned char* prop = readText(propFileName, propLen, 0);

	if (fileExists(rankFileName)) {
		FILE *inFile;
		inFile = fopen(rankFileName, "rb");
		rank->load(inFile);
		fclose(inFile);
	} else {
		rank->build(text, textLen);
		FILE *outFile;
		outFile = fopen(rankFileName, "w");
		rank->save(outFile);
		fclose(outFile);
	}
	
	if (fileExists(selectFileName)) {
		FILE *inFile;
		inFile = fopen(selectFileName, "rb");
		select->load(inFile);
		fclose(inFile);
	} else {
		select->build(text, textLen);
		FILE *outFile;
		outFile = fopen(selectFileName, "w");
		select->save(outFile);
		fclose(outFile);
	}
    
	unsigned int queriesNumInt = atoi(queriesNum);
	
    unsigned int *rankPatterns = getPatternsForRank32(fileName, queriesNum, textLen);
    unsigned int *resRank = new unsigned int[queriesNumInt];
	
	unsigned int *selectPatterns = getPatternsForSelect32(fileName, queriesNum, text, textLen);
    unsigned int *resSelect = new unsigned int[queriesNumInt];
    
    timer.startTimer();
	for (unsigned int i = 0; i < queriesNumInt; ++i) {
		if (prop[i] == 1) resRank[i] = rank->rank(rankPatterns[i]);
		else resSelect[i] = select->select(selectPatterns[i]);
	}
	
	
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(rankPatterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/" + string(fileName) + "_mixed_rank_select.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = ((double)rank->getSize() + (double)select->getSize()) / (double)rank->getTextSize();
	cout << "mixed-rank-select-mpe2-32-128-4096 " << fileName << " " << propFileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe2-32 128 4096 " << propFileName << " " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
	
    delete[] rankPatterns;
    delete[] resRank;
    delete rank;
	
	delete[] selectPatterns;
	delete[] resSelect;
    delete select;
    exit(0);
}