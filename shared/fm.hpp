#ifndef FM_HPP
#define	FM_HPP

#include "wt.hpp"
#include "huff.hpp"
#include "hash.hpp"

namespace shared {

template<class WT, typename DATATYPE> class FMHWT {
protected:
	WT *wt;
	alignas(128) unsigned long long code[256];
	alignas(128) unsigned int codeLen[256];
	alignas(128) DATATYPE c[257];
	DATATYPE textLen;
        
	void freeMemory() {
            if (this->wt != NULL) delete this->wt;
        }
        
	void initialize() {
            this->wt = new WT();
            for (unsigned int i = 0; i < 256; ++i) {
                    this->code[i] = 0;
                    this->codeLen[i] = 0;
            }
            for (unsigned int i = 0; i < 257; ++i) this->c[i] = 0;
            this->textLen = 0;
        }
        
        DATATYPE countHWT(unsigned char *pattern, int i, DATATYPE *C, WT *wt, DATATYPE firstVal, DATATYPE lastVal, unsigned long long *code, unsigned int *codeLen) {
            unsigned char c;
            --i;

            while (firstVal <= lastVal && i >= 0) {
                    c = pattern[i--];
                    if (codeLen[c] == 0) return 0;
                    tie(firstVal, lastVal) = WT::getRankHWT(wt, code[c], codeLen[c], firstVal - 1, lastVal);
                    firstVal += C[c] + 1;
                    lastVal += C[c];
            }

            if (firstVal > lastVal) return 0;
            else return lastVal - firstVal + 1;
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
            unsigned char *text = readText(textFileName, this->textLen, (DATATYPE)0);
            checkNullChar(text, this->textLen);
            DATATYPE bwtLen;
            unsigned char *bwt = getBWT(textFileName, text, this->textLen, bwtLen, (DATATYPE)0);
            cout << "Huffman encoding ... " << flush;
            encodeHuffFromText(2, bwt, bwtLen, this->code, this->codeLen);
            cout << "Done" << endl;
            cout << "Building WT ... " << flush;
            this->wt->createHWT(bwt, bwtLen, this->code, this->codeLen);
            delete[] bwt;
            cout << "Done" << endl;
            fillArrayC(text, this->textLen, this->c);
            delete[] text;
            cout << "Index successfully built" << endl;
        }
        
        void save(FILE *outFile) {
            fwrite(&this->textLen, (size_t)sizeof(DATATYPE), (size_t)1, outFile);
            fwrite(this->c, (size_t)sizeof(DATATYPE), (size_t)257, outFile);
            fwrite(this->code, (size_t)sizeof(unsigned long long), (size_t)256, outFile);
            fwrite(this->codeLen, (size_t)sizeof(unsigned int), (size_t)256, outFile);
            this->wt->save(outFile);
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
            size_t result = fread(&this->textLen, (size_t)sizeof(DATATYPE), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading index" << endl;
                    exit(1);
            }
            result = fread(this->c, (size_t)sizeof(DATATYPE), (size_t)257, inFile);
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
            this->wt->load(inFile);
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
        
	unsigned long long getIndexSize() {
            unsigned long long size = sizeof(this->wt) + sizeof(this->textLen);
            size += (257 * sizeof(DATATYPE) + 256 * sizeof(unsigned int) + 256 * sizeof(unsigned long long));
            if (this->wt != NULL) size += this->wt->getWTSize();
            return size;
        }
        
	DATATYPE getTextSize() {
            return this->textLen * sizeof(unsigned char);
        }

	DATATYPE count(unsigned char *pattern, unsigned int patternLen) {
            return this->countHWT(pattern, patternLen - 1, this->c, this->wt, this->c[pattern[patternLen - 1]] + 1, this->c[pattern[patternLen - 1] + 1], this->code, this->codeLen);
        }
        
        DATATYPE count(unsigned char *pattern, unsigned int pos, DATATYPE firstVal, DATATYPE lastVal) {
            return this->countHWT(pattern, pos, this->c, this->wt, firstVal, lastVal, this->code, this->codeLen);
        }
};

template <class RANK32> using FMHWT32 = FMHWT<WT32<RANK32>, unsigned int>;
template <class RANK64> using FMHWT64 = FMHWT<WT64<RANK64>, unsigned long long>;

template<class WT, typename DATATYPE> class FMHWTHash : public FMHWT<WT,DATATYPE> {
private:
	HTExt<HTType::HT_STANDARD, DATATYPE> *ht = NULL;
        
	void freeMemory() {
            FMHWT<WT,DATATYPE>::freeMemory();
            if (this->ht != NULL) this->ht->free();
        }

public:

	FMHWTHash(unsigned int k, double loadFactor) {
            this->initialize();
            this->ht = new HTExt<HTType::HT_STANDARD, DATATYPE>(k, loadFactor);
	}

	~FMHWTHash() {
            this->free();
            if (this->ht != NULL) delete this->ht;
	}

	void build(const char *textFileName) {
            this->free();
            unsigned char *text = readText(textFileName, this->textLen, (DATATYPE)0);
            checkNullChar(text, this->textLen);
            DATATYPE bwtLen;
            DATATYPE saLen;
            DATATYPE *sa = getSA(textFileName, text, this->textLen, saLen, (DATATYPE)0);
            cout << "Building hash table ... " << flush;
            this->ht->build(text, this->textLen, sa, saLen);
            cout << "Done" << endl;
            unsigned char *bwt = getBWT(text, this->textLen, sa, saLen, bwtLen, (DATATYPE)0);
            delete[] sa;
            cout << "Huffman encoding ... " << flush;
            encodeHuffFromText(2, bwt, bwtLen, this->code, this->codeLen);
            cout << "Done" << endl;
            cout << "Building WT ... " << flush;
            this->wt->createHWT(bwt, bwtLen, this->code, this->codeLen);
            delete[] bwt;
            cout << "Done" << endl;
            fillArrayC(text, this->textLen, this->c);
            delete[] text;
            cout << "Index successfully built" << endl;
        }
        
	void save(FILE *outFile) {
            FMHWT<WT,DATATYPE>::save(outFile);
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
            FMHWT<WT,DATATYPE>::load(inFile);
            delete this->ht;
            this->ht = new HTExt<HTType::HT_STANDARD, DATATYPE>();
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
        
	unsigned long long getIndexSize() {
            return FMHWT<WT,DATATYPE>::getIndexSize() + sizeof(this->ht) + this->ht->getHTSize();
        }

	unsigned int count(unsigned char *pattern, unsigned int patternLen) {
            if (patternLen < this->ht->k) return FMHWT<WT,DATATYPE>::count(pattern, patternLen);
            DATATYPE leftBoundary, rightBoundary;
            this->ht->getBoundaries(pattern + (patternLen - this->ht->k), leftBoundary, rightBoundary);
            return this->countHWT(pattern, patternLen - this->ht->k, this->c, this->wt, leftBoundary + 1, rightBoundary, this->code, this->codeLen);
        }
};

template <class RANK32> using FMHWTHash32 = FMHWTHash<WT32<RANK32>, unsigned int>;
template <class RANK64> using FMHWTHash64 = FMHWTHash<WT64<RANK64>, unsigned long long>;

}

#endif	/* FM_HPP */

