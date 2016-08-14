#ifndef SHARED_PATTERNS_H_
#define SHARED_PATTERNS_H_

#include <vector>

using namespace std;

namespace shared {

class Patterns {
private:
	const char *textFileName;
	unsigned int queriesNum;
	unsigned int m;
	vector<unsigned char> selectedChars;
	unsigned char **patterns = NULL;
	unsigned int *counts = NULL;
        vector<unsigned int> *locates = NULL;

	void freeMemory();
        void setQueriesNum(unsigned int queriesNum);
        void setM(unsigned int m);
	void initializePatterns();
	void initializeSACounts();
        void initializeSALocates();
	void setSelectedChars(vector<unsigned char> selectedChars);

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
	unsigned char **getPatterns();
	unsigned int *getSACounts();
        vector<unsigned int> *getSALocates();
	unsigned int getErrorCountsNumber(unsigned int *countsToCheck);
        unsigned int getErrorLocatesNumber(vector<unsigned int> *locatesToCheck);
};

class NegativePatterns {
private:
	const char *textFileName;
	unsigned int queriesNum;
	unsigned int m;
	unsigned char **patterns = NULL;

	void freeMemory();
        void setQueriesNum(unsigned int queriesNum);
        void setM(unsigned int m);
	void initializePatterns();
        unsigned int getSACount(unsigned int *sa, unsigned char *text, unsigned int saLen, unsigned char *pattern, int patternLength);

public:
	NegativePatterns(const char *textFileName, unsigned int queriesNum, unsigned int m) {
		this->textFileName = textFileName;
		this->setQueriesNum(queriesNum);
		this->setM(m);
	}
	~NegativePatterns() {
		this->freeMemory();
	}
	unsigned char **getPatterns();
	unsigned int getErrorCountsNumber(unsigned int *countsToCheck);
        unsigned int getErrorLocatesNumber(vector<unsigned int> *locatesToCheck);
};

class MaliciousPatterns {
private:
	const char *textFileName;
	unsigned int queriesNum;
	unsigned int m;
	vector<unsigned char> selectedChars;
        bool initialized = false;
	unsigned char **patterns = NULL;
	unsigned int *counts = NULL;
        vector<unsigned int> *locates = NULL;

	void freeMemory();
        void setM(unsigned int m);
	void initializePatterns();
	void initializeSACounts();
        void initializeSALocates();
	void setSelectedChars(vector<unsigned char> selectedChars);

public:
	MaliciousPatterns(const char *textFileName, unsigned int m, vector<unsigned char> selectedChars = {}) {
		this->textFileName = textFileName;
		this->setM(m);
		this->setSelectedChars(selectedChars);
	}
	~MaliciousPatterns() {
		this->freeMemory();
	}
	unsigned char **getPatterns();
        unsigned int getQueriesNum();
	unsigned int *getSACounts();
        vector<unsigned int> *getSALocates();
	unsigned int getErrorCountsNumber(unsigned int *countsToCheck);
        unsigned int getErrorLocatesNumber(vector<unsigned int> *locatesToCheck);
};

}

#endif /* SHARED_PATTERNS_H_ */
