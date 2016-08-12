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
void rankCF(const char *fileName, const char *queriesNum);
void rankMPE1(const char *fileName, const char *queriesNum);
void rankMPE2(const char *fileName, const char *queriesNum);
void rankMPE3(const char *fileName, const char *queriesNum);

void getUsage(char **argv) {
	cout << "Select rank you want to test:" << endl;
	cout << "rank-basic: " << argv[0] << " basic fileName queriesNum" << endl;
	cout << "rank-bch: " << argv[0] << " bch fileName queriesNum" << endl;
	cout << "rank-cf: " << argv[0] << " cf fileName queriesNum" << endl;
	cout << "rank-mpe1: " << argv[0] << " mpe1 fileName queriesNum" << endl;
	cout << "rank-mpe2: " << argv[0] << " mpe2 fileName queriesNum" << endl;
	cout << "rank-mpe3: " << argv[0] << " mpe3 fileName queriesNum" << endl;
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
	if (string(argv[1]) == "cf") rankCF(argv[2], argv[3]);
	if (string(argv[1]) == "mpe1") rankMPE1(argv[2], argv[3]);
	if (string(argv[1]) == "mpe2") rankMPE2(argv[2], argv[3]);
	if (string(argv[1]) == "mpe3") rankMPE3(argv[2], argv[3]);
	
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
	RankBasic<RankBasicType::STANDARD> *rank = new RankBasic<RankBasicType::STANDARD>();
	string rankFileNameString = (string)fileName + "-basic.rank";
	const char *rankFileName = rankFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//rank->testRank(text, textLen);
    
    unsigned int *patterns = getPatternsForRank(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->getRank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getRankSize() / (double)rank->getTextSize();
	cout << "rank-basic" << " " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "basic " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    if (text != NULL) delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankBch(const char *fileName, const char *queriesNum) {
	RankBasic<RankBasicType::WITH_COMPRESSED_HEADERS> *rank = new RankBasic<RankBasicType::WITH_COMPRESSED_HEADERS>();
	string rankFileNameString = (string)fileName + "-bch.rank";
	const char *rankFileName = rankFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//rank->testRank(text, textLen);
    
    unsigned int *patterns = getPatternsForRank(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->getRank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getRankSize() / (double)rank->getTextSize();
	cout << "rank-bch" << " " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "bch " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    if (text != NULL) delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankCF(const char *fileName, const char *queriesNum) {
	RankCF *rank = new RankCF();
	string rankFileNameString = (string)fileName + "-cf.rank";
	const char *rankFileName = rankFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//rank->testRank(text, textLen);
    
    unsigned int *patterns = getPatternsForRank(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->getRank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getRankSize() / (double)rank->getTextSize();
	cout << "rank-cf" << " " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "cf " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    if (text != NULL) delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankMPE1(const char *fileName, const char *queriesNum) {
	RankMPE<RankMPEType::V1> *rank = new RankMPE<RankMPEType::V1>();
	string rankFileNameString = (string)fileName + "-mpe1.rank";
	const char *rankFileName = rankFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//rank->testRank(text, textLen);
    
    unsigned int *patterns = getPatternsForRank(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->getRank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getRankSize() / (double)rank->getTextSize();
	cout << "rank-mpe1" << " " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "mpe1 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    if (text != NULL) delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankMPE2(const char *fileName, const char *queriesNum) {
	RankMPE<RankMPEType::V2> *rank = new RankMPE<RankMPEType::V2>();
	string rankFileNameString = (string)fileName + "-mpe2.rank";
	const char *rankFileName = rankFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//rank->testRank(text, textLen);
    
    unsigned int *patterns = getPatternsForRank(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->getRank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getRankSize() / (double)rank->getTextSize();
	cout << "rank-mpe2" << " " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "mpe2 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    if (text != NULL) delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankMPE3(const char *fileName, const char *queriesNum) {
	RankMPE<RankMPEType::V3> *rank = new RankMPE<RankMPEType::V3>();
	string rankFileNameString = (string)fileName + "-mpe3.rank";
	const char *rankFileName = rankFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//rank->testRank(text, textLen);
    
    unsigned int *patterns = getPatternsForRank(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->getRank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getRankSize() / (double)rank->getTextSize();
	cout << "rank-mpe3" << " " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "mpe3 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    if (text != NULL) delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}