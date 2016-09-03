#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "../shared/patterns.h"
#include "../shared/timer.h"
#include "../shared/fm.hpp"

using namespace std;
using namespace shared;

ChronoStopWatch timer;

void fmHWTBasic(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTBch(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTCF(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTMPE1(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTMPE2(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTMPE3(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTHashBasic(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTHashBch(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTHashCF(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTHashMPE1(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTHashMPE2(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWTHashMPE3(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);

void getUsage(char **argv) {
	cout << "Select index you want to test (count):" << endl;
	cout << "FMHWT-basic: " << argv[0] << " basic fileName patternNum patternLen" << endl;
	cout << "FMHWT-bch: " << argv[0] << " bch fileName patternNum patternLen" << endl;
	cout << "FMHWT-cf: " << argv[0] << " cf fileName patternNum patternLen" << endl;
	cout << "FMHWT-mpe1: " << argv[0] << " mpe1 fileName patternNum patternLen" << endl;
	cout << "FMHWT-mpe2: " << argv[0] << " mpe2 fileName patternNum patternLen" << endl;
	cout << "FMHWT-mpe3: " << argv[0] << " mpe3 fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-basic: " << argv[0] << " hash-basic k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-bch: " << argv[0] << " hash-bch k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-cf: " << argv[0] << " hash-cf k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-mpe1: " << argv[0] << " hash-mpe1 k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-mpe2: " << argv[0] << " hash-mpe2 k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-mpe3: " << argv[0] << " hash-mpe3 k loadFactor fileName patternNum patternLen" << endl;
	cout << "where:" << endl;
	cout << "fileName - name of text file" << endl;
	cout << "patternNum - number of patterns" << endl;
	cout << "patternLen - pattern length" << endl;
	cout << "k - suffix length to be hashed (k > 0)" << endl;
	cout << "loadFactor - load factor of hash table (range: (0.0, 1.0))" << endl << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 5) {
		getUsage(argv);
		exit(1);
	}
	if (string(argv[1]) == "basic") fmHWTBasic(argv[2], atoi(argv[3]), atoi(argv[4]));
	if (string(argv[1]) == "bch") fmHWTBch(argv[2], atoi(argv[3]), atoi(argv[4]));
	if (string(argv[1]) == "cf") fmHWTCF(argv[2], atoi(argv[3]), atoi(argv[4]));
	if (string(argv[1]) == "mpe1") fmHWTMPE1(argv[2], atoi(argv[3]), atoi(argv[4]));
	if (string(argv[1]) == "mpe2") fmHWTMPE2(argv[2], atoi(argv[3]), atoi(argv[4]));
	if (string(argv[1]) == "mpe3") fmHWTMPE3(argv[2], atoi(argv[3]), atoi(argv[4]));
	if (string(argv[1]) == "hash-basic") fmHWTHashBasic(string(argv[2]), string(argv[3]), argv[4], atoi(argv[5]), atoi(argv[6]));
	if (string(argv[1]) == "hash-bch") fmHWTHashBch(string(argv[2]), string(argv[3]), argv[4], atoi(argv[5]), atoi(argv[6]));
	if (string(argv[1]) == "hash-cf") fmHWTHashCF(string(argv[2]), string(argv[3]), argv[4], atoi(argv[5]), atoi(argv[6]));
	if (string(argv[1]) == "hash-mpe1") fmHWTHashMPE1(string(argv[2]), string(argv[3]), argv[4], atoi(argv[5]), atoi(argv[6]));
	if (string(argv[1]) == "hash-mpe2") fmHWTHashMPE2(string(argv[2]), string(argv[3]), argv[4], atoi(argv[5]), atoi(argv[6]));
	if (string(argv[1]) == "hash-mpe3") fmHWTHashMPE3(string(argv[2]), string(argv[3]), argv[4], atoi(argv[5]), atoi(argv[6]));
	getUsage(argv);
	exit(1);
}

void fmHWTBasic(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT<WT<RankBasic<RankBasicType::RANK_STANDARD>>> *fm = new FMHWT<WT<RankBasic<RankBasicType::RANK_STANDARD>>>();
	string indexFileNameString = "FMHWT-" + (string)textFileName + "-basic.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-basic " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " basic " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
	exit(0);
}

void fmHWTBch(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT<WT<RankBasic<RankBasicType::RANK_WITH_COMPRESSED_HEADERS>>> *fm = new FMHWT<WT<RankBasic<RankBasicType::RANK_WITH_COMPRESSED_HEADERS>>>();
	string indexFileNameString = "FMHWT-" + (string)textFileName + "-bch.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-bch " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " bch " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
	exit(0);
}

void fmHWTCF(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT<WT<RankCF>> *fm = new FMHWT<WT<RankCF>>();
	string indexFileNameString = "FMHWT-" + (string)textFileName + "-cf.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-cf " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " cf " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
	exit(0);
}

void fmHWTMPE1(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT<WT<RankMPE<RankMPEType::RANK_V1>>> *fm = new FMHWT<WT<RankMPE<RankMPEType::RANK_V1>>>();
	string indexFileNameString = "FMHWT-" + (string)textFileName + "-mpe1.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-mpe1 " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " mpe1 " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
	exit(0);
}

void fmHWTMPE2(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT<WT<RankMPE<RankMPEType::RANK_V2>>> *fm = new FMHWT<WT<RankMPE<RankMPEType::RANK_V2>>>();
	string indexFileNameString = "FMHWT-" + (string)textFileName + "-mpe2.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-mpe2 " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " mpe2 " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
	exit(0);
}

void fmHWTMPE3(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT<WT<RankMPE<RankMPEType::RANK_V3>>> *fm = new FMHWT<WT<RankMPE<RankMPEType::RANK_V3>>>();
	string indexFileNameString = "FMHWT-" + (string)textFileName + "-mpe3.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-mpe3 " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " mpe3 " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
	exit(0);
}

void fmHWTHashBasic(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash<WT<RankBasic<RankBasicType::RANK_STANDARD>>> *fm = new FMHWTHash<WT<RankBasic<RankBasicType::RANK_STANDARD>>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT-hash-basic-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-hash-basic-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " basic " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
    exit(0);
}

void fmHWTHashBch(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash<WT<RankBasic<RankBasicType::RANK_WITH_COMPRESSED_HEADERS>>> *fm = new FMHWTHash<WT<RankBasic<RankBasicType::RANK_WITH_COMPRESSED_HEADERS>>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT-hash-bch-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-hash-bch-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " bch " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
    exit(0);
}

void fmHWTHashCF(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash<WT<RankCF>> *fm = new FMHWTHash<WT<RankCF>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT-hash-cf-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-hash-cf-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " cf " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
    exit(0);
}

void fmHWTHashMPE1(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash<WT<RankMPE<RankMPEType::RANK_V1>>> *fm = new FMHWTHash<WT<RankMPE<RankMPEType::RANK_V1>>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT-hash-mpe1-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-hash-mpe1-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " mpe1 " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
    exit(0);
}

void fmHWTHashMPE2(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash<WT<RankMPE<RankMPEType::RANK_V2>>> *fm = new FMHWTHash<WT<RankMPE<RankMPEType::RANK_V2>>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT-hash-mpe2-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-hash-mpe2-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " mpe2 " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
    exit(0);
}

void fmHWTHashMPE3(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash<WT<RankMPE<RankMPEType::RANK_V3>>> *fm = new FMHWTHash<WT<RankMPE<RankMPEType::RANK_V3>>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT-hash-mpe3-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns *P = new Patterns(textFileName, queriesNum, m);
	//NegativePatterns *P = new NegativePatterns(textFileName, queriesNum, m);
	/*MaliciousPatterns *P = new MaliciousPatterns(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/rank-select/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT-hash-mpe3-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " mpe3 " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

	unsigned int differences = P->getErrorCountsNumber(indexCounts);
	if (differences > 0) {
		cout << "DIFFERENCES: " << differences << endl;
		resultFile << " DIFFERENCES: " << differences;
	} else {
		cout << "Differences: " << differences << endl;
	}
	resultFile << endl;
	resultFile.close();

	delete[] indexCounts;
	delete fm;
	delete P;
    exit(0);
}