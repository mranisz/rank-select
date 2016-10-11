#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <random>
#include "../shared/common.hpp"
#include "../shared/patterns.hpp"
#include "../shared/timer.hpp"
#include "../shared/select.hpp"

using namespace std;
using namespace shared;

ChronoStopWatch timer;

void selectBasic_32_128_4096(const char *fileName, const char *queriesNum);
void selectBasic_32_512_8192(const char *fileName, const char *queriesNum);
void selectBch_32_128_4096(const char *fileName, const char *queriesNum);
void selectBch_32_512_8192(const char *fileName, const char *queriesNum);
void selectMpe1_32_128_4096(const char *fileName, const char *queriesNum);
void selectMpe2_32_128_4096(const char *fileName, const char *queriesNum);
void selectMpe3_32_128_4096(const char *fileName, const char *queriesNum);
void selectBasic_64_128_4096(const char *fileName, const char *queriesNum);
void selectBasic_64_512_8192(const char *fileName, const char *queriesNum);
void selectBch_64_128_4096(const char *fileName, const char *queriesNum);
void selectBch_64_512_8192(const char *fileName, const char *queriesNum);
void selectMpe1_64_128_4096(const char *fileName, const char *queriesNum);
void selectMpe2_64_128_4096(const char *fileName, const char *queriesNum);
void selectMpe3_64_128_4096(const char *fileName, const char *queriesNum);

void getUsage(char **argv) {
	cout << "Choose select you want to test:" << endl;
	cout << "select-basic-128-4096: " << argv[0] << " basic-128-4096 version fileName queriesNum" << endl;
	cout << "select-basic-512-8192: " << argv[0] << " basic-512-8192 version fileName queriesNum" << endl;
	cout << "select-bch-128-4096: " << argv[0] << " bch-128-4096 version fileName queriesNum" << endl;
	cout << "select-bch-512-8192: " << argv[0] << " bch-512-8192 version fileName queriesNum" << endl;
	cout << "select-mpe1-128-4096: " << argv[0] << " mpe1-128-4096 version fileName queriesNum" << endl;
	cout << "select-mpe2-128-4096: " << argv[0] << " mpe2-128-4096 version fileName queriesNum" << endl;
	cout << "select-mpe3-128-4096: " << argv[0] << " mpe3-128-4096 version fileName queriesNum" << endl;
	cout << "where:" << endl;
	cout << "version - version of select: 32|64 (32bit or 64bit)" << endl;
	cout << "fileName - name of bits file" << endl;
	cout << "queriesNum - number of queries" << endl << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		getUsage(argv);
		exit(1);
	}
	if (string(argv[1]) == "basic-128-4096") {
		if (string(argv[2]) == "32") selectBasic_32_128_4096(argv[3], argv[4]);
		if (string(argv[2]) == "64") selectBasic_64_128_4096(argv[3], argv[4]);
	}
	if (string(argv[1]) == "basic-512-8192") {
		if (string(argv[2]) == "32") selectBasic_32_512_8192(argv[3], argv[4]);
		if (string(argv[2]) == "64") selectBasic_64_512_8192(argv[3], argv[4]);
	}
	if (string(argv[1]) == "bch-128-4096") {
		if (string(argv[2]) == "32") selectBch_32_128_4096(argv[3], argv[4]);
		if (string(argv[2]) == "64") selectBch_64_128_4096(argv[3], argv[4]);
	}
	if (string(argv[1]) == "bch-512-8192") {
		if (string(argv[2]) == "32") selectBch_32_512_8192(argv[3], argv[4]);
		if (string(argv[2]) == "64") selectBch_64_512_8192(argv[3], argv[4]);
	}
	if (string(argv[1]) == "mpe1-128-4096") {
		if (string(argv[2]) == "32") selectMpe1_32_128_4096(argv[3], argv[4]);
		if (string(argv[2]) == "64") selectMpe1_64_128_4096(argv[3], argv[4]);
	}
	if (string(argv[1]) == "mpe2-128-4096") {
		if (string(argv[2]) == "32") selectMpe2_32_128_4096(argv[3], argv[4]);
		if (string(argv[2]) == "64") selectMpe2_64_128_4096(argv[3], argv[4]);
	}
	if (string(argv[1]) == "mpe3-128-4096") {
		if (string(argv[2]) == "32") selectMpe3_32_128_4096(argv[3], argv[4]);
		if (string(argv[2]) == "64") selectMpe3_64_128_4096(argv[3], argv[4]);
	}
	
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

void selectBasic_32_128_4096(const char *fileName, const char *queriesNum) {
	SelectBasic32<SELECT_BASIC_STANDARD, 128, 4096> *select = new SelectBasic32<SELECT_BASIC_STANDARD, 128, 4096>();
	string selectFileNameString = (string)fileName + "-basic-32-128-4096.select";
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
	cout << "select-basic-32-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " basic-32 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBasic_32_512_8192(const char *fileName, const char *queriesNum) {
	SelectBasic32<SELECT_BASIC_STANDARD, 512, 8192> *select = new SelectBasic32<SELECT_BASIC_STANDARD, 512, 8192>();
	string selectFileNameString = (string)fileName + "-basic-32-512-8192.select";
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
	cout << "select-basic-32-512-8192 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " basic-32 512 8192 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBch_32_128_4096(const char *fileName, const char *queriesNum) {
	SelectBasic32<SELECT_BASIC_COMPRESSED_HEADERS, 128, 4096> *select = new SelectBasic32<SELECT_BASIC_COMPRESSED_HEADERS, 128, 4096>();
	string selectFileNameString = (string)fileName + "-bch-32-128-4096.select";
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
	cout << "select-bch-32-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " bch-32 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBch_32_512_8192(const char *fileName, const char *queriesNum) {
	SelectBasic32<SELECT_BASIC_COMPRESSED_HEADERS, 512, 8192> *select = new SelectBasic32<SELECT_BASIC_COMPRESSED_HEADERS, 512, 8192>();
	string selectFileNameString = (string)fileName + "-bch-32-512-8192.select";
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
	cout << "select-bch-32-512-8192 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " bch-32 512 8192 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectMpe1_32_128_4096(const char *fileName, const char *queriesNum) {
	SelectMPE32<SELECT_MPE1, 128, 4096> *select = new SelectMPE32<SELECT_MPE1, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe1-32-128-4096.select";
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
	cout << "select-mpe1-32-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe1-32 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectMpe2_32_128_4096(const char *fileName, const char *queriesNum) {
	SelectMPE32<SELECT_MPE2, 128, 4096> *select = new SelectMPE32<SELECT_MPE2, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe2-32-128-4096.select";
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
	cout << "select-mpe2-32-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe2-32 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectMpe3_32_128_4096(const char *fileName, const char *queriesNum) {
	SelectMPE32<SELECT_MPE3, 128, 4096> *select = new SelectMPE32<SELECT_MPE3, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe3-32-128-4096.select";
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
	cout << "select-mpe3-32-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe3-32 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBasic_64_128_4096(const char *fileName, const char *queriesNum) {
	SelectBasic64<SELECT_BASIC_STANDARD, 128, 4096> *select = new SelectBasic64<SELECT_BASIC_STANDARD, 128, 4096>();
	string selectFileNameString = (string)fileName + "-basic-64-128-4096.select";
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
	cout << "select-basic-64-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " basic-64 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBasic_64_512_8192(const char *fileName, const char *queriesNum) {
	SelectBasic64<SELECT_BASIC_STANDARD, 512, 8192> *select = new SelectBasic64<SELECT_BASIC_STANDARD, 512, 8192>();
	string selectFileNameString = (string)fileName + "-basic-64-512-8192.select";
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
	cout << "select-basic-64-512-8192 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " basic-64 512 8192 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBch_64_128_4096(const char *fileName, const char *queriesNum) {
	SelectBasic64<SELECT_BASIC_COMPRESSED_HEADERS, 128, 4096> *select = new SelectBasic64<SELECT_BASIC_COMPRESSED_HEADERS, 128, 4096>();
	string selectFileNameString = (string)fileName + "-bch-64-128-4096.select";
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
	cout << "select-bch-64-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " bch-64 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectBch_64_512_8192(const char *fileName, const char *queriesNum) {
	SelectBasic64<SELECT_BASIC_COMPRESSED_HEADERS, 512, 8192> *select = new SelectBasic64<SELECT_BASIC_COMPRESSED_HEADERS, 512, 8192>();
	string selectFileNameString = (string)fileName + "-bch-64-512-8192.select";
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
	cout << "select-bch-64-512-8192 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " bch-64 512 8192 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectMpe1_64_128_4096(const char *fileName, const char *queriesNum) {
	SelectMPE64<SELECT_MPE1, 128, 4096> *select = new SelectMPE64<SELECT_MPE1, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe1-64-128-4096.select";
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
	cout << "select-mpe1-64-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe1-64 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectMpe2_64_128_4096(const char *fileName, const char *queriesNum) {
	SelectMPE64<SELECT_MPE2, 128, 4096> *select = new SelectMPE64<SELECT_MPE2, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe2-64-128-4096.select";
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
	cout << "select-mpe2-64-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe2-64 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}

void selectMpe3_64_128_4096(const char *fileName, const char *queriesNum) {
	SelectMPE64<SELECT_MPE3, 128, 4096> *select = new SelectMPE64<SELECT_MPE3, 128, 4096>();
	string selectFileNameString = (string)fileName + "-mpe3-64-128-4096.select";
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
	cout << "select-mpe3-64-128-4096 " << fileName << " queries=" << queriesNum << " size=" << size << "n time=" << timer.getElapsedTime() << endl;
	resultFile << queriesNum << " mpe3-64 128 4096 " << size << " " << timer.getElapsedTime() << endl;
	resultFile.close();

    delete[] text;
    delete[] patterns;
    delete[] resSelect;
    delete select;
    exit(0);
}
