#ifndef SHARED_HASH_H_
#define SHARED_HASH_H_
    
namespace shared {
    
/*HTBASE*/

class HTBase {
protected:
	void freeMemory();
	void initialize();
        void setType(int type);
	void setLoadFactor(double loadFactor);
	void setK(unsigned int k);
        
public:
        enum HTType {
		STANDARD = 1,
		DENSE = 2
	};
        
        int type;
	double loadFactor;
	unsigned int k;
        unsigned int prefixLength;
	unsigned long long bucketsNum;

	alignas(128) unsigned int lut2[256][256][2];
	unsigned int *boundariesHT;
	unsigned int *alignedBoundariesHT;
        unsigned long long boundariesHTLen;
        unsigned int *denseBoundariesHT;
        unsigned int *alignedDenseBoundariesHT;
        unsigned long long denseBoundariesHTLen;
        
	const static unsigned int emptyValueHT;

        unsigned long long getHashValue(unsigned char* str, unsigned int strLen);
	unsigned int getHTSize();
	void save(FILE *outFile);
	void loadBase(FILE *inFile);
	void free();
};
    
/*HT*/

class HT : public HTBase {
private:
        void setFunctions();
	void fillStandardHTData(unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen);
        void fillDenseHTData(unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen);
        void getStandardHTBoundaries(unsigned char *pattern, unsigned char *text, unsigned int *sa, unsigned int &leftBoundary, unsigned int &rightBoundary);
        void getDenseHTBoundaries(unsigned char *pattern, unsigned char *text, unsigned int *sa, unsigned int &leftBoundary, unsigned int &rightBoundary);
        
        void (HT::*getBoundariesOperation)(unsigned char *, unsigned char *, unsigned int *, unsigned int &, unsigned int &) = NULL;
        
public:

	HT() {
                this->initialize();
                this->setType(HTBase::STANDARD);
		this->setK(8);
		this->setLoadFactor(0.9);
                this->setFunctions();
	}

	HT(HTBase::HTType type, unsigned int k, double loadFactor) {
		this->initialize();
                this->setType(type);
		this->setK(k);
		this->setLoadFactor(loadFactor);
                this->setFunctions();
	};

        void build(unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen);
        void load(FILE *inFile);
        void getBoundaries(unsigned char *pattern, unsigned char *text, unsigned int *sa, unsigned int &leftBoundary, unsigned int &rightBoundary);

	~HT() {
		this->freeMemory();
	}
};

/*HTEXT*/

class HTExt {
private:
	void freeMemory();
	void initialize();
        void setType(int type);
	void setLoadFactor(double loadFactor);
	void setK(unsigned int k);
        void setFunctions();
        unsigned long long getHashValue(unsigned char* str, unsigned int strLen);
	void fillStandardHTData(unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen, vector<unsigned char> selectedChars = {}, unsigned char *cutOutEntries = NULL);
        void fillDenseHTData(unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen, vector<unsigned char> selectedChars = {});
        void getStandardHTBoundaries(unsigned char *pattern, unsigned int &leftBoundary, unsigned int &rightBoundary);
        void getDenseHTBoundaries(unsigned char *pattern, unsigned int &leftBoundary, unsigned int &rightBoundary);
        
        void (HTExt::*getBoundariesOperation)(unsigned char *, unsigned int &, unsigned int &) = NULL;

public:
        enum HTExtType {
                STANDARD = 1,
                DENSE = 2
	};
        
        int type;
	double loadFactor;
	unsigned int k;
        unsigned int prefixLength;
	unsigned long long bucketsNum;

	alignas(128) unsigned int lut2[256][256][2];
	unsigned int *boundariesHT;
	unsigned int *alignedBoundariesHT;
        unsigned long long boundariesHTLen;
        unsigned int *denseBoundariesHT;
        unsigned int *alignedDenseBoundariesHT;
        unsigned long long denseBoundariesHTLen;
	unsigned char *entriesHT;
	unsigned char *alignedEntriesHT;
        unsigned long long entriesHTLen;
        

	const static unsigned int emptyValueHT;
        const static unsigned int emptyValueDenseHT;

	HTExt() {
		this->initialize();
                this->setType(HTExt::STANDARD);
		this->setK(8);
		this->setLoadFactor(0.9);
                this->setFunctions();
	}

	HTExt(HTExt::HTExtType type, unsigned int k, double loadFactor) {
		this->initialize();
                this->setType(type);
		this->setK(k);
		this->setLoadFactor(loadFactor);
                this->setFunctions();
	};

	unsigned int getHTSize();
        
        void build(unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen, vector<unsigned char> selectedChars = {}, unsigned char *cutOutEntries = NULL);
        void getBoundaries(unsigned char *pattern, unsigned int &leftBoundary, unsigned int &rightBoundary);
	void save(FILE *outFile);
	void load(FILE *inFile);
	void free();

	~HTExt() {
		this->freeMemory();
	}
};

/*SHARED STUFF*/

unsigned int getUniqueSuffixNum(unsigned int k, unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen, vector<unsigned char> selectedChars = {});

}

#endif /* SHARED_HASH_H_ */
