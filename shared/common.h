#ifndef SHARED_COMMON_H_
#define SHARED_COMMON_H_

#include <string.h>
#include <vector>

using namespace std;

namespace fmdummy {

class Index {
protected:
	bool verbose = false;
public:
	void setVerbose(bool verbose) {
		this->verbose = verbose;
	}
};

unsigned long long getFileSize(const char *inFileName, int elemSize);
FILE *openFile(const char *inFileName, int elemSize, unsigned int &len);
unsigned char *readFileChar(const char *inFileName, unsigned int &len, unsigned int addLen);
unsigned int *readFileInt(const char *inFileName, unsigned int &len, unsigned int addLen);
unsigned long long *readFileLong(const char *inFileName, unsigned int &len, unsigned int addLen);
unsigned char **readFilePatterns(const char *inFileName, unsigned int m, unsigned int queriesNum);
bool fileExists(const char *inFileName);
unsigned char *readText(const char *inFileName, unsigned int &textLen, unsigned char eof);
void checkNullChar(unsigned char *text, unsigned int textLen);
unsigned int *getSA(unsigned char *text, unsigned int textLen, unsigned int &saLen, unsigned int addLen, bool verbose);
unsigned int *getSA(const char *textFileName, unsigned int &saLen, unsigned int addLen, bool verbose);
unsigned int *getSA(const char *textFileName, unsigned char *text, unsigned int textLen, unsigned int &saLen, unsigned int addLen, bool verbose);
unsigned char *getBWT(unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen, unsigned int &bwtLen, unsigned int addLen, bool verbose);
unsigned char *getBWT(unsigned char *text, unsigned int textLen, unsigned int &bwtLen, unsigned int addLen, bool verbose);
unsigned char *getBWT(const char *textFileName, unsigned char *text, unsigned int textLen, unsigned int &bwtLen, unsigned int addLen, bool verbose);
void fillArrayC(unsigned char *text, unsigned int textLen, unsigned int* C, bool verbose);
string getStringFromSelectedChars(vector<unsigned char> selectedChars, string separator);
void binarySearch(unsigned int *sa, unsigned char *text, unsigned int lStart, unsigned int rStart, unsigned char *pattern, int patternLength, unsigned int &beg, unsigned int &end);
void binarySearchAStrcmp(unsigned int *sa, unsigned char *text, unsigned int lStart, unsigned int rStart, unsigned char *pattern, int patternLength, unsigned int &beg, unsigned int &end);
void binarySearchStrncmp(unsigned int *sa, unsigned char *text, unsigned int lStart, unsigned int rStart, unsigned char *pattern, int patternLength, unsigned int &beg, unsigned int &end);
unsigned int getSACount(unsigned int *sa, unsigned char *text, unsigned int saLen, unsigned char *pattern, int patternLength);
void getSALocate(unsigned int *sa, unsigned char *text, unsigned int saLen, unsigned char *pattern, int patternLength, vector<unsigned int>& res);
void fillLUT1(unsigned int lut1[256][2], unsigned char *text, unsigned int *sa, unsigned int saLen);
void fillLUT2(unsigned int lut2[256][256][2], unsigned char *text, unsigned int *sa, unsigned int saLen);
void encode(unsigned char *pattern, unsigned int patternLen, unsigned char *encodedChars, unsigned int *encodedCharsLen, unsigned int maxEncodedCharsLen, unsigned char *encodedPattern, unsigned int &encodedPatternLen);

}

#endif /* SHARED_COMMON_H_ */
