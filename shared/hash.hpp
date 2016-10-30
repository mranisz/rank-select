#ifndef HASH_HPP
#define	HASH_HPP

#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "xxhash.h"

using namespace std;

namespace shared {
    
enum HTType {
    HT_STANDARD = 1,
    HT_DENSE = 2
};

template<typename DATATYPE> DATATYPE getUniqueSuffixNum(unsigned int k, unsigned char *text, DATATYPE textLen, DATATYPE *sa, DATATYPE saLen, vector<unsigned char> selectedChars = {}) {
    DATATYPE uniqueSuffixNum = 0;
    bool isSelectedChars = (selectedChars.size() != 0);

    unsigned char *lastPattern = new unsigned char[k + 1];
    for (unsigned int i = 0; i < k; ++i) lastPattern[i] = 255;
    lastPattern[k] = '\0';

    unsigned char *pattern = new unsigned char[k + 1];

    for (DATATYPE i = 0; i < saLen; i++) {
            if (sa[i] > (textLen - k)) continue;
            strncpy((char *)pattern, (const char*)(text + sa[i]), k);
            pattern[k] = '\0';
            if (strcmp((char *)pattern, (const char*)lastPattern) == 0) continue;
            else {
                    strcpy((char *)lastPattern, (const char*)pattern);
                    if (isSelectedChars) {
                            bool rejectPattern = false;
                            for (unsigned int j = 0; j < k; ++j) {
                                    bool symbolInSelectedChars = false;
                                    for (vector<unsigned char>::iterator it = selectedChars.begin(); it != selectedChars.end(); ++it) {
                                            if ((unsigned int)pattern[j] == (*it)) {
                                                    symbolInSelectedChars = true;
                                                    break;
                                            }
                                    }
                                    if (!symbolInSelectedChars) {
                                            rejectPattern = true;
                                            break;
                                    }
                            }
                            if (rejectPattern) continue;
                    }
                    ++uniqueSuffixNum;
            }
    }

    delete[] lastPattern;
    delete[] pattern;

    return uniqueSuffixNum;
}

template unsigned int getUniqueSuffixNum<unsigned int>(unsigned int k, unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen, vector<unsigned char> selectedChars = {});
template unsigned long long getUniqueSuffixNum<unsigned long long>(unsigned int k, unsigned char *text, unsigned long long textLen, unsigned long long *sa, unsigned long long saLen, vector<unsigned char> selectedChars = {});

template<typename DATATYPE> class HTBase {
protected:
	void freeMemory() {
            if (this->boundariesHT != NULL) delete[] this->boundariesHT;
            if (this->denseBoundariesHT != NULL) delete[] this->denseBoundariesHT;
        }
        
	void initialize() {
            this->bucketsNum = 0;
            this->boundariesHT = NULL;
            this->alignedBoundariesHT = NULL;
            this->boundariesHTLen = 0;
            this->denseBoundariesHT = NULL;
            this->alignedDenseBoundariesHT = NULL;
            this->denseBoundariesHTLen = 0;
        }
	
        void setLoadFactor(double loadFactor) {
            if (loadFactor <= 0.0 || loadFactor >= 1.0) {
                cout << "Error: not valid loadFactor value" << endl;
                exit(1);
            }
            this->loadFactor = loadFactor;
        }
        
	void setK(unsigned int k) {
            if (k < 2) {
                    cout << "Error: not valid k value" << endl;
                    exit(1);
            }
            this->k = k;
            this->prefixLength = k - 2;
        }
        
public:
	double loadFactor;
	unsigned int k;
        unsigned int prefixLength;
	unsigned long long bucketsNum;

	alignas(128) DATATYPE lut2[256][256][2];
	DATATYPE *boundariesHT;
	DATATYPE *alignedBoundariesHT;
        unsigned long long boundariesHTLen;
        DATATYPE *denseBoundariesHT;
        DATATYPE *alignedDenseBoundariesHT;
        unsigned long long denseBoundariesHTLen;
        
	const static DATATYPE emptyValueHT = (DATATYPE)-1;

        unsigned long long getHashValue(unsigned char *str, unsigned int strLen) {
            return XXH64(str, strLen, 0);
        }
        
	unsigned long long getHTSize() {
            unsigned long long size = sizeof(this->loadFactor) + sizeof(this->k) + sizeof(this->prefixLength) + sizeof(this->bucketsNum) + 256 * 256 * 2 * sizeof(DATATYPE) + sizeof(this->alignedBoundariesHT) + sizeof(this->alignedDenseBoundariesHT) + sizeof(this->boundariesHTLen) + sizeof(this->denseBoundariesHTLen);
            if (this->boundariesHTLen > 0) size += (this->boundariesHTLen + (128 / sizeof(DATATYPE))) * sizeof(DATATYPE);
            if (this->denseBoundariesHTLen > 0) size += (this->denseBoundariesHTLen + (128 / sizeof(DATATYPE))) * sizeof(DATATYPE);
            return size;
        }
        
	void save(FILE *outFile) {
            fwrite(&this->loadFactor, (size_t)sizeof(double), (size_t)1, outFile);
            fwrite(&this->k, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(&this->bucketsNum, (size_t)sizeof(unsigned long long), (size_t)1, outFile);
            fwrite(&this->lut2, (size_t)sizeof(DATATYPE), (size_t)(256 * 256 * 2), outFile);
            fwrite(&this->boundariesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, outFile);
            if (this->boundariesHTLen > 0) fwrite(this->alignedBoundariesHT, (size_t)sizeof(DATATYPE), (size_t)this->boundariesHTLen, outFile);
            fwrite(&this->denseBoundariesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, outFile);
            if (this->denseBoundariesHTLen > 0) fwrite(this->alignedDenseBoundariesHT, (size_t)sizeof(DATATYPE), (size_t)this->denseBoundariesHTLen, outFile);
        }
        
	void load(FILE *inFile) {
            this->free();
            size_t result;
            result = fread(&this->loadFactor, (size_t)sizeof(double), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(&this->k, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            this->prefixLength = this->k - 2;
            result = fread(&this->bucketsNum, (size_t)sizeof(unsigned long long), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(this->lut2, (size_t)sizeof(DATATYPE), (size_t)(256 * 256 * 2), inFile);
            if (result != (256 * 256 * 2)) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(&this->boundariesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            if (this->boundariesHTLen > 0) {
                    this->boundariesHT = new DATATYPE[this->boundariesHTLen + (128 / sizeof(DATATYPE))];
                    this->alignedBoundariesHT = this->boundariesHT;
                    while ((unsigned long long)(this->alignedBoundariesHT) % 128) ++(this->alignedBoundariesHT);
                    result = fread(this->alignedBoundariesHT, (size_t)sizeof(DATATYPE), (size_t)this->boundariesHTLen, inFile);
                    if (result != this->boundariesHTLen) {
                            cout << "Error loading index" << endl;
                            exit(1);
                    }
            }
            result = fread(&this->denseBoundariesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            if (this->denseBoundariesHTLen > 0) {
                    this->denseBoundariesHT = new DATATYPE[this->denseBoundariesHTLen + (128 / sizeof(DATATYPE))];
                    this->alignedDenseBoundariesHT = this->denseBoundariesHT;
                    while ((unsigned long long)(this->alignedDenseBoundariesHT) % 128) ++(this->alignedDenseBoundariesHT);
                    result = fread(this->alignedDenseBoundariesHT, (size_t)sizeof(DATATYPE), (size_t)this->denseBoundariesHTLen, inFile);
                    if (result != this->denseBoundariesHTLen) {
                            cout << "Error loading index" << endl;
                            exit(1);
                    }
            }
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
};

typedef HTBase<unsigned int> HTBase32;
typedef HTBase<unsigned long long> HTBase64;

template<HTType T, typename DATATYPE> class HT : public HTBase<DATATYPE> {
private:
	void fillStandardHTData(unsigned char *text, DATATYPE textLen, DATATYPE *sa, DATATYPE saLen) {
            unsigned long long hash = this->bucketsNum;
            this->boundariesHTLen = 2 * this->bucketsNum;
            this->boundariesHT = new DATATYPE[this->boundariesHTLen + (128 / sizeof(DATATYPE))];
            this->alignedBoundariesHT = this->boundariesHT;
            while ((unsigned long long)(this->alignedBoundariesHT) % 128) ++(this->alignedBoundariesHT);

            for (unsigned long long i = 0; i < this->boundariesHTLen; ++i) this->alignedBoundariesHT[i] = HT::emptyValueHT;

            unsigned char *lastPattern = new unsigned char[this->k + 1];
            for (unsigned int i = 0; i < this->k; ++i) lastPattern[i] = 255;
            lastPattern[this->k] = '\0';

            unsigned char *pattern = new unsigned char[this->k + 1];
            DATATYPE lastNotOutsideTextIndex;
            bool notLastOutsideText = true;

            for (DATATYPE i = 0; i < saLen; i++) {
                    if (sa[i] > (textLen - this->k)) {
                            if (notLastOutsideText) lastNotOutsideTextIndex = i;
                            notLastOutsideText = false;
                            continue;
                    }
                    strncpy((char *)pattern, (const char *)(text + sa[i]), this->k);
                    pattern[this->k] = '\0';
                    if (strcmp((char *)pattern, (const char *)lastPattern) == 0) {
                            notLastOutsideText = true;
                            continue;
                    }
                    else {
                            strcpy((char *)lastPattern, (const char *)pattern);
                            if (hash != this->bucketsNum)  {
                                    if (notLastOutsideText) this->alignedBoundariesHT[2 * hash + 1] = i;
                                    else this->alignedBoundariesHT[2 * hash + 1] = lastNotOutsideTextIndex;
                            }
                            notLastOutsideText = true;
                            hash = this->getHashValue(pattern, this->k) % this->bucketsNum;
                    }
                    while (true) {
                            if (this->alignedBoundariesHT[2 * hash] == HT::emptyValueHT) {
                                    this->alignedBoundariesHT[2 * hash] = i;
                                    break;
                            }
                            else {
                                    hash = (hash + 1) % this->bucketsNum;
                            }
                    }
            }
            if (hash != this->bucketsNum) this->alignedBoundariesHT[2 * hash + 1] = saLen;

            delete[] lastPattern;
            delete[] pattern;
        }
        
        void fillDenseHTData(unsigned char *text, DATATYPE textLen, DATATYPE *sa, DATATYPE saLen) {
            unsigned long long hash = this->bucketsNum;
            DATATYPE beg = 0, end = 0, step = 0;
            this->boundariesHTLen = this->bucketsNum;
            this->boundariesHT = new DATATYPE[this->boundariesHTLen + (128 / sizeof(DATATYPE))];
            this->alignedBoundariesHT = this->boundariesHT;
            while ((unsigned long long)(this->alignedBoundariesHT) % 128) ++(this->alignedBoundariesHT);
            this->denseBoundariesHTLen = this->boundariesHTLen / 2 + this->boundariesHTLen % 2;
            this->denseBoundariesHT = new DATATYPE[this->denseBoundariesHTLen + (128 / sizeof(DATATYPE))];
            this->alignedDenseBoundariesHT = this->denseBoundariesHT;
            while ((unsigned long long)(this->alignedDenseBoundariesHT) % 128) ++(this->alignedDenseBoundariesHT);
            DATATYPE* boundariesHTTemp = new DATATYPE[this->boundariesHTLen];

            for (unsigned long long i = 0; i < this->boundariesHTLen; ++i) {
                    this->alignedBoundariesHT[i] = HT::emptyValueHT;
                    boundariesHTTemp[i] = 0;
            }

            unsigned char *lastPattern = new unsigned char[this->k + 1];
            for (unsigned int i = 0; i < this->k; ++i) lastPattern[i] = 255;
            lastPattern[this->k] = '\0';

            unsigned char *pattern = new unsigned char[this->k + 1];
            DATATYPE lastNotOutsideTextIndex;
            bool notLastOutsideText = true;

            for (DATATYPE i = 0; i < saLen; i++) {
                    if (sa[i] > (textLen - this->k)) {
                            if (notLastOutsideText) lastNotOutsideTextIndex = i;
                            notLastOutsideText = false;
                            continue;
                    }
                    strncpy((char *)pattern, (const char *)(text + sa[i]), this->k);
                    pattern[this->k] = '\0';
                    if (strcmp((char *)pattern, (const char *)lastPattern) == 0) {
                            notLastOutsideText = true;
                            continue;
                    }
                    else {
                            strcpy((char *)lastPattern, (const char *)pattern);
                            if (hash != this->bucketsNum)  {
                                    if (notLastOutsideText) boundariesHTTemp[hash] = (DATATYPE)ceil(((double)i - beg) / step);
                                    else boundariesHTTemp[hash] = (DATATYPE)ceil(((double)lastNotOutsideTextIndex - beg) / step);

                            }
                            notLastOutsideText = true;
                            hash = this->getHashValue(pattern, this->k) % this->bucketsNum;
                    }
                    while (true) {
                            if (this->alignedBoundariesHT[hash] == HT::emptyValueHT) {
                                    beg = this->lut2[pattern[0]][pattern[1]][0];
                                    end = this->lut2[pattern[0]][pattern[1]][1];
                                    step = (DATATYPE)ceil(((double)end + 1 - beg) / (1ULL << (sizeof(DATATYPE) * 4)));
                                    this->alignedBoundariesHT[hash] = i;
                                    break;
                            }
                            else {
                                    hash = (hash + 1) % this->bucketsNum;
                            }
                    }
            }
            if (hash != this->bucketsNum) boundariesHTTemp[hash] = (DATATYPE)ceil(((double)saLen - beg) / step);

            for (unsigned long long i = 0; i < this->denseBoundariesHTLen; ++i) this->alignedDenseBoundariesHT[i] = (boundariesHTTemp[2 * i] << (sizeof(DATATYPE) * 4)) + boundariesHTTemp[2 * i + 1];

            delete[] boundariesHTTemp;
            delete[] lastPattern;
            delete[] pattern;
        }
        
        void getStandardHTBoundaries(unsigned char *pattern, unsigned char *text, DATATYPE *sa, DATATYPE &leftBoundary, DATATYPE &rightBoundary) {
            DATATYPE leftBoundaryLUT2 = this->lut2[pattern[0]][pattern[1]][0];
            DATATYPE rightBoundaryLUT2 = this->lut2[pattern[0]][pattern[1]][1];
            if (leftBoundaryLUT2 < rightBoundaryLUT2) {
                unsigned long long hash = this->getHashValue(pattern, this->k) % this->bucketsNum;
                while (true) {
                    leftBoundary = this->alignedBoundariesHT[2 * hash];
                    if (leftBoundary >= leftBoundaryLUT2 && leftBoundary < rightBoundaryLUT2 && strncmp((const char *)pattern + 2, (const char *)(text + sa[leftBoundary] + 2), this->prefixLength) == 0) {
                            rightBoundary = this->alignedBoundariesHT[2 * hash + 1];
                            break;
                    }
                    if (leftBoundary == HT::emptyValueHT) {
                            leftBoundary = 0;
                            rightBoundary = 0;
                            return;
                    }
                    ++hash;
                    if (hash == this->bucketsNum) {
                            hash = 0;
                    }
                }
            } else {
                leftBoundary = 0;
                rightBoundary = 0;
            }
        }
        
        void getDenseHTBoundaries(unsigned char *pattern, unsigned char *text, DATATYPE *sa, DATATYPE &leftBoundary, DATATYPE &rightBoundary) {
            DATATYPE step;
            DATATYPE leftBoundaryLUT2 = this->lut2[pattern[0]][pattern[1]][0];
            DATATYPE rightBoundaryLUT2 = this->lut2[pattern[0]][pattern[1]][1];
            if (leftBoundaryLUT2 < rightBoundaryLUT2) {
                unsigned long long hash = this->getHashValue(pattern, this->k) % this->bucketsNum;
                while (true) {
                    leftBoundary = this->alignedBoundariesHT[hash];
                    if (leftBoundary >= leftBoundaryLUT2 && leftBoundary < rightBoundaryLUT2 && strncmp((const char *)pattern + 2, (const char *)(text + sa[leftBoundary] + 2), this->prefixLength) == 0) {
                            step = (DATATYPE)ceil(((double)rightBoundaryLUT2 + 1 - leftBoundaryLUT2) / (1ULL << (sizeof(DATATYPE) * 4)));
                            if ((hash & 1) == 0) rightBoundary = (this->alignedDenseBoundariesHT[hash / 2] >> (sizeof(DATATYPE) * 4)) * step + leftBoundaryLUT2;
                            else rightBoundary = (this->alignedDenseBoundariesHT[hash / 2] & ((1ULL << (sizeof(DATATYPE) * 4)) - 1)) * step + leftBoundaryLUT2;
                            break;
                    }
                    if (leftBoundary == HT::emptyValueHT) {
                            leftBoundary = 0;
                            rightBoundary = 0;
                            return;
                    }
                    ++hash;
                    if (hash == this->bucketsNum) {
                            hash = 0;
                    }
                }
            } else {
                leftBoundary = 0;
                rightBoundary = 0;
            }
        }
        
public:

	HT() {
            this->initialize();
            this->setK(8);
            this->setLoadFactor(0.9);
	}

	HT(unsigned int k, double loadFactor) {
            this->initialize();
            this->setK(k);
            this->setLoadFactor(loadFactor);
	};
        
        ~HT() {
            this->free();
	}

        void build(unsigned char *text, DATATYPE textLen, DATATYPE *sa, DATATYPE saLen) {
            this->free();
            fillLUT2(this->lut2, text, sa, saLen);
            DATATYPE uniqueSuffixNum = getUniqueSuffixNum(this->k, text, textLen, sa, saLen);
            this->bucketsNum = (double)uniqueSuffixNum * (1.0 / this->loadFactor);
            switch(T) {
                case HTType::HT_DENSE:
                    this->fillDenseHTData(text, textLen, sa, saLen);
                    break;
                default:
                    this->fillStandardHTData(text, textLen, sa, saLen);
                    break;
            }
        }
        
        void getBoundaries(unsigned char *pattern, unsigned char *text, DATATYPE *sa, DATATYPE &leftBoundary, DATATYPE &rightBoundary) {
            switch (T) {
                case HTType::HT_DENSE:
                    return this->getDenseHTBoundaries(pattern, text, sa, leftBoundary, rightBoundary);
                    break;
                default:
                    return this->getStandardHTBoundaries(pattern, text, sa, leftBoundary, rightBoundary);
                    break;
            }
        }
};

template <HTType T> using HT32 = HT<T, unsigned int>;
template <HTType T> using HT64 = HT<T, unsigned long long>;

template<HTType T, typename DATATYPE> class HTExt {
private:
	void freeMemory() {
            if (this->boundariesHT != NULL) delete[] this->boundariesHT;
            if (this->denseBoundariesHT != NULL) delete[] this->denseBoundariesHT;
            if (this->entriesHT != NULL) delete[] this->entriesHT;
        }
        
	void initialize() {
            this->bucketsNum = 0;
            this->boundariesHT = NULL;
            this->alignedBoundariesHT = NULL;
            this->boundariesHTLen = 0;
            this->denseBoundariesHT = NULL;
            this->alignedDenseBoundariesHT = NULL;
            this->denseBoundariesHTLen = 0;
            this->entriesHT = NULL;
            this->alignedEntriesHT = NULL;
            this->entriesHTLen = 0;
        }
        
	void setLoadFactor(double loadFactor) {
            if (loadFactor <= 0.0 || loadFactor >= 1.0) {
		cout << "Error: not valid loadFactor value" << endl;
		exit(1);
            }
            this->loadFactor = loadFactor;
        }
        
	void setK(unsigned int k) {
            if (k < 2) {
		cout << "Error: not valid k value" << endl;
		exit(1);
            }
            this->k = k;
            this->prefixLength = k - 2;
        }
        
        unsigned long long getHashValue(unsigned char* str, unsigned int strLen) {
            return XXH64(str, strLen, 0);
        }
        
	void fillStandardHTData(unsigned char *text, DATATYPE textLen, DATATYPE *sa, DATATYPE saLen, vector<unsigned char> selectedChars = {}, unsigned char *cutOutEntries = NULL) {
            bool isSelectedChars = (selectedChars.size() != 0);
            unsigned long long hash = this->bucketsNum;
            this->boundariesHTLen = 2 * this->bucketsNum;
            this->boundariesHT = new DATATYPE[this->boundariesHTLen + (128 / sizeof(DATATYPE))];
            this->alignedBoundariesHT = this->boundariesHT;
            while ((unsigned long long)(this->alignedBoundariesHT) % 128) ++(this->alignedBoundariesHT);
            this->entriesHTLen = this->bucketsNum * this->prefixLength;
            this->entriesHT = new unsigned char[this->entriesHTLen + 128];
            this->alignedEntriesHT = this->entriesHT;
            while ((unsigned long long)(this->alignedEntriesHT) % 128) ++(this->alignedEntriesHT);

            for (unsigned long long i = 0; i < this->boundariesHTLen; ++i) this->alignedBoundariesHT[i] = HTExt::emptyValueHT;

            unsigned char *lastPattern = new unsigned char[this->k + 1];
            for (unsigned int i = 0; i < this->k; ++i) lastPattern[i] = 255;
            lastPattern[this->k] = '\0';

            unsigned char *pattern = new unsigned char[this->k + 1];
            DATATYPE lastNotOutsideTextIndex;
            bool notLastOutsideText = true;

            for (DATATYPE i = 0; i < saLen; i++) {
                    if (sa[i] > (textLen - this->k)) {
                            if (notLastOutsideText) lastNotOutsideTextIndex = i;
                            notLastOutsideText = false;
                            continue;
                    }
                    strncpy((char *)pattern, (const char *)(text + sa[i]), this->k);
                    pattern[this->k] = '\0';
                    if (strcmp((char *)pattern, (const char *)lastPattern) == 0) {
                            notLastOutsideText = true;
                            continue;
                    }
                    else {
                            strcpy((char *)lastPattern, (const char *)pattern);
                            if (hash != this->bucketsNum) {
                                    if (notLastOutsideText) this->alignedBoundariesHT[2 * hash + 1] = i;
                                    else this->alignedBoundariesHT[2 * hash + 1] = lastNotOutsideTextIndex;
                            }
                            notLastOutsideText = true;
                            if (isSelectedChars) {
                                    bool rejectPattern = false;
                                    for (unsigned int j = 0; j < this->k; ++j) {
                                            bool symbolInSelectedChars = false;
                                            for (vector<unsigned char>::iterator it = selectedChars.begin(); it != selectedChars.end(); ++it) {
                                                    if ((unsigned int)pattern[j] == (*it)) {
                                                            symbolInSelectedChars = true;
                                                            break;
                                                    }
                                            }
                                            if (!symbolInSelectedChars) {
                                                    rejectPattern = true;
                                                    break;
                                            }
                                    }
                                    if (rejectPattern) {
                                            hash = this->bucketsNum;
                                            continue;
                                    }
                            }
                            hash = getHashValue(pattern, this->k) % this->bucketsNum;
                    }
                    while (true) {
                            if (this->alignedBoundariesHT[2 * hash] == HTExt::emptyValueHT) {
                                    this->alignedBoundariesHT[2 * hash] = i;
                                    if (cutOutEntries != NULL)  for (unsigned int j = 0; j < 2; ++j) cutOutEntries[hash * 2 + j] = pattern[j];
                                    for (unsigned int j = 0; j < this->prefixLength; ++j) this->alignedEntriesHT[hash * this->prefixLength + j] = pattern[j + 2];
                                    break;
                            }
                            else {
                                    hash = (hash + 1) % this->bucketsNum;
                            }
                    }
            }
            if (hash != this->bucketsNum) this->alignedBoundariesHT[2 * hash + 1] = saLen;

            delete[] lastPattern;
            delete[] pattern;
        }
        
        void fillDenseHTData(unsigned char *text, DATATYPE textLen, DATATYPE *sa, DATATYPE saLen, vector<unsigned char> selectedChars = {}) {
            bool isSelectedChars = (selectedChars.size() != 0);
            unsigned long long hash = this->bucketsNum;
            DATATYPE beg = 0, end = 0, step = 0;
            this->boundariesHTLen = this->bucketsNum;
            this->boundariesHT = new DATATYPE[this->boundariesHTLen + (128 / sizeof(DATATYPE))];
            this->alignedBoundariesHT = this->boundariesHT;
            while ((unsigned long long)(this->alignedBoundariesHT) % 128) ++(this->alignedBoundariesHT);
            this->denseBoundariesHTLen = this->boundariesHTLen / 2 + this->boundariesHTLen % 2;
            this->denseBoundariesHT = new DATATYPE[this->denseBoundariesHTLen + (128 / sizeof(DATATYPE))];
            this->alignedDenseBoundariesHT = this->denseBoundariesHT;
            while ((unsigned long long)(this->alignedDenseBoundariesHT) % 128) ++(this->alignedDenseBoundariesHT);
            DATATYPE* boundariesHTTemp = new DATATYPE[this->boundariesHTLen];
            this->entriesHTLen = this->bucketsNum * this->prefixLength;
            this->entriesHT = new unsigned char[this->entriesHTLen + 128];
            this->alignedEntriesHT = this->entriesHT;
            while ((unsigned long long)(this->alignedEntriesHT) % 128) ++(this->alignedEntriesHT);

            for (unsigned long long i = 0; i < this->boundariesHTLen; ++i) {
                    this->alignedBoundariesHT[i] = HTExt::emptyValueHT;
                    boundariesHTTemp[i] = 0;
            }

            unsigned char *lastPattern = new unsigned char[this->k + 1];
            for (unsigned int i = 0; i < this->k; ++i) lastPattern[i] = 255;
            lastPattern[this->k] = '\0';

            unsigned char *pattern = new unsigned char[this->k + 1];
            DATATYPE lastNotOutsideTextIndex;
            bool notLastOutsideText = true;

            for (DATATYPE i = 0; i < saLen; i++) {
                    if (sa[i] > (textLen - this->k)) {
                            if (notLastOutsideText) lastNotOutsideTextIndex = i;
                            notLastOutsideText = false;
                            continue;
                    }
                    strncpy((char *)pattern, (const char *)(text + sa[i]), this->k);
                    pattern[this->k] = '\0';
                    if (strcmp((char *)pattern, (const char *)lastPattern) == 0) {
                            notLastOutsideText = true;
                            continue;
                    }
                    else {
                            strcpy((char *)lastPattern, (const char *)pattern);
                            if (hash != this->bucketsNum) {
                                    if (notLastOutsideText) boundariesHTTemp[hash] = (DATATYPE)ceil(((double)i - beg) / step);
                                    else boundariesHTTemp[hash] = (DATATYPE)ceil(((double)lastNotOutsideTextIndex - beg) / step);
                            }
                            notLastOutsideText = true;
                            if (isSelectedChars) {
                                    bool rejectPattern = false;
                                    for (unsigned int j = 0; j < this->k; ++j) {
                                            bool symbolInSelectedChars = false;
                                            for (vector<unsigned char>::iterator it = selectedChars.begin(); it != selectedChars.end(); ++it) {
                                                    if ((unsigned int)pattern[j] == (*it)) {
                                                            symbolInSelectedChars = true;
                                                            break;
                                                    }
                                            }
                                            if (!symbolInSelectedChars) {
                                                    rejectPattern = true;
                                                    break;
                                            }
                                    }
                                    if (rejectPattern) {
                                            hash = this->bucketsNum;
                                            continue;
                                    }
                            }
                            hash = getHashValue(pattern, this->k) % this->bucketsNum;
                    }
                    while (true) {
                            if (this->alignedBoundariesHT[hash] == HTExt::emptyValueHT) {
                                    beg = this->lut2[pattern[0]][pattern[1]][0];
                                    end = this->lut2[pattern[0]][pattern[1]][1];
                                    step = (DATATYPE)ceil(((double)end + 1 - beg) / (1ULL << (sizeof(DATATYPE) * 4)));
                                    this->alignedBoundariesHT[hash] = i;
                                    for (unsigned int j = 0; j < this->prefixLength; ++j) this->alignedEntriesHT[hash * this->prefixLength + j] = pattern[j + 2];
                                    break;
                            }
                            else {
                                    hash = (hash + 1) % this->bucketsNum;
                            }
                    }
            }
            if (hash != this->bucketsNum) boundariesHTTemp[hash] = (DATATYPE)ceil(((double)saLen - beg) / step);

            for (unsigned long long i = 0; i < this->denseBoundariesHTLen; ++i) this->alignedDenseBoundariesHT[i] = (boundariesHTTemp[2 * i] << (sizeof(DATATYPE) * 4)) + boundariesHTTemp[2 * i + 1];

            delete[] boundariesHTTemp;
            delete[] lastPattern;
            delete[] pattern;
        }
        
        void getStandardHTBoundaries(unsigned char *pattern, DATATYPE &leftBoundary, DATATYPE &rightBoundary) {
            DATATYPE leftBoundaryLUT2 = this->lut2[pattern[0]][pattern[1]][0];
            DATATYPE rightBoundaryLUT2 = this->lut2[pattern[0]][pattern[1]][1];
            if (leftBoundaryLUT2 < rightBoundaryLUT2) {
                    unsigned long long hash = this->getHashValue(pattern, this->k) % this->bucketsNum;
                    while (true) {
                            leftBoundary = this->alignedBoundariesHT[2 * hash];
                            if (leftBoundary >= leftBoundaryLUT2 && leftBoundary < rightBoundaryLUT2 && strncmp((const char *)pattern + 2, (const char *)&(this->alignedEntriesHT[hash * this->prefixLength]), this->prefixLength) == 0) {
                                    rightBoundary = this->alignedBoundariesHT[2 * hash + 1];
                                    break;
                            }
                            if (leftBoundary == HTExt::emptyValueHT) {
                                    leftBoundary = 0;
                                    rightBoundary = 0;
                                    return;
                            }
                            ++hash;
                            if (hash == this->bucketsNum) {
                                    hash = 0;
                            }
                    }
            } else {
                    leftBoundary = 0;
                    rightBoundary = 0;
            }
        }
        
        void getDenseHTBoundaries(unsigned char *pattern, DATATYPE &leftBoundary, DATATYPE &rightBoundary) {
            DATATYPE step;
            DATATYPE leftBoundaryLUT2 = this->lut2[pattern[0]][pattern[1]][0];
            DATATYPE rightBoundaryLUT2 = this->lut2[pattern[0]][pattern[1]][1];
            if (leftBoundaryLUT2 < rightBoundaryLUT2) {
                    unsigned long long hash = this->getHashValue(pattern, this->k) % this->bucketsNum;
                    while (true) {
                            leftBoundary = this->alignedBoundariesHT[hash];
                            if (leftBoundary >= leftBoundaryLUT2 && leftBoundary < rightBoundaryLUT2 && strncmp((const char *)pattern + 2, (const char *)&(this->alignedEntriesHT[hash * this->prefixLength]), this->prefixLength) == 0) {
                                    step = (DATATYPE)ceil(((double)rightBoundaryLUT2 + 1 - leftBoundaryLUT2) / (1ULL << (sizeof(DATATYPE) * 4)));
                                    if ((hash & 1) == 0) rightBoundary = (this->alignedDenseBoundariesHT[hash / 2] >> (sizeof(DATATYPE) * 4)) * step + leftBoundaryLUT2;
                                    else rightBoundary = (this->alignedDenseBoundariesHT[hash / 2] & ((1ULL << (sizeof(DATATYPE) * 4)) - 1)) * step + leftBoundaryLUT2;
                                    break;
                            }
                            if (leftBoundary == HTExt::emptyValueHT) {
                                    leftBoundary = 0;
                                    rightBoundary = 0;
                                    return;
                            }
                            ++hash;
                            if (hash == this->bucketsNum) {
                                    hash = 0;
                            }
                    }
            } else {
                    leftBoundary = 0;
                    rightBoundary = 0;
            }
        }

public:
	double loadFactor;
	unsigned int k;
        unsigned int prefixLength;
	unsigned long long bucketsNum;

	alignas(128) DATATYPE lut2[256][256][2];
	DATATYPE *boundariesHT;
	DATATYPE *alignedBoundariesHT;
        unsigned long long boundariesHTLen;
        DATATYPE *denseBoundariesHT;
        DATATYPE *alignedDenseBoundariesHT;
        unsigned long long denseBoundariesHTLen;
	unsigned char *entriesHT;
	unsigned char *alignedEntriesHT;
        unsigned long long entriesHTLen;
        

	const static DATATYPE emptyValueHT = (DATATYPE)-1;
        const static DATATYPE emptyValueDenseHT = ((DATATYPE)-1 >> (sizeof(DATATYPE) * 4));

	HTExt() {
		this->initialize();
		this->setK(8);
		this->setLoadFactor(0.9);
	}

	HTExt(unsigned int k, double loadFactor) {
		this->initialize();
		this->setK(k);
		this->setLoadFactor(loadFactor);
	};
        
        ~HTExt() {
		this->free();
	}

	unsigned long long getHTSize() {
            unsigned long long size = sizeof(this->loadFactor) + sizeof(this->k) + sizeof(this->prefixLength) + sizeof(this->bucketsNum) + 256 * 256 * 2 * sizeof(DATATYPE) + sizeof(this->alignedBoundariesHT) + sizeof(this->alignedDenseBoundariesHT) + sizeof(this->alignedEntriesHT) + sizeof(this->boundariesHTLen) + sizeof(this->denseBoundariesHTLen) + sizeof(this->entriesHTLen);
            if (this->boundariesHTLen > 0) size += (this->boundariesHTLen + (128 / sizeof(DATATYPE))) * sizeof(DATATYPE);
            if (this->denseBoundariesHTLen > 0) size += (this->denseBoundariesHTLen + (128 / sizeof(DATATYPE))) * sizeof(DATATYPE);
            if (this->entriesHTLen > 0) size += (this->entriesHTLen + 128) * sizeof(unsigned char);
            return size;
        }
        
        void build(unsigned char *text, DATATYPE textLen, DATATYPE *sa, DATATYPE saLen, vector<unsigned char> selectedChars = {}, unsigned char *cutOutEntries = NULL) {
            this->free();
            fillLUT2(this->lut2, text, sa, saLen);
            DATATYPE uniqueSuffixNum = getUniqueSuffixNum(this->k, text, textLen, sa, saLen, selectedChars);
            this->bucketsNum = (double)uniqueSuffixNum * (1.0 / this->loadFactor);
            switch(T) {
                case HTType::HT_DENSE:
                    this->fillDenseHTData(text, textLen, sa, saLen, selectedChars);
                    break;
                default:
                    this->fillStandardHTData(text, textLen, sa, saLen, selectedChars, cutOutEntries);
                    break;
            }
        }
        
        void getBoundaries(unsigned char *pattern, DATATYPE &leftBoundary, DATATYPE &rightBoundary) {
            switch(T) {
                case HTType::HT_DENSE:
                    return this->getDenseHTBoundaries(pattern, leftBoundary, rightBoundary);
                    break;
                default:
                    return this->getStandardHTBoundaries(pattern, leftBoundary, rightBoundary);
                    break;
            }
        }
        
	void save(FILE *outFile) {
            fwrite(&this->loadFactor, (size_t)sizeof(double), (size_t)1, outFile);
            fwrite(&this->k, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(&this->bucketsNum, (size_t)sizeof(unsigned long long), (size_t)1, outFile);
            fwrite(&this->lut2, (size_t)sizeof(DATATYPE), (size_t)(256 * 256 * 2), outFile);
            fwrite(&this->boundariesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, outFile);
            if (this->boundariesHTLen > 0) fwrite(this->alignedBoundariesHT, (size_t)sizeof(DATATYPE), (size_t)this->boundariesHTLen, outFile);
            fwrite(&this->denseBoundariesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, outFile);
            if (this->denseBoundariesHTLen > 0) fwrite(this->alignedDenseBoundariesHT, (size_t)sizeof(DATATYPE), (size_t)this->denseBoundariesHTLen, outFile);
            fwrite(&this->entriesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, outFile);
            if (this->entriesHTLen > 0) fwrite(this->alignedEntriesHT, (size_t)sizeof(unsigned char), (size_t)this->entriesHTLen, outFile);
        }
        
	void load(FILE *inFile) {
            this->free();
            size_t result;
            result = fread(&this->loadFactor, (size_t)sizeof(double), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(&this->k, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            this->prefixLength = this->k - 2;
            result = fread(&this->bucketsNum, (size_t)sizeof(unsigned long long), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(this->lut2, (size_t)sizeof(DATATYPE), (size_t)(256 * 256 * 2), inFile);
            if (result != (256 * 256 * 2)) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(&this->boundariesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            if (this->boundariesHTLen > 0) {
                    this->boundariesHT = new DATATYPE[this->boundariesHTLen + (128 / sizeof(DATATYPE))];
                    this->alignedBoundariesHT = this->boundariesHT;
                    while ((unsigned long long)(this->alignedBoundariesHT) % 128) ++(this->alignedBoundariesHT);
                    result = fread(this->alignedBoundariesHT, (size_t)sizeof(DATATYPE), (size_t)this->boundariesHTLen, inFile);
                    if (result != this->boundariesHTLen) {
                            cout << "Error loading index" << endl;
                            exit(1);
                    }
            }
            result = fread(&this->denseBoundariesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            if (this->denseBoundariesHTLen > 0) {
                    this->denseBoundariesHT = new DATATYPE[this->denseBoundariesHTLen + (128 / sizeof(DATATYPE))];
                    this->alignedDenseBoundariesHT = this->denseBoundariesHT;
                    while ((unsigned long long)(this->alignedDenseBoundariesHT) % 128) ++(this->alignedDenseBoundariesHT);
                    result = fread(this->alignedDenseBoundariesHT, (size_t)sizeof(DATATYPE), (size_t)this->denseBoundariesHTLen, inFile);
                    if (result != this->denseBoundariesHTLen) {
                            cout << "Error loading index" << endl;
                            exit(1);
                    }
            }
            result = fread(&this->entriesHTLen, (size_t)sizeof(unsigned long long), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            if (this->entriesHTLen > 0) {
                    this->entriesHT = new unsigned char[this->entriesHTLen + 128];
                    this->alignedEntriesHT = this->entriesHT;
                    while ((unsigned long long)(this->alignedEntriesHT) % 128) ++(this->alignedEntriesHT);
                    result = fread(this->alignedEntriesHT, (size_t)sizeof(unsigned char), (size_t)this->entriesHTLen, inFile);
                    if (result != this->entriesHTLen) {
                            cout << "Error loading index" << endl;
                            exit(1);
                    }
            }
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
};

template <HTType T> using HTExt32 = HTExt<T, unsigned int>;
template <HTType T> using HTExt64 = HTExt<T, unsigned long long>;

}

#endif	/* HASH_HPP */

