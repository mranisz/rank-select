#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <random>
#include "../shared/common.hpp"
#include "../shared/patterns.hpp"
#include "../shared/timer.hpp"
#include "../shared/rank.hpp"

using namespace std;
using namespace shared;

ChronoStopWatch timer;

void rankBasic_32(const char *fileName, const char *queriesNum);
void rankBch_32(const char *fileName, const char *queriesNum);
void rankCF_32(const char *fileName, const char *queriesNum);
void rankMPE1_32(const char *fileName, const char *queriesNum);
void rankMPE2_32(const char *fileName, const char *queriesNum);
void rankMPE3_32(const char *fileName, const char *queriesNum);
void rankBasic_64(const char *fileName, const char *queriesNum);
void rankBch_64(const char *fileName, const char *queriesNum);
void rankCF_64(const char *fileName, const char *queriesNum);
void rankMPE1_64(const char *fileName, const char *queriesNum);
void rankMPE2_64(const char *fileName, const char *queriesNum);
void rankMPE3_64(const char *fileName, const char *queriesNum);

void getUsage(char **argv) {
	cout << "Select rank you want to test:" << endl;
	cout << "rank-basic: " << argv[0] << " basic version fileName queriesNum" << endl;
	cout << "rank-bch: " << argv[0] << " bch version fileName queriesNum" << endl;
	cout << "rank-cf: " << argv[0] << " cf version fileName queriesNum" << endl;
	cout << "rank-mpe1: " << argv[0] << " mpe1 version fileName queriesNum" << endl;
	cout << "rank-mpe2: " << argv[0] << " mpe2 version fileName queriesNum" << endl;
	cout << "rank-mpe3: " << argv[0] << " mpe3 version fileName queriesNum" << endl;
	cout << "where:" << endl;
	cout << "version - version of rank: 32|64 (32bit or 64bit)" << endl;
	cout << "fileName - name of bits file" << endl;
	cout << "queriesNum - number of queries" << endl << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 5) {
		getUsage(argv);
		exit(1);
	}
	
	if (string(argv[1]) == "basic") {
		if (string(argv[2]) == "32") rankBasic_32(argv[3], argv[4]);
		if (string(argv[2]) == "64") rankBasic_64(argv[3], argv[4]);
	}
	if (string(argv[1]) == "bch"){
		if (string(argv[2]) == "32") rankBch_32(argv[3], argv[4]);
		if (string(argv[2]) == "64") rankBch_64(argv[3], argv[4]);
	}
	if (string(argv[1]) == "cf") {
		if (string(argv[2]) == "32") rankCF_32(argv[3], argv[4]);
		if (string(argv[2]) == "64") rankCF_64(argv[3], argv[4]);
	}
	if (string(argv[1]) == "mpe1") {
		if (string(argv[2]) == "32") rankMPE1_32(argv[3], argv[4]);
		if (string(argv[2]) == "64") rankMPE1_64(argv[3], argv[4]);
	}
	if (string(argv[1]) == "mpe2") {
		if (string(argv[2]) == "32") rankMPE2_32(argv[3], argv[4]);
		if (string(argv[2]) == "64") rankMPE2_64(argv[3], argv[4]);
	}
	if (string(argv[1]) == "mpe3") {
		if (string(argv[2]) == "32") rankMPE3_32(argv[3], argv[4]);
		if (string(argv[2]) == "64") rankMPE3_64(argv[3], argv[4]);
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

void rankBasic_32(const char *fileName, const char *queriesNum) {
	RankBasic32<RANK_BASIC_STANDARD> *rank = new RankBasic32<RANK_BASIC_STANDARD>();
	string rankFileNameString = (string)fileName + "-basic-32.rank";
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
    
    unsigned int *patterns = getPatternsForRank32(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-basic-32 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " basic-32 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankBch_32(const char *fileName, const char *queriesNum) {
	RankBasic32<RANK_BASIC_COMPRESSED_HEADERS> *rank = new RankBasic32<RANK_BASIC_COMPRESSED_HEADERS>();
	string rankFileNameString = (string)fileName + "-bch-32.rank";
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
    
    unsigned int *patterns = getPatternsForRank32(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-bch-32 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " bch-32 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankCF_32(const char *fileName, const char *queriesNum) {
	RankCF32 *rank = new RankCF32();
	string rankFileNameString = (string)fileName + "-cf-32.rank";
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
    
    unsigned int *patterns = getPatternsForRank32(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-cf-32 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " cf-32 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankMPE1_32(const char *fileName, const char *queriesNum) {
	RankMPE32<RANK_MPE1> *rank = new RankMPE32<RANK_MPE1>();
	string rankFileNameString = (string)fileName + "-mpe1-32.rank";
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
    
    unsigned int *patterns = getPatternsForRank32(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-mpe1-32 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe1-32 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankMPE2_32(const char *fileName, const char *queriesNum) {
	RankMPE32<RANK_MPE2> *rank = new RankMPE32<RANK_MPE2>();
	string rankFileNameString = (string)fileName + "-mpe2-32.rank";
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
    
    unsigned int *patterns = getPatternsForRank32(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-mpe2-32 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe2-32 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankMPE3_32(const char *fileName, const char *queriesNum) {
	RankMPE32<RANK_MPE3> *rank = new RankMPE32<RANK_MPE3>();
	string rankFileNameString = (string)fileName + "-mpe3-32.rank";
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
    
    unsigned int *patterns = getPatternsForRank32(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resRank = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-mpe3-32 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe3-32 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankBasic_64(const char *fileName, const char *queriesNum) {
	RankBasic64<RANK_BASIC_STANDARD> *rank = new RankBasic64<RANK_BASIC_STANDARD>();
	string rankFileNameString = (string)fileName + "-basic-64.rank";
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
    
    unsigned long long *patterns = getPatternsForRank64(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned long long *resRank = new unsigned long long[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-basic-64 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " basic-64 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankBch_64(const char *fileName, const char *queriesNum) {
	RankBasic64<RANK_BASIC_COMPRESSED_HEADERS> *rank = new RankBasic64<RANK_BASIC_COMPRESSED_HEADERS>();
	string rankFileNameString = (string)fileName + "-bch-64.rank";
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
    
    unsigned long long *patterns = getPatternsForRank64(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned long long *resRank = new unsigned long long[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-bch-64 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " bch-64 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankCF_64(const char *fileName, const char *queriesNum) {
	RankCF64 *rank = new RankCF64();
	string rankFileNameString = (string)fileName + "-cf-64.rank";
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
    
    unsigned long long *patterns = getPatternsForRank64(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned long long *resRank = new unsigned long long[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-cf-64 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " cf-64 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankMPE1_64(const char *fileName, const char *queriesNum) {
	RankMPE64<RANK_MPE1> *rank = new RankMPE64<RANK_MPE1>();
	string rankFileNameString = (string)fileName + "-mpe1-64.rank";
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
    
    unsigned long long *patterns = getPatternsForRank64(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned long long *resRank = new unsigned long long[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-mpe1-64 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe1-64 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankMPE2_64(const char *fileName, const char *queriesNum) {
	RankMPE64<RANK_MPE2> *rank = new RankMPE64<RANK_MPE2>();
	string rankFileNameString = (string)fileName + "-mpe2-64.rank";
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
    
    unsigned long long *patterns = getPatternsForRank64(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned long long *resRank = new unsigned long long[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-mpe2-64 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe2-64 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}

void rankMPE3_64(const char *fileName, const char *queriesNum) {
	RankMPE64<RANK_MPE3> *rank = new RankMPE64<RANK_MPE3>();
	string rankFileNameString = (string)fileName + "-mpe3-64.rank";
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
    
    unsigned long long *patterns = getPatternsForRank64(fileName, queriesNum, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned long long *resRank = new unsigned long long[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resRank[i] = rank->rank(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_rank.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)rank->getSize() / (double)rank->getTextSize();
	cout << "rank-mpe3-64 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe3-64 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resRank;
    delete rank;
    exit(0);
}