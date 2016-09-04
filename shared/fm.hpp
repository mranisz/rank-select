#ifndef FM_HPP
#define	FM_HPP

#include "wt.hpp"
#include "huff.h"
#include "hash.hpp"

namespace shared {

template<class WT> class FMHWT {
protected:
	WT *wt;
	alignas(128) unsigned long long code[256];
	alignas(128) unsigned int codeLen[256];
	alignas(128) unsigned int c[257];
	unsigned int textLen;
        
	void freeMemory() {
            if (this->wt != NULL) delete this->wt;
        }
        
	void initialize() {
            this->wt = NULL;
            for (int i = 0; i < 256; ++i) {
                    this->code[i] = 0;
                    this->codeLen[i] = 0;
            }
            for (int i = 0; i < 257; ++i) this->c[i] = 0;
            this->textLen = 0;
        }
        
	unsigned int countHWT(unsigned char *pattern, int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen) {
            unsigned char c;
            --i;

            while (firstVal <= lastVal && i >= 0) {
                    c = pattern[i--];
                    if (codeLen[c] == 0) return 0;
                    firstVal = C[c] + wt->getRankHWT(code[c], codeLen[c], firstVal - 1, 0) + 1;
                    lastVal = C[c] + wt->getRankHWT(code[c], codeLen[c], lastVal, 0);
            }

            return (lastVal + 1) - firstVal;
        }

public:

	FMHWT() {
		this->initialize();
	}

	~FMHWT() {
		this->free();
	}

	void build(const char *textFileName) {
            this->free();
            unsigned char *text = readText(textFileName, this->textLen, 0);
            checkNullChar(text, this->textLen);
            unsigned int bwtLen;
            unsigned char *bwt = getBWT(textFileName, text, this->textLen, bwtLen, 0);
            cout << "Huffman encoding ... " << flush;
            encodeHuffFromText(2, bwt, bwtLen, this->code, this->codeLen);
            cout << "Done" << endl;
            cout << "Building WT ... " << flush;
            this->wt = WT::createHWT(bwt, bwtLen, 0, this->code, this->codeLen);
            delete[] bwt;
            cout << "Done" << endl;
            fillArrayC(text, this->textLen, this->c);
            delete[] text;
            cout << "Index successfully built" << endl;
        }
        
        void save(FILE *outFile) {
            bool nullPointer = false;
            bool notNullPointer = true;
            fwrite(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(this->c, (size_t)sizeof(unsigned int), (size_t)257, outFile);
            fwrite(this->code, (size_t)sizeof(unsigned long long), (size_t)256, outFile);
            fwrite(this->codeLen, (size_t)sizeof(unsigned int), (size_t)256, outFile);
            if (this->wt == NULL) fwrite(&nullPointer, (size_t)sizeof(bool), (size_t)1, outFile);
            else {
                    fwrite(&notNullPointer, (size_t)sizeof(bool), (size_t)1, outFile);
                    this->wt->save(outFile);
            }
        }
        
	void save(const char *fileName) {
            cout << "Saving index in " << fileName << " ... " << flush;
            FILE *outFile = fopen(fileName, "w");
            this->save(outFile);
            fclose(outFile);
            cout << "Done" << endl;
        }
        
	void load(FILE *inFile) {
            this->free();
            bool isNotNullPointer;
            size_t result = fread(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(this->c, (size_t)sizeof(unsigned int), (size_t)257, inFile);
            if (result != 257) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(this->code, (size_t)sizeof(unsigned long long), (size_t)256, inFile);
            if (result != 256) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(this->codeLen, (size_t)sizeof(unsigned int), (size_t)256, inFile);
            if (result != 256) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(&isNotNullPointer, (size_t)sizeof(bool), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            if (isNotNullPointer) {
                    this->wt = new WT();
                    this->wt->load(inFile);
            }
        }
        
        void load(const char *fileName) {
            FILE *inFile = fopen(fileName, "rb");
            cout << "Loading index from " << fileName << " ... " << flush;
            this->load(inFile);
            fclose(inFile);
            cout << "Done" << endl;
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
        
	unsigned int getIndexSize() {
            unsigned int size = sizeof(this->wt);
            size += (257 * sizeof(unsigned int) + 256 * sizeof(unsigned int) + 256 * sizeof(unsigned long long));
            if (this->wt != NULL) size += this->wt->getWTSize();
            return size;
        }
        
	unsigned int getTextSize() {
            return this->textLen * sizeof(unsigned char);
        }

	unsigned int count(unsigned char *pattern, unsigned int patternLen) {
            return this->countHWT(pattern, patternLen - 1, this->c, this->wt, this->c[pattern[patternLen - 1]] + 1, this->c[pattern[patternLen - 1] + 1], this->code, this->codeLen);
        }
};

template<class WT> class FMHWTHash : public FMHWT<WT> {
private:
	HTExt<HTType::HT_STANDARD> *ht = NULL;
        
	void freeMemory() {
            FMHWT<WT>::freeMemory();
            if (this->ht != NULL) this->ht->free();
        }

public:

	FMHWTHash(unsigned int k, double loadFactor) {
            this->initialize();
            this->ht = new HTExt<HTType::HT_STANDARD>(k, loadFactor);
	}

	~FMHWTHash() {
            this->free();
            if (this->ht != NULL) delete this->ht;
	}

	void build(const char *textFileName) {
            this->free();
            unsigned char *text = readText(textFileName, this->textLen, 0);
            checkNullChar(text, this->textLen);
            unsigned int bwtLen;
            unsigned int saLen;
            unsigned int *sa = getSA(textFileName, text, this->textLen, saLen, 0);
            cout << "Building hash table ... " << flush;
            this->ht->build(text, this->textLen, sa, saLen);
            cout << "Done" << endl;
            unsigned char *bwt = getBWT(text, this->textLen, sa, saLen, bwtLen, 0);
            delete[] sa;
            cout << "Huffman encoding ... " << flush;
            encodeHuffFromText(2, bwt, bwtLen, this->code, this->codeLen);
            cout << "Done" << endl;
            cout << "Building WT ... " << flush;
            this->wt = WT::createHWT(bwt, bwtLen, 0, this->code, this->codeLen);
            delete[] bwt;
            cout << "Done" << endl;
            fillArrayC(text, this->textLen, this->c);
            delete[] text;
            cout << "Index successfully built" << endl;
        }
        
	void save(FILE *outFile) {
            FMHWT<WT>::save(outFile);
            this->ht->save(outFile);
        }
        
        void save(const char *fileName) {
            cout << "Saving index in " << fileName << " ... " << flush;
            FILE *outFile = fopen(fileName, "w");
            this->save(outFile);
            fclose(outFile);
            cout << "Done" << endl;
        }
        
	void load(FILE *inFile) {
            FMHWT<WT>::load(inFile);
            delete this->ht;
            this->ht = new HTExt<HTType::HT_STANDARD>();
            this->ht->load(inFile);
        }
        
        void load(const char *fileName) {
            FILE *inFile = fopen(fileName, "rb");
            cout << "Loading index from " << fileName << " ... " << flush;
            this->load(inFile);
            fclose(inFile);
            cout << "Done" << endl;
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
        
	unsigned int getIndexSize() {
            return FMHWT<WT>::getIndexSize() + sizeof(this->ht) + this->ht->getHTSize();
        }

	unsigned int count(unsigned char *pattern, unsigned int patternLen) {
            if (patternLen < this->ht->k) return FMHWT<WT>::count(pattern, patternLen);
            unsigned int leftBoundary, rightBoundary;
            this->ht->getBoundaries(pattern + (patternLen - this->ht->k), leftBoundary, rightBoundary);
            return this->countHWT(pattern, patternLen - this->ht->k, this->c, this->wt, leftBoundary + 1, rightBoundary, this->code, this->codeLen);
        }
};

}

#endif	/* FM_HPP */

