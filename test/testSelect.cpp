#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <random>
#include "../shared/common.h"
#include "../shared/patterns.h"
#include "../shared/timer.h"
#include "../shared/select.hpp"

using namespace std;
using namespace shared;

ChronoStopWatch timer;

void selectBasic_128_4096(const char *fileName, const char *queriesNum);
void selectBasic_1024_8192(const char *fileName, const char *queriesNum);
void selectBch_128_4096(const char *fileName, const char *queriesNum);
void selectBch_1024_8192(const char *fileName, const char *queriesNum);
void selectMpe1_128_4096(const char *fileName, const char *queriesNum);
void selectMpe2_128_4096(const char *fileName, const char *queriesNum);
void selectMpe3_128_4096(const char *fileName, const char *queriesNum);

void getUsage(char **argv) {
	cout << "Choose select you want to test:" << endl;
	cout << "select-basic-128-4096: " << argv[0] << " basic-128-4096 fileName queriesNum" << endl;
	cout << "select-basic-1024-8192: " << argv[0] << " basic-1024-8192 fileName queriesNum" << endl;
	cout << "select-bch-128-4096: " << argv[0] << " bch-128-4096 fileName queriesNum" << endl;
	cout << "select-bch-1024-8192: " << argv[0] << " bch-1024-8192 fileName queriesNum" << endl;
	cout << "select-mpe1-128-4096: " << argv[0] << " mpe1-128-4096 fileName queriesNum" << endl;
	cout << "select-mpe2-128-4096: " << argv[0] << " mpe2-128-4096 fileName queriesNum" << endl;
	cout << "select-mpe3-128-4096: " << argv[0] << " mpe3-128-4096 fileName queriesNum" << endl;
	cout << "where:" << endl;
	cout << "fileName - name of bits file" << endl;
	cout << "queriesNum - number of queries" << endl << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		getUsage(argv);
		exit(1);
	}
	if (string(argv[1]) == "basic-128-4096") selectBasic_128_4096(argv[2], argv[3]);
	if (string(argv[1]) == "basic-1024-8192") selectBasic_1024_8192(argv[2], argv[3]);
	if (string(argv[1]) == "bch-128-4096") selectBch_128_4096(argv[2], argv[3]);
	if (string(argv[1]) == "bch-1024-8192") selectBch_1024_8192(argv[2], argv[3]);
	if (string(argv[1]) == "mpe1-128-4096") selectMpe1_128_4096(argv[2], argv[3]);
	if (string(argv[1]) == "mpe2-128-4096") selectMpe2_128_4096(argv[2], argv[3]);
	if (string(argv[1]) == "mpe3-128-4096") selectMpe3_128_4096(argv[2], argv[3]);
	
	getUsage(argv);
	exit(1);
}

unsigned int *getPatternsForSelect(const char *fileName, const char *queriesNum, unsigned char *text, unsigned int textLen) {
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
		unsigned numberOfOnes = 0;
        for (unsigned int i = 0; i < textLen; ++i) numberOfOnes += __builtin_popcount(text[i]);
		random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<unsigned int> dis(1, numberOfOnes);
        for (unsigned long long i = 0; i < (unsigned long long)atoi(queriesNum); ++i) patSelects[i] = dis(gen);
        FILE *outFile;
		outFile = fopen(selectsFileName, "w");
		fwrite(patSelects, (size_t)sizeof(unsigned int), (size_t)atoi(queriesNum), outFile);
        fclose(outFile);
    }
	return patSelects;
}

void selectBasic_128_4096(const char *fileName, const char *queriesNum) {
	SelectBasic<SelectBasicType::STANDARD, 128, 4096> *select = new SelectBasic<SelectBasicType::STANDARD, 128, 4096>();
	string selectFileNameString = (string)fileName + "-basic-128-4096.select";
	const char *selectFileName = selectFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//select->testSelect(text, textLen);
    
    unsigned int *patterns = getPatternsForSelect(fileName, queriesNum, text, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resSelect = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resSelect[i] = select->select(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_select.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)select->getSize() / (double)select->getTextSize();
	cout << "select-basic-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "basic 128 4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBasic_1024_8192(const char *fileName, const char *queriesNum) {
	SelectBasic<SelectBasicType::STANDARD, 1024, 8192> *select = new SelectBasic<SelectBasicType::STANDARD, 1024, 8192>();
	string selectFileNameString = (string)fileName + "-basic-1024-8192.select";
	const char *selectFileName = selectFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//select->testSelect(text, textLen);
    
    unsigned int *patterns = getPatternsForSelect(fileName, queriesNum, text, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resSelect = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resSelect[i] = select->select(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_select.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)select->getSize() / (double)select->getTextSize();
	cout << "select-basic-1024-8192 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "basic 1024 8192 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBch_128_4096(const char *fileName, const char *queriesNum) {
	SelectBasic<SelectBasicType::WITH_COMPRESSED_HEADERS, 128, 4096> *select = new SelectBasic<SelectBasicType::WITH_COMPRESSED_HEADERS, 128, 4096>();
	string selectFileNameString = (string)fileName + "-bch-128-4096.select";
	const char *selectFileName = selectFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//select->testSelect(text, textLen);
    
    unsigned int *patterns = getPatternsForSelect(fileName, queriesNum, text, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resSelect = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resSelect[i] = select->select(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_select.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)select->getSize() / (double)select->getTextSize();
	cout << "select-bch-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "bch 128 4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBch_1024_8192(const char *fileName, const char *queriesNum) {
	SelectBasic<SelectBasicType::WITH_COMPRESSED_HEADERS, 1024, 8192> *select = new SelectBasic<SelectBasicType::WITH_COMPRESSED_HEADERS, 1024, 8192>();
	string selectFileNameString = (string)fileName + "-bch-1024-8192.select";
	const char *selectFileName = selectFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//select->testSelect(text, textLen);
    
    unsigned int *patterns = getPatternsForSelect(fileName, queriesNum, text, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resSelect = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resSelect[i] = select->select(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_select.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)select->getSize() / (double)select->getTextSize();
	cout << "select-bch-1024-8192 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "bch 1024 8192 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectMpe1_128_4096(const char *fileName, const char *queriesNum) {
	SelectMPE<SelectMPEType::V1, 128, 4096> *select = new SelectMPE<SelectMPEType::V1, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe1-128-4096.select";
	const char *selectFileName = selectFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//select->testSelect(text, textLen);
    
    unsigned int *patterns = getPatternsForSelect(fileName, queriesNum, text, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resSelect = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resSelect[i] = select->select(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_select.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)select->getSize() / (double)select->getTextSize();
	cout << "select-mpe1-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "mpe1 128 4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectMpe2_128_4096(const char *fileName, const char *queriesNum) {
	SelectMPE<SelectMPEType::V2, 128, 4096> *select = new SelectMPE<SelectMPEType::V2, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe2-128-4096.select";
	const char *selectFileName = selectFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//select->testSelect(text, textLen);
    
    unsigned int *patterns = getPatternsForSelect(fileName, queriesNum, text, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resSelect = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resSelect[i] = select->select(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_select.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)select->getSize() / (double)select->getTextSize();
	cout << "select-mpe2-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "mpe2 128 4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectMpe3_128_4096(const char *fileName, const char *queriesNum) {
	SelectMPE<SelectMPEType::V3, 128, 4096> *select = new SelectMPE<SelectMPEType::V3, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe3-128-4096.select";
	const char *selectFileName = selectFileNameString.c_str();
	unsigned int textLen;
	unsigned char* text = readText(fileName, textLen, 0);

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
	//select->testSelect(text, textLen);
    
    unsigned int *patterns = getPatternsForSelect(fileName, queriesNum, text, textLen);
	unsigned int queriesNumInt = atoi(queriesNum);
    unsigned int *resSelect = new unsigned int[queriesNumInt];
    
    timer.startTimer();
    for (unsigned int i = 0; i < queriesNumInt; ++i) resSelect[i] = select->select(patterns[i]);
    timer.stopTimer();
	
	string resultFileName = "results/rank-select/" + string(fileName) + "_select.txt";
	fstream resultFile(resultFileName.c_str(), ios::out | ios::binary | ios::app);
	double size = (double)select->getSize() / (double)select->getTextSize();
	cout << "select-mpe3-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << "mpe3 128 4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}
