#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <random>
#include "../shared/common.h"
#include "../shared/patterns.h"
#include "../shared/timer.h"
#include "../shared/rank.hpp"

using namespace std;
using namespace fmdummy;

ChronoStopWatch timer;

void rankBasic(const char *fileName, const char *queriesNum);
void rankBch(const char *fileName, const char *queriesNum);

void getUsage(char **argv) {
	cout << "Select rank you want to test:" << endl;
	cout << "rank-basic: " << argv[0] << " basic fileName queriesNum" << endl;
	cout << "rank-bch: " << argv[0] << " bch fileName queriesNum" << endl;
	cout << "where:" << endl;
	cout << "fileName - name of bits file" << endl;
	cout << "queriesNum - number of queries" << endl << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		getUsage(argv);
		exit(1);
	}
	if (string(argv[1]) == "basic") rankBasic(argv[2], argv[3]);
	if (string(argv[1]) == "bch") rankBch(argv[2], argv[3]);
	getUsage(argv);
	exit(1);
}

unsigned int *getPatternsForRank(const char *fileName, const char *queriesNum, unsigned int textLen) {
	string bitsFileNameString = "bits-" + string(fileName) + "-" + string(queriesNum) + ".dat";
    const char *bitsFileName = bitsFileNameString.c_str();
    
    unsigned int *patBits = new unsigned int[atoi(queriesNum)];
    
    if (fileExists(bitsFileName)) {
		FILE *inFile;
		inFile = fopen(bitsFileName, "rb");
		size_t result;
		result = fread(patBits, (size_t)sizeof(unsigned int), (size_t)atoi(queriesNum), inFile);
		if (result != (unsigned int)atoi(queriesNum)) {
			cout << "Error loading bits from " << bitsFileName << endl;
			exit(1);
		}
		fclose(inFile);
    } else {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<unsigned int> dis(1, 8 * textLen);
		for (unsigned long long i = 0; i < (unsigned long long)atoi(queriesNum); ++i) patBits[i] = dis(gen);
		FILE *outFile;
		outFile = fopen(bitsFileName, "w");
		fwrite(patBits, (size_t)sizeof(unsigned int), (size_t)atoi(queriesNum), outFile);
        fclose(outFile);
    }
	return patBits;
}

void rankBasic(const char *fileName, const char *queriesNum) {
	RankBasic<RankBasicType::STANDARD> *rb = new RankBasic<RankBasicType::STANDARD>();
	string rankFileNameString = (string)fileName + "-basic.rank";
	const char *rankFileName = rankFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

	if (fileExists(rankFileName)) {
		FILE *inFile;
		inFile = fopen(rankFileName, "rb");
		rb->load(inFile);
		fclose(inFile);
	} else {
		rb->build(text, textLen);
		FILE *outFile;
		outFile = fopen(rankFileName, "w");
		rb->save(outFile);
		fclose(outFile);
	}
	//rb->testRank(text, textLen);
    
    unsigned int *patterns = getPatternsForRank(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rb->getRank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rb->getRankSize() / (double)rb->getTextSize();
	cout << "rank-basic" << " " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "basic " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    if (text != NULL) delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rb;
    exit(0);
}

void rankBch(const char *fileName, const char *queriesNum) {
	RankBasic<RankBasicType::WITH_COMPRESSED_HEADERS> *rb = new RankBasic<RankBasicType::WITH_COMPRESSED_HEADERS>();
	string rankFileNameString = (string)fileName + "-bch.rank";
	const char *rankFileName = rankFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

	if (fileExists(rankFileName)) {
		FILE *inFile;
		inFile = fopen(rankFileName, "rb");
		rb->load(inFile);
		fclose(inFile);
	} else {
		rb->build(text, textLen);
		FILE *outFile;
		outFile = fopen(rankFileName, "w");
		rb->save(outFile);
		fclose(outFile);
	}
	//rb->testRank(text, textLen);
    
    unsigned int *patterns = getPatternsForRank(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rb->getRank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rb->getRankSize() / (double)rb->getTextSize();
	cout << "rank-bch" << " " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "bch " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    if (text != NULL) delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rb;
    exit(0);
}