#ifndef PATTERNS_HPP
#define	PATTERNS_HPP

#include <vector>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <unordered_set>
#include <algorithm>
#include "common.hpp"

using namespace std;

namespace shared {

template<typename DATATYPE> class Patterns {
private:
	const char *textFileName;
	unsigned int queriesNum;
	unsigned int m;
	vector<unsigned char> selectedChars;
	unsigned char **patterns = NULL;
	DATATYPE *counts = NULL;
        vector<DATATYPE> *locates = NULL;

	void freeMemory() {
            if (this->patterns != NULL) {
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                            delete[] this->patterns[i];
                    }
                    delete[] this->patterns;
            }
            if (this->counts != NULL) delete[] this->counts;
            if (this->locates != NULL) delete[] this->locates;
        }
        
        void setQueriesNum(unsigned int queriesNum) {
            if (queriesNum == 0) {
                    cout << "Error: not valid queriesNum value" << endl;
                    exit(1);
            }
            this->queriesNum = queriesNum;
        }
        
        void setM(unsigned int m) {
            if (m == 0) {
                    cout << "Error: not valid m value" << endl;
                    exit(1);
            }
            this->m = m;
        }
        
        void setSelectedChars(vector<unsigned char> selectedChars) {
            this->selectedChars = selectedChars;
        }
        
	void initializePatterns() {
            int bitType = sizeof(DATATYPE) * 8;
            DATATYPE textLen, queriesFirstIndexArrayLen;
            unsigned char *text = readFileChar(this->textFileName, textLen, (DATATYPE)0);
            if (textLen < this->m) {
                    cout << "Error: text shorter than pattern length" << endl;
                    exit(1);
            }
            DATATYPE *queriesFirstIndexArray;
            stringstream ss;
            ss << "patterns" << bitType << "-" << this->textFileName << "-" << this->m << "-" << this->queriesNum << "-" << getStringFromSelectedChars(this->selectedChars, ".") << ".dat";
            string s = ss.str();
            char *patternFileName = (char *)(s.c_str());

            if (!fileExists(patternFileName)) {
                    cout << "Generating " << this->queriesNum << " patterns of length " << this->m << " from " << this->textFileName;
                    if (this->selectedChars.size() != 0) {
                            cout << ", alphabet (ordinal): {" << getStringFromSelectedChars(this->selectedChars, ", ") << "}";
                    }
                    cout << " ... " << flush;

                    random_device rd;
                    mt19937 gen(rd());
                    uniform_int_distribution<DATATYPE> dis(0, textLen - this->m);

                    queriesFirstIndexArray = new DATATYPE[this->queriesNum];

                    DATATYPE genVal;

                    if (this->selectedChars.size() != 0) {
                            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                    genVal = dis(gen);
                                    queriesFirstIndexArray[i] = genVal;
                                    for (unsigned int j = 0; j < this->m; ++j) {
                                            bool inSigma = false;
                                            for (vector<unsigned char>::iterator it = this->selectedChars.begin(); it != this->selectedChars.end(); ++it) {
                                                    if (text[j + genVal] == (*it)) {
                                                            inSigma = true;
                                                            break;
                                                    }
                                            }
                                            if (!inSigma) {
                                                    --i;
                                                    break;
                                            }
                                    }
                            }
                    } else {
                            for (unsigned int i = 0; i < this->queriesNum; ++i) queriesFirstIndexArray[i] = dis(gen);
                    }
                    cout << "Done" << endl;
                    cout << "Saving patterns in " << patternFileName << " ... " << flush;
                    FILE *outFile;
                    outFile = fopen(patternFileName, "w");
                    fwrite(queriesFirstIndexArray, (size_t)(sizeof(DATATYPE)), (size_t)(this->queriesNum), outFile);
                    fclose(outFile);
                    cout << "Done" << endl;
            } else {
                    cout << "Loading patterns from " << patternFileName << " ... " << flush;
                    queriesFirstIndexArray = readFile(patternFileName, queriesFirstIndexArrayLen, (DATATYPE)0);
                    cout << "Done" << endl;
            }
            this->patterns = new unsigned char *[this->queriesNum];
            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                    this->patterns[i] = new unsigned char[this->m + 1];
                    this->patterns[i][this->m] = '\0';
                    for (unsigned int j = 0; j < this->m; ++j) {
                            this->patterns[i][j] = text[queriesFirstIndexArray[i] + j];
                    }
            }

            delete[] queriesFirstIndexArray;
            delete[] text;
        }
        
	void initializeSACounts() {
            int bitType = sizeof(DATATYPE) * 8;
            if (this->patterns == NULL) this->initializePatterns();
            stringstream ss;
            ss << "counts" << bitType << "-" << this->textFileName << "-" << this->m << "-" << this->queriesNum << "-" << getStringFromSelectedChars(this->selectedChars, ".") << ".dat";
            string s = ss.str();
            char *countsFileName = (char *)(s.c_str());

            if (!fileExists(countsFileName)) {
                    DATATYPE textLen;
                    unsigned char *text = readFileChar(this->textFileName, textLen, (DATATYPE)0);
                    DATATYPE saLen;
                    DATATYPE *sa = getSA(this->textFileName, text, textLen, saLen, (DATATYPE)0);

                    cout << "Getting counts from SA ... " << flush;
                    this->counts = new DATATYPE[this->queriesNum];
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                            this->counts[i] = getSACount(sa, text, saLen, this->patterns[i], this->m);
                    }
                    delete[] text;
                    delete[] sa;
                    cout << "Done" << endl;
                    cout << "Saving counts in " << countsFileName << " ... " << flush;
                    FILE *outFile;
                    outFile = fopen(countsFileName, "w");
                    fwrite(this->counts, (size_t)(sizeof(DATATYPE)), (size_t)(this->queriesNum), outFile);
                    fclose(outFile);
                    cout << "Done" << endl;

            } else {
                    cout << "Loading counts from " << countsFileName << " ... " << flush;
                    DATATYPE countsLen;
                    this->counts = readFile(countsFileName, countsLen, (DATATYPE)0);
                    cout << "Done" << endl;
            }
        }
        
        void initializeSALocates() {
            int bitType = sizeof(DATATYPE) * 8;
            if (this->patterns == NULL) this->initializePatterns();
            stringstream ss;
            ss << "locates" << bitType << "-" << this->textFileName << "-" << this->m << "-" << this->queriesNum << "-" << getStringFromSelectedChars(this->selectedChars, ".") << ".dat";
            string s = ss.str();
            char *locatesFileName = (char *)(s.c_str());

            if (!fileExists(locatesFileName)) {
                    DATATYPE textLen;
                    unsigned char *text = readFileChar(this->textFileName, textLen, (DATATYPE)0);
                    DATATYPE saLen;
                    DATATYPE *sa = getSA(this->textFileName, text, textLen, saLen, (DATATYPE)0);

                    unsigned long long counter = 0;
                    cout << "Getting locates from SA ... " << flush;
                    this->locates = new vector<unsigned int>[this->queriesNum];
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                            getSALocate(sa, text, saLen, this->patterns[i], this->m, this->locates[i]);
                            counter += (this->locates[i].size() + 1);
                    }
                    delete[] text;
                    delete[] sa;
                    cout << "Done" << endl;

                    if (counter * sizeof(DATATYPE) > 1 * 1024 * 1024 * 1024) return;

                    cout << "Saving locates in " << locatesFileName << " ... " << flush;
                    FILE *outFile;
                    outFile = fopen(locatesFileName, "w");
                    DATATYPE locateSize;
                    typename vector<DATATYPE>::iterator it;
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                            locateSize = this->locates[i].size();
                            fwrite(&locateSize, (size_t)(sizeof(DATATYPE)), (size_t)1, outFile);
                            for (it = this->locates[i].begin(); it != this->locates[i].end(); ++it) {
                                    fwrite(&(*it), (size_t)(sizeof(DATATYPE)), (size_t)1, outFile);
                            }
                    }
                    fclose(outFile);
                    cout << "Done" << endl;

            } else {
                    cout << "Loading locates from " << locatesFileName << " ... " << flush;
                    size_t result;
                    DATATYPE locateSize, locateValue;
                    FILE *inFile;
                    inFile = fopen(locatesFileName, "rb");
                    this->locates = new vector<DATATYPE>[this->queriesNum];
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                        result = fread(&locateSize, (size_t)(sizeof(DATATYPE)), (size_t)1, inFile);
                        if (result != 1) {
                                cout << "Error reading file " << locatesFileName << endl;
                                exit(1);
                        }
                        for (DATATYPE j = 0; j < locateSize; ++j) {
                                result = fread(&locateValue, (size_t)(sizeof(DATATYPE)), (size_t)1, inFile);
                                if (result != 1) {
                                        cout << "Error reading file " << locatesFileName << endl;
                                        exit(1);
                                }
                                this->locates[i].push_back(locateValue);
                        }
                    }
                    fclose(inFile);
                    cout << "Done" << endl;
            }
        }

public:
	Patterns(const char *textFileName, unsigned int queriesNum, unsigned int m, vector<unsigned char> selectedChars = {}) {
            this->textFileName = textFileName;
            this->setQueriesNum(queriesNum);
            this->setM(m);
            this->setSelectedChars(selectedChars);
	}
        
	~Patterns() {
            this->freeMemory();
	}
        
	unsigned char **getPatterns() {
            if (this->patterns == NULL) this->initializePatterns();
            return this->patterns;
        }
        
	unsigned int *getSACounts() {
            if (this->counts == NULL) this->initializeSACounts();
            return this->counts;
        }
        
        vector<unsigned int> *getSALocates() {
            if (this->locates == NULL) this->initializeSALocates();
            return this->locates;
        }
        
	unsigned int getErrorCountsNumber(DATATYPE *countsToCheck) {
            if (this->counts == NULL) this->initializeSACounts();
            cout << "Checking counts consistency ... " << flush;
            unsigned int errorCountsNumber = 0;
            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                    if (countsToCheck[i] != this->counts[i]) ++errorCountsNumber;
            }
            cout << "Done" << endl;
            return errorCountsNumber;
        }
        
        unsigned int getErrorLocatesNumber(vector<DATATYPE> *locatesToCheck) {
            if (this->locates == NULL) this->initializeSALocates();
            cout << "Checking locates consistency ... " << flush;
            unsigned int errorLocatesNumber = 0;
            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                    if (locatesToCheck[i].size() != this->locates[i].size()) ++errorLocatesNumber;
                    else {
                        unordered_set<DATATYPE> set1(this->locates[i].begin(), this->locates[i].end());
                        unordered_set<DATATYPE> set2(locatesToCheck[i].begin(), locatesToCheck[i].end());
                        if (set1 != set2) ++errorLocatesNumber;
                    }
            }
            cout << "Done" << endl;
            return errorLocatesNumber;
        }
};

typedef Patterns<unsigned int> Patterns32;
typedef Patterns<unsigned long long> Patterns64;

template<typename DATATYPE> class NegativePatterns {
private:
	const char *textFileName;
	unsigned int queriesNum;
	unsigned int m;
	unsigned char **patterns = NULL;

	void freeMemory() {
            if (this->patterns != NULL) {
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                            delete[] this->patterns[i];
                    }
                    delete[] this->patterns;
            }
        }
        
        void setQueriesNum(unsigned int queriesNum) {
            if (queriesNum == 0) {
                    cout << "Error: not valid queriesNum value" << endl;
                    exit(1);
            }
            this->queriesNum = queriesNum;
        }
        
        void setM(unsigned int m) {
            if (m == 0) {
                    cout << "Error: not valid m value" << endl;
                    exit(1);
            }
            this->m = m;
        }
        
	void initializePatterns() {
            int bitType = sizeof(DATATYPE) * 8;
            DATATYPE textLen;
            unsigned char *text = readFileChar(this->textFileName, textLen, (DATATYPE)0);
            if (textLen < this->m) {
                    cout << "Error: text shorter than pattern length" << endl;
                    exit(1);
            }
            stringstream ss;
            ss << "negative-patterns" << bitType << "-" << this->textFileName << "-" << this->m << "-" << this->queriesNum << ".dat";
            string s = ss.str();
            char *patternFileName = (char *)(s.c_str());

            if (!fileExists(patternFileName)) {
                    DATATYPE saLen;
                    DATATYPE *sa = getSA(this->textFileName, text, textLen, saLen, (DATATYPE)0);

                    cout << "Generating " << this->queriesNum << " negative patterns of length " << this->m << " from " << this->textFileName << " ... " << flush;

                    random_device rd;
                    mt19937 gen(rd());
                    uniform_int_distribution<DATATYPE> dis(this->m - 1, textLen - 1);
                    uniform_int_distribution<unsigned int> disChars(1, 255);
                    uniform_int_distribution<unsigned int> disM(0, this->m - 1);
                    uniform_int_distribution<unsigned int> disChoice(0, 2);

                    this->patterns = new unsigned char *[this->queriesNum];

                    unsigned int genCounter = 0;
                    unsigned int replaceCharsNum, selectedPosition;
                    bool *selectedPos = new bool[this->m];
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                            this->patterns[i] = new unsigned char[this->m + 1];
                            this->patterns[i][this->m] = '\0';
                            genCounter = 0;
                            if (disChoice(gen) > 0) {
                                    while(true) {
                                            if (genCounter < 10) {
                                                    for (unsigned int j = 0; j < this->m; ++j) selectedPos[j] = false;
                                                    for (unsigned int j = 0; j < this->m; ++j) this->patterns[i][j] = text[dis(gen) - j];
                                                    replaceCharsNum = disM(gen);
                                                    for (unsigned int j = 0; j < replaceCharsNum; ++j) {
                                                            while(true) {
                                                                    selectedPosition = disM(gen);
                                                                    if (!selectedPos[selectedPosition]) {
                                                                            selectedPos[selectedPosition] = true;
                                                                            this->patterns[i][selectedPosition] = (unsigned char)disChars(gen);
                                                                            break;
                                                                    }
                                                            }
                                                    }
                                            }
                                            else if (genCounter < 1000) for (unsigned int j = 0; j < this->m; ++j) this->patterns[i][j] = (unsigned char)disChars(gen);
                                            else {
                                                    cout << "Error: problem with generating negative patterns" << endl;
                                                    exit(1);
                                            }
                                            if (this->getSACount(sa, text, saLen, this->patterns[i], this->m) == 0) break;
                                            ++genCounter;
                                    }
                            } else {
                                    while(true) {
                                            if (genCounter < 1000) for (unsigned int j = 0; j < this->m; ++j) this->patterns[i][j] = (unsigned char)disChars(gen);
                                            else {
                                                    cout << "Error: problem with generating negative patterns" << endl;
                                                    exit(1);
                                            }
                                            if (this->getSACount(sa, text, saLen, this->patterns[i], this->m) == 0) break;
                                            ++genCounter;
                                    }
                            }
                    }

                    delete[] selectedPos;
                    delete[] sa;
                    delete[] text;

                    cout << "Done" << endl;
                    cout << "Saving patterns in " << patternFileName << " ... " << flush;
                    FILE *outFile;
                    outFile = fopen(patternFileName, "w");
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                            fwrite(this->patterns[i], (size_t)(sizeof(unsigned char)), (size_t)(this->m), outFile);
                    }
                    fclose(outFile);
                    cout << "Done" << endl;
            } else {
                    cout << "Loading patterns from " << patternFileName << " ... " << flush;
                    this->patterns = readFilePatterns(patternFileName, this->m, this->queriesNum);
                    cout << "Done" << endl;
            }
        }
        
        DATATYPE getSACount(DATATYPE *sa, unsigned char *text, DATATYPE saLen, unsigned char *pattern, int patternLength) {
            DATATYPE beg = 0, end = 0;
            binarySearch(sa, text, (DATATYPE)0, saLen, pattern, patternLength, beg, end);
            return end - beg;
        }

public:
	NegativePatterns(const char *textFileName, unsigned int queriesNum, unsigned int m) {
            this->textFileName = textFileName;
            this->setQueriesNum(queriesNum);
            this->setM(m);
	}
        
	~NegativePatterns() {
            this->freeMemory();
	}
        
	unsigned char **getPatterns() {
            if (this->patterns == NULL) this->initializePatterns();
            return this->patterns;
        }
        
	unsigned int getErrorCountsNumber(DATATYPE *countsToCheck) {
            cout << "Checking counts consistency ... " << flush;
            unsigned int errorCountsNumber = 0;
            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                    if (countsToCheck[i] != 0) ++errorCountsNumber;
            }
            cout << "Done" << endl;
            return errorCountsNumber;
        }
        
        unsigned int getErrorLocatesNumber(vector<DATATYPE> *locatesToCheck) {
            cout << "Checking locates consistency ... " << flush;
            unsigned int errorLocatesNumber = 0;
            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                    if (locatesToCheck[i].size() != 0) ++errorLocatesNumber;
            }
            cout << "Done" << endl;
            return errorLocatesNumber;
        }
};

typedef NegativePatterns<unsigned int> NegativePatterns32;
typedef NegativePatterns<unsigned long long> NegativePatterns64;

template<typename DATATYPE> class MaliciousPatterns {
private:
	const char *textFileName;
	unsigned int queriesNum;
	unsigned int m;
	vector<unsigned char> selectedChars;
        bool initialized = false;
	unsigned char **patterns = NULL;
	DATATYPE *counts = NULL;
        vector<DATATYPE> *locates = NULL;

	void freeMemory() {
            if (this->patterns != NULL) {
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                            delete[] this->patterns[i];
                    }
                    delete[] this->patterns;
            }
            if (this->counts != NULL) delete[] this->counts;
            if (this->locates != NULL) delete[] this->locates;
            this->initialized = false;
        }
        
        void setM(unsigned int m) {
            if (m == 0) {
                    cout << "Error: not valid m value" << endl;
                    exit(1);
            }
            this->m = m;
        }
        
        void setSelectedChars(vector<unsigned char> selectedChars) {
            this->selectedChars = selectedChars;
        }
        
	void initializePatterns() {
            int bitType = sizeof(DATATYPE) * 8;
            DATATYPE textLen;
            unsigned char *text = readFileChar(this->textFileName, textLen, (DATATYPE)0);
            if (textLen < this->m) {
                    cout << "Error: text shorter than pattern length" << endl;
                    exit(1);
            }

            stringstream ss;
            ss << "malicious-patterns" << bitType << "-" << this->textFileName << "-" << this->m << "-" << getStringFromSelectedChars(this->selectedChars, ".") << ".dat";
            string s = ss.str();
            char *patternFileName = (char *)(s.c_str());

            if (!fileExists(patternFileName)) {
                    DATATYPE saLen;
                    DATATYPE *sa = getSA(this->textFileName, text, textLen, saLen, (DATATYPE)0);

                    cout << "Getting malicious patterns of length " << this->m << " from " << this->textFileName;
                    if (this->selectedChars.size() != 0) {
                            cout << ", alphabet (ordinal): {" << getStringFromSelectedChars(this->selectedChars, ", ") << "}";
                    }
                    cout << " ... " << flush;

                    unsigned int maliciousPatternsNum = 4;
                    DATATYPE queriesFirstIndexArray[maliciousPatternsNum] = {0, textLen - this->m, sa[1], sa[saLen - 1]};

                    if (this->selectedChars.size() != 0) {
                            this->queriesNum = 0;
                            for (unsigned int i = 0; i < maliciousPatternsNum; ++i) {
                                    queriesFirstIndexArray[(this->queriesNum)++] = queriesFirstIndexArray[i];
                                    for (unsigned int j = 0; j < this->m; ++j) {
                                            bool inSigma = false;
                                            for (vector<unsigned char>::iterator it = this->selectedChars.begin(); it != this->selectedChars.end(); ++it) {
                                                    if (text[j + queriesFirstIndexArray[i]] == (*it)) {
                                                            inSigma = true;
                                                            break;
                                                    }
                                            }
                                            if (!inSigma) {
                                                    --(this->queriesNum);
                                                    break;
                                            }
                                    }
                            }
                    } else {
                            this->queriesNum = maliciousPatternsNum;
                    }
                    unsigned char *maliciousPattern255 = new unsigned char[this->m + 1];
                    maliciousPattern255[this->m] = '\0';
                    for (unsigned int i = 0; i < this->m; ++i) maliciousPattern255[i] = (unsigned char)255;
                    bool addMaliciousPattern255 = true;
                    if (this->selectedChars.size() != 0 && getSACount(sa, text, saLen, maliciousPattern255, this->m) > 0) {
                            addMaliciousPattern255 = false;
                            for (vector<unsigned char>::iterator it = this->selectedChars.begin(); it != this->selectedChars.end(); ++it) {
                                    if ((unsigned char)255 == (*it)) {
                                            addMaliciousPattern255 = true;
                                            break;
                                    }
                            }
                    }
                    delete[] maliciousPattern255;
                    if (this->queriesNum > 0 || addMaliciousPattern255) {
                            if (addMaliciousPattern255) this->patterns = new unsigned char *[this->queriesNum + 1];
                            else this->patterns = new unsigned char *[this->queriesNum];
                            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                    this->patterns[i] = new unsigned char[this->m + 1];
                                    this->patterns[i][this->m] = '\0';
                                    for (unsigned int j = 0; j < this->m; ++j) {
                                            this->patterns[i][j] = text[queriesFirstIndexArray[i] + j];
                                    }
                            }
                            if (addMaliciousPattern255) {
                                    this->patterns[this->queriesNum] = new unsigned char[this->m + 1];
                                    this->patterns[this->queriesNum][this->m] = '\0';
                                    for (unsigned int j = 0; j < this->m; ++j) {
                                            this->patterns[this->queriesNum][j] = (unsigned char)255;
                                    }
                                    ++(this->queriesNum);
                            }
                            this->counts = new DATATYPE[this->queriesNum];
                            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                    this->counts[i] = getSACount(sa, text, saLen, this->patterns[i], this->m);
                            }
                            this->locates = new vector<DATATYPE>[this->queriesNum];
                            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                    getSALocate(sa, text, saLen, this->patterns[i], this->m, this->locates[i]);
                            }
                    }
                    delete[] sa;
                    cout << "Done" << endl;
                    if (this->queriesNum == 0) cout << "There is no malicious pattern for selected characters" << endl;

                    cout << "Saving patterns in " << patternFileName << " ... " << flush;
                    FILE *outFile;
                    outFile = fopen(patternFileName, "w");
                    fwrite(&this->queriesNum, (size_t)(sizeof(unsigned int)), (size_t)1, outFile);
                    typename vector<DATATYPE>::iterator it;
                    if (this->queriesNum > 0) {
                            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                    fwrite(this->patterns[i], (size_t)(sizeof(unsigned char)), (size_t)(this->m), outFile);
                            }
                            fwrite(this->counts, (size_t)(sizeof(DATATYPE)), (size_t)(this->queriesNum), outFile);
                            DATATYPE locateSize;
                            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                    locateSize = this->locates[i].size();
                                    fwrite(&locateSize, (size_t)(sizeof(DATATYPE)), (size_t)1, outFile);
                                    for(it = this->locates[i].begin(); it != this->locates[i].end(); ++it) {
                                            fwrite(&(*it), (size_t)(sizeof(DATATYPE)), (size_t)1, outFile);
                                    }
                            }
                    }
                    fclose(outFile);
                    cout << "Done" << endl;
            } else {
                    cout << "Loading patterns from " << patternFileName << " ... " << flush;
                    size_t result;
                    DATATYPE locateSize, locateValue;
                    FILE *inFile;
                    inFile = fopen(patternFileName, "rb");
                    result = fread(&this->queriesNum, (size_t)(sizeof(unsigned int)), (size_t)1, inFile);
                    if (result != 1) {
                            cout << "Error reading file " << patternFileName << endl;
                            exit(1);
                    }
                    if (this->queriesNum > 0) {
                            this->patterns = new unsigned char *[this->queriesNum];
                            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                    this->patterns[i] = new unsigned char[this->m + 1];
                                    this->patterns[i][this->m] = '\0';
                                    result = fread(this->patterns[i], (size_t)(sizeof(unsigned char)), (size_t)(this->m), inFile);
                                    if (result != this->m) {
                                            cout << "Error reading file " << patternFileName << endl;
                                            exit(1);
                                    }
                            }
                            this->counts = new DATATYPE[this->queriesNum];
                            result = fread(this->counts, (size_t)(sizeof(DATATYPE)), (size_t)(this->queriesNum), inFile);
                            if (result != this->queriesNum) {
                                    cout << "Error reading file " << patternFileName << endl;
                                    exit(1);
                            }
                            this->locates = new vector<DATATYPE>[this->queriesNum];
                            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                    result = fread(&locateSize, (size_t)(sizeof(DATATYPE)), (size_t)1, inFile);
                                    if (result != 1) {
                                            cout << "Error reading file " << patternFileName << endl;
                                            exit(1);
                                    }
                                    for (unsigned int j = 0; j < locateSize; ++j) {
                                            result = fread(&locateValue, (size_t)(sizeof(DATATYPE)), (size_t)1, inFile);
                                            if (result != 1) {
                                                    cout << "Error reading file " << patternFileName << endl;
                                                    exit(1);
                                            }
                                            this->locates[i].push_back(locateValue);
                                    }
                            }
                    }
                    fclose(inFile);
                    cout << "Done" << endl;

            }

            delete[] text;

            this->initialized = true;
        }

public:
	MaliciousPatterns(const char *textFileName, unsigned int m, vector<unsigned char> selectedChars = {}) {
            this->textFileName = textFileName;
            this->setM(m);
            this->setSelectedChars(selectedChars);
	}
        
	~MaliciousPatterns() {
            this->freeMemory();
	}
        
	unsigned char **getPatterns() {
            if (!this->initialized) this->initializePatterns();
            return this->patterns;
        }
        
        unsigned int getQueriesNum() {
            if (!this->initialized) this->initializePatterns();
            return this->queriesNum;
        }
        
	DATATYPE *getSACounts() {
            if (!this->initialized) this->initializePatterns();
            return this->counts;
        }
        
        vector<DATATYPE> *getSALocates() {
            if (!this->initialized) this->initializePatterns();
            return this->locates;
        }
        
	unsigned int getErrorCountsNumber(DATATYPE *countsToCheck) {
            if (!this->initialized) this->initializePatterns();
            cout << "Checking counts consistency ... " << flush;
            unsigned int errorCountsNumber = 0;
            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                    if (countsToCheck[i] != this->counts[i]) ++errorCountsNumber;
            }
            cout << "Done" << endl;
            return errorCountsNumber;
        }
        
        unsigned int getErrorLocatesNumber(vector<DATATYPE> *locatesToCheck) {
            if (!this->initialized) this->initializePatterns();
            cout << "Checking locates consistency ... " << flush;
            unsigned int errorLocatesNumber = 0;
            for (unsigned int i = 0; i < this->queriesNum; ++i) {
                    if (locatesToCheck[i].size() != this->locates[i].size()) ++errorLocatesNumber;
                    else {
                        unordered_set<DATATYPE> set1(this->locates[i].begin(), this->locates[i].end());
                        unordered_set<DATATYPE> set2(locatesToCheck[i].begin(), locatesToCheck[i].end());
                        if (set1 != set2) ++errorLocatesNumber;
                    }
            }
            cout << "Done" << endl;
            return errorLocatesNumber;
        }
};

typedef MaliciousPatterns<unsigned int> MaliciousPatterns32;
typedef MaliciousPatterns<unsigned long long> MaliciousPatterns64;

class SAKeys {
private:
	const char *textFileName;
	unsigned int queriesNum;
        unsigned int seqLen;
	unsigned int *saKeys;
	unsigned int *saValues = NULL;

	void initialize() {
            this->initializeSAKeys();
        }
        
	void freeMemory() {
            delete[] this->saKeys;
            delete[] this->saValues;
        }
        
	void initializeSAKeys() {
            unsigned int textLen, saKeysLen;
            unsigned char *text = readFileChar(this->textFileName, textLen, 0U);
            if (textLen < this->seqLen) {
                    cout << "Error: text shorter than sequence length" << endl;
                    exit(1);
            }
            stringstream ss;
            ss << "sakeys-" << this->textFileName << "-" << this->queriesNum << "-" << this->seqLen << ".dat";
            string s = ss.str();
            char *sakeysFileName = (char *)(s.c_str());
            this->saKeys = new unsigned int[this->queriesNum];

            if (!fileExists(sakeysFileName)) {
                    cout << "Generating " << this->queriesNum << " SA keys from " << this->textFileName << " (for sequence length = " << this->seqLen << ") ... " << flush;

                    random_device rd;
                    mt19937 gen(rd());
                    uniform_int_distribution<unsigned int> dis(0, textLen - this->seqLen);

                    for (unsigned int i = 0; i < this->queriesNum; ++i) this->saKeys[i] = dis(gen);
                    cout << "Done" << endl;
                    cout << "Saving SA keys in " << sakeysFileName << " ... " << flush;
                    FILE *outFile;
                    outFile = fopen(sakeysFileName, "w");
                    fwrite(this->saKeys, (size_t)4, (size_t)(this->queriesNum), outFile);
                    fclose(outFile);
                    cout << "Done" << endl;
            } else {
                    cout << "Loading SA keys from " << sakeysFileName << " ... " << flush;
                    this->saKeys = readFile(sakeysFileName, saKeysLen, 0U);
                    cout << "Done" << endl;
            }

            delete[] text;
        }
        
	void initializeSAValues() {
            stringstream ss;
            ss << "savalues-" << this->textFileName << "-" << this->queriesNum  << "-" << this->seqLen << ".dat";
            string s = ss.str();
            char *saValuesFileName = (char *)(s.c_str());

            if (!fileExists(saValuesFileName)) {
                    unsigned int textLen;
                    unsigned char *text = readFileChar(this->textFileName, textLen, 0U);
                    unsigned int saLen;
                    unsigned int *sa = getSA(this->textFileName, text, textLen, saLen, 0U);

                    cout << "Getting SA values (sequence length = " << this->seqLen << ") from SA ... " << flush;
                    this->saValues = new unsigned int[this->queriesNum * this->seqLen];
                    for (unsigned int i = 0; i < this->queriesNum; ++i) {
                        this->saValues[i * this->seqLen] = sa[this->saKeys[i]];
                        for (unsigned int j = 0; j < this->seqLen; ++j) this->saValues[i * this->seqLen + j] = sa[this->saKeys[i] + j];
                    }
                    delete[] text;
                    delete[] sa;
                    cout << "Done" << endl;
                    cout << "Saving SA values in " << saValuesFileName << " ... " << flush;
                    FILE *outFile;
                    outFile = fopen(saValuesFileName, "w");
                    fwrite(this->saValues, (size_t)4, (size_t)(this->queriesNum * this->seqLen), outFile);
                    fclose(outFile);
                    cout << "Done" << endl;

            } else {
                    cout << "Loading SA values (sequence length = " << this->seqLen << ") from " << saValuesFileName << " ... " << flush;
                    unsigned int saValuesLen;
                    this->saValues = readFile(saValuesFileName, saValuesLen, 0U);
                    cout << "Done" << endl;
            }
        }
	
public:
	SAKeys(const char *textFileName, unsigned int queriesNum, unsigned int seqLen) {
		this->textFileName = textFileName;
		this->queriesNum = queriesNum;
                this->seqLen = seqLen;
		this->initialize();
	}
	~SAKeys() {
		this->freeMemory();
	}
	unsigned int *getSAKeys() {
            return this->saKeys;
        }
        
	unsigned int *getSAValues() {
            if (this->saValues == NULL) this->initializeSAValues();
            return this->saValues;
        }
        
	unsigned int getErrorSAValuesNumber(unsigned int *valuesToCheck) {
            if (this->saValues == NULL) this->initializeSAValues();
            cout << "Checking SA values consistency ... " << flush;
            unsigned int errorSAValuesNumber = 0;
            for (unsigned int i = 0; i < this->queriesNum * this->seqLen; ++i) {
                    if (valuesToCheck[i] != this->saValues[i]) ++errorSAValuesNumber;
            }
            cout << "Done" << endl;
            return errorSAValuesNumber;
        }
};

}

#endif	/* PATTERNS_HPP */

