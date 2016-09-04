#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <unordered_set>
#include <algorithm>
#include "common.h"
#include "patterns.h"

namespace shared {
    
void Patterns::setQueriesNum(unsigned int queriesNum) {
	if (queriesNum == 0) {
		cout << "Error: not valid queriesNum value" << endl;
		exit(1);
	}
	this->queriesNum = queriesNum;
}

void Patterns::setM(unsigned int m) {
	if (m == 0) {
		cout << "Error: not valid m value" << endl;
		exit(1);
	}
	this->m = m;
}

void Patterns::setSelectedChars(vector<unsigned char> selectedChars) {
	this->selectedChars = selectedChars;
}

void Patterns::initializePatterns() {
	unsigned int textLen, queriesFirstIndexArrayLen;
	unsigned char *text = readFileChar(this->textFileName, textLen, 0);
        if (textLen < this->m) {
                cout << "Error: text shorter than pattern length" << endl;
                exit(1);
        }
	unsigned int *queriesFirstIndexArray;
        stringstream ss;
	ss << "patterns-" << this->textFileName << "-" << this->m << "-" << this->queriesNum << "-" << getStringFromSelectedChars(this->selectedChars, ".") << ".dat";
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
		uniform_int_distribution<unsigned int> dis(0, textLen - this->m);
                
		queriesFirstIndexArray = new unsigned int[this->queriesNum];

		unsigned int genVal;

		if (this->selectedChars.size() != 0) {
			for (unsigned long long i = 0; i < this->queriesNum; ++i) {
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
		fwrite(queriesFirstIndexArray, (size_t)(sizeof(unsigned int)), (size_t)(this->queriesNum), outFile);
		fclose(outFile);
		cout << "Done" << endl;
	} else {
		cout << "Loading patterns from " << patternFileName << " ... " << flush;
		queriesFirstIndexArray = readFileInt(patternFileName, queriesFirstIndexArrayLen, 0);
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

void Patterns::initializeSACounts() {
	if (this->patterns == NULL) this->initializePatterns();
	stringstream ss;
	ss << "counts-" << this->textFileName << "-" << this->m << "-" << this->queriesNum << "-" << getStringFromSelectedChars(this->selectedChars, ".") << ".dat";
	string s = ss.str();
	char *countsFileName = (char *)(s.c_str());

	if (!fileExists(countsFileName)) {
                unsigned int textLen;
		unsigned char *text = readFileChar(this->textFileName, textLen, 0);
                unsigned int saLen;
		unsigned int *sa = getSA(this->textFileName, text, textLen, saLen, 0);

		cout << "Getting counts from SA ... " << flush;
		this->counts = new unsigned int[this->queriesNum];
		for (unsigned int i = 0; i < this->queriesNum; ++i) {
			this->counts[i] = getSACount(sa, text, saLen, this->patterns[i], this->m);
		}
		delete[] text;
		delete[] sa;
		cout << "Done" << endl;
		cout << "Saving counts in " << countsFileName << " ... " << flush;
		FILE *outFile;
		outFile = fopen(countsFileName, "w");
		fwrite(this->counts, (size_t)(sizeof(unsigned int)), (size_t)(this->queriesNum), outFile);
		fclose(outFile);
		cout << "Done" << endl;

	} else {
		cout << "Loading counts from " << countsFileName << " ... " << flush;
		unsigned int countsLen;
		this->counts = readFileInt(countsFileName, countsLen, 0);
		cout << "Done" << endl;
	}
}

void Patterns::initializeSALocates() {
	if (this->patterns == NULL) this->initializePatterns();
	stringstream ss;
	ss << "locates-" << this->textFileName << "-" << this->m << "-" << this->queriesNum << "-" << getStringFromSelectedChars(this->selectedChars, ".") << ".dat";
	string s = ss.str();
	char *locatesFileName = (char *)(s.c_str());

	if (!fileExists(locatesFileName)) {
                unsigned int textLen;
		unsigned char *text = readFileChar(this->textFileName, textLen, 0);
		unsigned int saLen;
		unsigned int *sa = getSA(this->textFileName, text, textLen, saLen, 0);
                
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
                
                if (counter * sizeof(unsigned int) > 1 * 1024 * 1024 * 1024) return;
                
		cout << "Saving locates in " << locatesFileName << " ... " << flush;
		FILE *outFile;
		outFile = fopen(locatesFileName, "w");
                unsigned int locateSize;
                for (unsigned int i = 0; i < this->queriesNum; ++i) {
                        locateSize = this->locates[i].size();
                        fwrite(&locateSize, (size_t)(sizeof(unsigned int)), (size_t)1, outFile);
                        for(vector<unsigned int>::iterator it = this->locates[i].begin(); it != this->locates[i].end(); ++it) {
                                fwrite(&(*it), (size_t)(sizeof(unsigned int)), (size_t)1, outFile);
                        }
                }
		fclose(outFile);
		cout << "Done" << endl;

	} else {
		cout << "Loading locates from " << locatesFileName << " ... " << flush;
                size_t result;
                unsigned int locateSize, locateValue;
                FILE *inFile;
		inFile = fopen(locatesFileName, "rb");
                this->locates = new vector<unsigned int>[this->queriesNum];
		for (unsigned int i = 0; i < this->queriesNum; ++i) {
                    result = fread(&locateSize, (size_t)(sizeof(unsigned int)), (size_t)1, inFile);
                    if (result != 1) {
                            cout << "Error reading file " << locatesFileName << endl;
                            exit(1);
                    }
                    for (unsigned int j = 0; j < locateSize; ++j) {
                            result = fread(&locateValue, (size_t)(sizeof(unsigned int)), (size_t)1, inFile);
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

void Patterns::freeMemory() {
        if (this->patterns != NULL) {
                for (unsigned int i = 0; i < this->queriesNum; ++i) {
                        delete[] this->patterns[i];
                }
                delete[] this->patterns;
        }
	if (this->counts != NULL) delete[] this->counts;
        if (this->locates != NULL) delete[] this->locates;
}

unsigned char **Patterns::getPatterns() {
        if (this->patterns == NULL) this->initializePatterns();
	return this->patterns;
}

unsigned int *Patterns::getSACounts() {
	if (this->counts == NULL) this->initializeSACounts();
	return this->counts;
}

vector<unsigned int> *Patterns::getSALocates() {
	if (this->locates == NULL) this->initializeSALocates();
	return this->locates;
}

unsigned int Patterns::getErrorCountsNumber(unsigned int *countsToCheck) {
	if (this->counts == NULL) this->initializeSACounts();
	cout << "Checking counts consistency ... " << flush;
	unsigned int errorCountsNumber = 0;
	for (unsigned int i = 0; i < this->queriesNum; ++i) {
		if (countsToCheck[i] != this->counts[i]) ++errorCountsNumber;
	}
	cout << "Done" << endl;
	return errorCountsNumber;
}

unsigned int Patterns::getErrorLocatesNumber(vector<unsigned int> *locatesToCheck) {
	if (this->locates == NULL) this->initializeSALocates();
	cout << "Checking locates consistency ... " << flush;
	unsigned int errorLocatesNumber = 0;
	for (unsigned int i = 0; i < this->queriesNum; ++i) {
		if (locatesToCheck[i].size() != this->locates[i].size()) ++errorLocatesNumber;
                else {
                    unordered_set<unsigned int> set1(this->locates[i].begin(), this->locates[i].end());
                    unordered_set<unsigned int> set2(locatesToCheck[i].begin(), locatesToCheck[i].end());
                    if (set1 != set2) ++errorLocatesNumber;
                }
	}
	cout << "Done" << endl;
	return errorLocatesNumber;
}

void NegativePatterns::setQueriesNum(unsigned int queriesNum) {
	if (queriesNum == 0) {
		cout << "Error: not valid queriesNum value" << endl;
		exit(1);
	}
	this->queriesNum = queriesNum;
}

void NegativePatterns::setM(unsigned int m) {
	if (m == 0) {
		cout << "Error: not valid m value" << endl;
		exit(1);
	}
	this->m = m;
}

void NegativePatterns::initializePatterns() {
	unsigned int textLen;
	unsigned char *text = readFileChar(this->textFileName, textLen, 0);
        if (textLen < this->m) {
                cout << "Error: text shorter than pattern length" << endl;
                exit(1);
        }
        stringstream ss;
	ss << "negative-patterns-" << this->textFileName << "-" << this->m << "-" << this->queriesNum << ".dat";
	string s = ss.str();
	char *patternFileName = (char *)(s.c_str());

	if (!fileExists(patternFileName)) {
		unsigned int saLen;
		unsigned int *sa = getSA(this->textFileName, text, textLen, saLen, 0);
            
		cout << "Generating " << this->queriesNum << " negative patterns of length " << this->m << " from " << this->textFileName << " ... " << flush;

		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<unsigned int> dis(this->m - 1, textLen - 1);
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

unsigned int NegativePatterns::getSACount(unsigned int *sa, unsigned char *text, unsigned int saLen, unsigned char *pattern, int patternLength) {
	unsigned int beg = 0, end = 0;
	binarySearch(sa, text, 0, saLen, pattern, patternLength, beg, end);
	return end - beg;
}

void NegativePatterns::freeMemory() {
        if (this->patterns != NULL) {
                for (unsigned int i = 0; i < this->queriesNum; ++i) {
                        delete[] this->patterns[i];
                }
                delete[] this->patterns;
        }
}

unsigned char **NegativePatterns::getPatterns() {
        if (this->patterns == NULL) this->initializePatterns();
	return this->patterns;
}

unsigned int NegativePatterns::getErrorCountsNumber(unsigned int *countsToCheck) {
	cout << "Checking counts consistency ... " << flush;
	unsigned int errorCountsNumber = 0;
	for (unsigned int i = 0; i < this->queriesNum; ++i) {
		if (countsToCheck[i] != 0) ++errorCountsNumber;
	}
	cout << "Done" << endl;
	return errorCountsNumber;
}

unsigned int NegativePatterns::getErrorLocatesNumber(vector<unsigned int> *locatesToCheck) {
	cout << "Checking locates consistency ... " << flush;
	unsigned int errorLocatesNumber = 0;
	for (unsigned int i = 0; i < this->queriesNum; ++i) {
		if (locatesToCheck[i].size() != 0) ++errorLocatesNumber;
	}
	cout << "Done" << endl;
	return errorLocatesNumber;
}

void MaliciousPatterns::setM(unsigned int m) {
	if (m == 0) {
		cout << "Error: not valid m value" << endl;
		exit(1);
	}
	this->m = m;
}

void MaliciousPatterns::setSelectedChars(vector<unsigned char> selectedChars) {
	this->selectedChars = selectedChars;
}

void MaliciousPatterns::initializePatterns() {
	unsigned int textLen;
	unsigned char *text = readFileChar(this->textFileName, textLen, 0);
        if (textLen < this->m) {
                cout << "Error: text shorter than pattern length" << endl;
                exit(1);
        }
        
        stringstream ss;
	ss << "malicious-patterns-" << this->textFileName << "-" << this->m << "-" << getStringFromSelectedChars(this->selectedChars, ".") << ".dat";
	string s = ss.str();
	char *patternFileName = (char *)(s.c_str());

	if (!fileExists(patternFileName)) {
                unsigned int saLen;
                unsigned int *sa = getSA(this->textFileName, text, textLen, saLen, 0);

                cout << "Getting malicious patterns of length " << this->m << " from " << this->textFileName;
                if (this->selectedChars.size() != 0) {
                        cout << ", alphabet (ordinal): {" << getStringFromSelectedChars(this->selectedChars, ", ") << "}";
                }
                cout << " ... " << flush;
                
                unsigned int maliciousPatternsNum = 4;
                unsigned int queriesFirstIndexArray[maliciousPatternsNum] = {0, textLen - this->m, sa[1], sa[saLen - 1]};

                if (this->selectedChars.size() != 0) {
                        this->queriesNum = 0;
                        for (unsigned long long i = 0; i < maliciousPatternsNum; ++i) {
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
                        this->counts = new unsigned int[this->queriesNum];
                        for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                this->counts[i] = getSACount(sa, text, saLen, this->patterns[i], this->m);
                        }
                        this->locates = new vector<unsigned int>[this->queriesNum];
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
                if (this->queriesNum > 0) {
                        for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                fwrite(this->patterns[i], (size_t)(sizeof(unsigned char)), (size_t)(this->m), outFile);
                        }
                        fwrite(this->counts, (size_t)(sizeof(unsigned int)), (size_t)(this->queriesNum), outFile);
                        unsigned int locateSize;
                        for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                locateSize = this->locates[i].size();
                                fwrite(&locateSize, (size_t)(sizeof(unsigned int)), (size_t)1, outFile);
                                for(vector<unsigned int>::iterator it = this->locates[i].begin(); it != this->locates[i].end(); ++it) {
                                        fwrite(&(*it), (size_t)(sizeof(unsigned int)), (size_t)1, outFile);
                                }
                        }
                }
		fclose(outFile);
		cout << "Done" << endl;
        } else {
                cout << "Loading patterns from " << patternFileName << " ... " << flush;
		size_t result;
                unsigned int locateSize, locateValue;
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
                        this->counts = new unsigned int[this->queriesNum];
                        result = fread(this->counts, (size_t)(sizeof(unsigned int)), (size_t)(this->queriesNum), inFile);
                        if (result != this->queriesNum) {
                                cout << "Error reading file " << patternFileName << endl;
                                exit(1);
                        }
                        this->locates = new vector<unsigned int>[this->queriesNum];
                        for (unsigned int i = 0; i < this->queriesNum; ++i) {
                                result = fread(&locateSize, (size_t)(sizeof(unsigned int)), (size_t)1, inFile);
                                if (result != 1) {
                                        cout << "Error reading file " << patternFileName << endl;
                                        exit(1);
                                }
                                for (unsigned int j = 0; j < locateSize; ++j) {
                                        result = fread(&locateValue, (size_t)(sizeof(unsigned int)), (size_t)1, inFile);
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

void MaliciousPatterns::freeMemory() {
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

unsigned char **MaliciousPatterns::getPatterns() {
        if (!this->initialized) this->initializePatterns();
	return this->patterns;
}

unsigned int *MaliciousPatterns::getSACounts() {
	if (!this->initialized) this->initializePatterns();
	return this->counts;
}

vector<unsigned int> *MaliciousPatterns::getSALocates() {
	if (!this->initialized) this->initializePatterns();
	return this->locates;
}

unsigned int MaliciousPatterns::getQueriesNum() {
        if (!this->initialized) this->initializePatterns();
	return this->queriesNum;
}

unsigned int MaliciousPatterns::getErrorCountsNumber(unsigned int *countsToCheck) {
	if (!this->initialized) this->initializePatterns();
	cout << "Checking counts consistency ... " << flush;
	unsigned int errorCountsNumber = 0;
	for (unsigned int i = 0; i < this->queriesNum; ++i) {
		if (countsToCheck[i] != this->counts[i]) ++errorCountsNumber;
	}
	cout << "Done" << endl;
	return errorCountsNumber;
}

unsigned int MaliciousPatterns::getErrorLocatesNumber(vector<unsigned int> *locatesToCheck) {
	if (!this->initialized) this->initializePatterns();
	cout << "Checking locates consistency ... " << flush;
	unsigned int errorLocatesNumber = 0;
	for (unsigned int i = 0; i < this->queriesNum; ++i) {
		if (locatesToCheck[i].size() != this->locates[i].size()) ++errorLocatesNumber;
                else {
                    unordered_set<unsigned int> set1(this->locates[i].begin(), this->locates[i].end());
                    unordered_set<unsigned int> set2(locatesToCheck[i].begin(), locatesToCheck[i].end());
                    if (set1 != set2) ++errorLocatesNumber;
                }
	}
	cout << "Done" << endl;
	return errorLocatesNumber;
}

}
