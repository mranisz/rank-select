#ifndef SHARED_WT_H_
#define SHARED_WT_H_

namespace fmdummy {

class WT {
private:
	void freeMemory();
	void initialize();

public:
	unsigned long long *bits;
	unsigned long long *alignedBits;
	unsigned int bitsLen;
	WT** nodes;
	unsigned int nodesLen;
        
        enum Type {
		TYPE_512 = 8,
		TYPE_1024 = 16
	};

	WT() {
		this->initialize();
	}

	WT(int wtType) {
		this->initialize();
		this->nodesLen = wtType;
		this->nodes = new WT *[this->nodesLen];
	};

	unsigned int getWTSize();
	void save(FILE *outFile);
	void load(FILE *inFile);
	void free();

	~WT() {
		this->freeMemory();
	}
};

WT *createWT2_512_counter40(unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen);
WT *createWT2_1024_counter32(unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen);
WT *createWT4(int type, unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen);
WT *createWT8(int type, unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen);
unsigned int count_WT2_512_counter40(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen);
unsigned int count_WT2_1024_counter32(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen);
unsigned int count_WT4_512(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen);
unsigned int count_WT4_1024(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen);
unsigned int count_WT8_512(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen);
unsigned int count_WT8_1024(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen);

}

#endif /* SHARED_WT_H_ */
