#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "../shared/common.hpp"
#include "../shared/patterns.hpp"
#include "../shared/timer.hpp"
#include "../shared/fm.hpp"

using namespace std;
using namespace shared;

ChronoStopWatch timer;

void fmHWT32Basic(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32Bch(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32CF(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32MPE1(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32MPE2(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32MPE3(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32HashBasic(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32HashBch(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32HashCF(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32HashMPE1(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32HashMPE2(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT32HashMPE3(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64Basic(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64Bch(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64CF(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64MPE1(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64MPE2(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64MPE3(const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64HashBasic(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64HashBch(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64HashCF(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64HashMPE1(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64HashMPE2(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);
void fmHWT64HashMPE3(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m);

void getUsage(char **argv) {
	cout << "Select index you want to test (count):" << endl;
	cout << "FMHWT-basic: " << argv[0] << " basic version fileName patternNum patternLen" << endl;
	cout << "FMHWT-bch: " << argv[0] << " bch version fileName patternNum patternLen" << endl;
	cout << "FMHWT-cf: " << argv[0] << " cf version fileName patternNum patternLen" << endl;
	cout << "FMHWT-mpe1: " << argv[0] << " mpe1 version fileName patternNum patternLen" << endl;
	cout << "FMHWT-mpe2: " << argv[0] << " mpe2 version fileName patternNum patternLen" << endl;
	cout << "FMHWT-mpe3: " << argv[0] << " mpe3 version fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-basic: " << argv[0] << " hash-basic version k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-bch: " << argv[0] << " hash-bch version k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-cf: " << argv[0] << " hash-cf version k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-mpe1: " << argv[0] << " hash-mpe1 version k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-mpe2: " << argv[0] << " hash-mpe2 version k loadFactor fileName patternNum patternLen" << endl;
	cout << "FMHWT-hash-mpe3: " << argv[0] << " hash-mpe3 version k loadFactor fileName patternNum patternLen" << endl;
	cout << "where:" << endl;
	cout << "version - version of FMHWT: 32|64 (32bit or 64bit)" << endl;
	cout << "fileName - name of text file" << endl;
	cout << "patternNum - number of patterns" << endl;
	cout << "patternLen - pattern length" << endl;
	cout << "k - suffix length to be hashed (k > 0)" << endl;
	cout << "loadFactor - load factor of hash table (range: (0.0, 1.0))" << endl << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 6) {
		getUsage(argv);
		exit(1);
	}
	if (string(argv[1]) == "basic") {
		if (string(argv[2]) == "32") fmHWT32Basic(argv[3], atoi(argv[4]), atoi(argv[5]));
		if (string(argv[2]) == "64") fmHWT64Basic(argv[3], atoi(argv[4]), atoi(argv[5]));
	}
	if (string(argv[1]) == "bch") {
		if (string(argv[2]) == "32") fmHWT32Bch(argv[3], atoi(argv[4]), atoi(argv[5]));
		if (string(argv[2]) == "64") fmHWT64Bch(argv[3], atoi(argv[4]), atoi(argv[5]));
	}
	if (string(argv[1]) == "cf") {
		if (string(argv[2]) == "32") fmHWT32CF(argv[3], atoi(argv[4]), atoi(argv[5]));
		if (string(argv[2]) == "64") fmHWT64CF(argv[3], atoi(argv[4]), atoi(argv[5]));
	}
	if (string(argv[1]) == "mpe1") {
		if (string(argv[2]) == "32") fmHWT32MPE1(argv[3], atoi(argv[4]), atoi(argv[5]));
		if (string(argv[2]) == "64") fmHWT64MPE1(argv[3], atoi(argv[4]), atoi(argv[5]));
	}
	if (string(argv[1]) == "mpe2") {
		if (string(argv[2]) == "32") fmHWT32MPE2(argv[3], atoi(argv[4]), atoi(argv[5]));
		if (string(argv[2]) == "64") fmHWT64MPE2(argv[3], atoi(argv[4]), atoi(argv[5]));
	}
	if (string(argv[1]) == "mpe3") {
		if (string(argv[2]) == "32") fmHWT32MPE3(argv[3], atoi(argv[4]), atoi(argv[5]));
		if (string(argv[2]) == "64") fmHWT64MPE3(argv[3], atoi(argv[4]), atoi(argv[5]));
	}
	if (string(argv[1]) == "hash-basic") {
		if (string(argv[2]) == "32") fmHWT32HashBasic(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
		if (string(argv[2]) == "64") fmHWT64HashBasic(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
	}
	if (string(argv[1]) == "hash-bch") {
		if (string(argv[2]) == "32") fmHWT32HashBch(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
		if (string(argv[2]) == "64") fmHWT64HashBch(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
	}
	if (string(argv[1]) == "hash-cf") {
		if (string(argv[2]) == "32") fmHWT32HashCF(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
		if (string(argv[2]) == "64") fmHWT64HashCF(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
	}
	if (string(argv[1]) == "hash-mpe1") {
		if (string(argv[2]) == "32") fmHWT32HashMPE1(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
		if (string(argv[2]) == "64") fmHWT64HashMPE1(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
	}
	if (string(argv[1]) == "hash-mpe2") {
		if (string(argv[2]) == "32") fmHWT32HashMPE2(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
		if (string(argv[2]) == "64") fmHWT64HashMPE2(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
	}
	if (string(argv[1]) == "hash-mpe3") {
		if (string(argv[2]) == "32") fmHWT32HashMPE3(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
		if (string(argv[2]) == "64") fmHWT64HashMPE3(string(argv[3]), string(argv[4]), argv[5], atoi(argv[6]), atoi(argv[7]));
	}
	getUsage(argv);
	exit(1);
}

void fmHWT32Basic(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT32<RankBasic32<RANK_BASIC_STANDARD>> *fm = new FMHWT32<RankBasic32<RANK_BASIC_STANDARD>>();
	string indexFileNameString = "FMHWT32-" + (string)textFileName + "-basic.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-basic " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-basic " << size << " " << timer.getElapsedTime();

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

void fmHWT32Bch(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT32<RankBasic32<RANK_BASIC_COMPRESSED_HEADERS>> *fm = new FMHWT32<RankBasic32<RANK_BASIC_COMPRESSED_HEADERS>>();
	string indexFileNameString = "FMHWT32-" + (string)textFileName + "-bch.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-bch " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-bch " << size << " " << timer.getElapsedTime();

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

void fmHWT32CF(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT32<RankCF32> *fm = new FMHWT32<RankCF32>();
	string indexFileNameString = "FMHWT32-" + (string)textFileName + "-cf.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-cf " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-cf " << size << " " << timer.getElapsedTime();

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

void fmHWT32MPE1(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT32<RankMPE32<RANK_MPE1>> *fm = new FMHWT32<RankMPE32<RANK_MPE1>>();
	string indexFileNameString = "FMHWT32-" + (string)textFileName + "-mpe1.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-mpe1 " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-mpe1 " << size << " " << timer.getElapsedTime();

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

void fmHWT32MPE2(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT32<RankMPE32<RANK_MPE2>> *fm = new FMHWT32<RankMPE32<RANK_MPE2>>();
	string indexFileNameString = "FMHWT32-" + (string)textFileName + "-mpe2.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-mpe2 " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-mpe2 " << size << " " << timer.getElapsedTime();

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

void fmHWT32MPE3(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT32<RankMPE32<RANK_MPE3>> *fm = new FMHWT32<RankMPE32<RANK_MPE3>>();
	string indexFileNameString = "FMHWT32-" + (string)textFileName + "-mpe3.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-mpe3 " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-mpe3 " << size << " " << timer.getElapsedTime();

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

void fmHWT32HashBasic(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash32<RankBasic32<RANK_BASIC_STANDARD>> *fm = new FMHWTHash32<RankBasic32<RANK_BASIC_STANDARD>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT32-hash-basic-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-hash-basic-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-basic " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT32HashBch(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash32<RankBasic32<RANK_BASIC_COMPRESSED_HEADERS>> *fm = new FMHWTHash32<RankBasic32<RANK_BASIC_COMPRESSED_HEADERS>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT32-hash-bch-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-hash-bch-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-bch " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT32HashCF(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash32<RankCF32> *fm = new FMHWTHash32<RankCF32>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT32-hash-cf-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-hash-cf-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-cf " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT32HashMPE1(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash32<RankMPE32<RANK_MPE1>> *fm = new FMHWTHash32<RankMPE32<RANK_MPE1>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT32-hash-mpe1-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-hash-mpe1-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-mpe1 " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT32HashMPE2(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash32<RankMPE32<RANK_MPE2>> *fm = new FMHWTHash32<RankMPE32<RANK_MPE2>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT32-hash-mpe2-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-hash-mpe2-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-mpe2 " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT32HashMPE3(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash32<RankMPE32<RANK_MPE3>> *fm = new FMHWTHash32<RankMPE32<RANK_MPE3>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT32-hash-mpe3-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns32 *P = new Patterns32(textFileName, queriesNum, m);
	//NegativePatterns32 *P = new NegativePatterns32(textFileName, queriesNum, m);
	/*MaliciousPatterns32 *P = new MaliciousPatterns32(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned int *indexCounts = new unsigned int[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT32-hash-mpe3-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 32-mpe3 " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT64Basic(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT64<RankBasic64<RANK_BASIC_STANDARD>> *fm = new FMHWT64<RankBasic64<RANK_BASIC_STANDARD>>();
	string indexFileNameString = "FMHWT64-" + (string)textFileName + "-basic.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-basic " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-basic " << size << " " << timer.getElapsedTime();

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

void fmHWT64Bch(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT64<RankBasic64<RANK_BASIC_COMPRESSED_HEADERS>> *fm = new FMHWT64<RankBasic64<RANK_BASIC_COMPRESSED_HEADERS>>();
	string indexFileNameString = "FMHWT64-" + (string)textFileName + "-bch.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-bch " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-bch " << size << " " << timer.getElapsedTime();

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

void fmHWT64CF(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT64<RankCF64> *fm = new FMHWT64<RankCF64>();
	string indexFileNameString = "FMHWT64-" + (string)textFileName + "-cf.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-cf " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-cf " << size << " " << timer.getElapsedTime();

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

void fmHWT64MPE1(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT64<RankMPE64<RANK_MPE1>> *fm = new FMHWT64<RankMPE64<RANK_MPE1>>();
	string indexFileNameString = "FMHWT64-" + (string)textFileName + "-mpe1.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-mpe1 " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-mpe1 " << size << " " << timer.getElapsedTime();

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

void fmHWT64MPE2(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT64<RankMPE64<RANK_MPE2>> *fm = new FMHWT64<RankMPE64<RANK_MPE2>>();
	string indexFileNameString = "FMHWT64-" + (string)textFileName + "-mpe2.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-mpe2 " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-mpe2 " << size << " " << timer.getElapsedTime();

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

void fmHWT64MPE3(const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWT64<RankMPE64<RANK_MPE3>> *fm = new FMHWT64<RankMPE64<RANK_MPE3>>();
	string indexFileNameString = "FMHWT64-" + (string)textFileName + "-mpe3.idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-mpe3 " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-mpe3 " << size << " " << timer.getElapsedTime();

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

void fmHWT64HashBasic(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash64<RankBasic64<RANK_BASIC_STANDARD>> *fm = new FMHWTHash64<RankBasic64<RANK_BASIC_STANDARD>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT64-hash-basic-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-hash-basic-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-basic " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT64HashBch(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash64<RankBasic64<RANK_BASIC_COMPRESSED_HEADERS>> *fm = new FMHWTHash64<RankBasic64<RANK_BASIC_COMPRESSED_HEADERS>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT64-hash-bch-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-hash-bch-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-bch " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT64HashCF(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash64<RankCF64> *fm = new FMHWTHash64<RankCF64>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT64-hash-cf-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-hash-cf-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-cf " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT64HashMPE1(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash64<RankMPE64<RANK_MPE1>> *fm = new FMHWTHash64<RankMPE64<RANK_MPE1>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT64-hash-mpe1-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-hash-mpe1-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-mpe1 " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT64HashMPE2(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash64<RankMPE64<RANK_MPE2>> *fm = new FMHWTHash64<RankMPE64<RANK_MPE2>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT64-hash-mpe2-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-hash-mpe2-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-mpe2 " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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

void fmHWT64HashMPE3(string k, string loadFactor, const char *textFileName, unsigned int queriesNum, unsigned int m) {
	FMHWTHash64<RankMPE64<RANK_MPE3>> *fm = new FMHWTHash64<RankMPE64<RANK_MPE3>>(atoi(k.c_str()), atof(loadFactor.c_str()));
	string indexFileNameString = "FMHWT64-hash-mpe3-" + (string)textFileName + "-" +  k + "-" + loadFactor + ".idx";
	char *indexFileName = (char *)indexFileNameString.c_str();

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	Patterns64 *P = new Patterns64(textFileName, queriesNum, m);
	//NegativePatterns64 *P = new NegativePatterns64(textFileName, queriesNum, m);
	/*MaliciousPatterns64 *P = new MaliciousPatterns64(textFileName, m);
	queriesNum = P->getQueriesNum();
	if (queriesNum == 0) exit(1);*/
	unsigned char **patterns = P->getPatterns();
	unsigned long long *indexCounts = new unsigned long long[queriesNum];

	timer.startTimer();
	for (unsigned int i = 0; i < queriesNum; ++i) {
		indexCounts[i] = fm->count(patterns[i], m);
	}
	timer.stopTimer();

	string resultFileName = "results/" + string(textFileName) + "_count_FMHWT-hash.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)fm->getIndexSize() / (double)fm->getTextSize();
	cout << "count FMHWT64-hash-mpe3-" << k << "-" << loadFactor << " " << textFileName << " m=" << m << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << m << " " << queriesNum << " 64-mpe3 " << k << " " << loadFactor << " " << size << " " << timer.getElapsedTime();

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