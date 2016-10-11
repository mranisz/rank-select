#ifndef COMMON_HPP
#define	COMMON_HPP

#include <string.h>
#include <fstream>
#include <vector>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include "sais.h"
#include "../libs/asmlib.h"

using namespace std;

namespace shared {
    
unsigned long long getFileSize(const char *inFileName, int elemSize) {
	FILE *InFile;
	InFile = fopen(inFileName, "rb");
	if (InFile == NULL) {
		cout << "Can't open file " << inFileName << endl;
		exit(1);
	}
	if (fseek(InFile, 0, SEEK_END) != 0) {
		cout << "Something is wrong with file " << inFileName << endl;
		exit(1);
	}
	long long fileSize = ftell(InFile);
	if (fileSize == -1) {
		cout << "Something is wrong with file " << inFileName << endl;
		exit(1);
	}
	fclose(InFile);
	return fileSize / elemSize;
}

template<typename DATATYPE> FILE *openFile(const char *inFileName, int elemSize, DATATYPE &len) {
	FILE *InFile;
	InFile = fopen(inFileName, "rb");
	if (InFile == NULL) {
		cout << "Can't open file " << inFileName << endl;
		exit(1);
	}
	if (fseek(InFile, 0, SEEK_END) != 0) {
		cout << "Something is wrong with file " << inFileName << endl;
		exit(1);
	}
	long long fileSize = ftell(InFile);
	if (fileSize == -1) {
		cout << "Something is wrong with file " << inFileName << endl;
		exit(1);
	}
	if (fileSize == 0) {
		cout << "File " << inFileName << " is empty." << endl;
		exit(1);
	}
	rewind(InFile);
	len = fileSize / elemSize;
	return InFile;
}

template FILE *openFile<unsigned int>(const char *inFileName, int elemSize, unsigned int &len);
template FILE *openFile<unsigned long long>(const char *inFileName, int elemSize, unsigned long long &len);

template<typename DATATYPE> unsigned char *readFileChar(const char *inFileName, DATATYPE &len, DATATYPE addLen) {
	FILE *InFile = openFile(inFileName, 1, len);
	unsigned char *S = new unsigned char[len + addLen];
	if (fread(S, (size_t)1, (size_t)len, InFile) != (size_t)len) {
		cout << "Error reading file " << inFileName << endl;
		exit(1);
	}
	fclose(InFile);
	return S;
}

template unsigned char *readFileChar<unsigned int>(const char *inFileName, unsigned int &len, unsigned int addLen);
template unsigned char *readFileChar<unsigned long long>(const char *inFileName, unsigned long long &len, unsigned long long addLen);

template<typename DATATYPE> DATATYPE *readFile(const char *inFileName, DATATYPE &len, DATATYPE addLen) {
	FILE *InFile = openFile(inFileName, sizeof(DATATYPE), len);
	DATATYPE *S = new DATATYPE[len + addLen];
	if (fread(S, (size_t)sizeof(DATATYPE), (size_t)len, InFile) != (size_t)len) {
		cout << "Error reading file " << inFileName << endl;
		exit(1);
	}
	fclose(InFile);
	return S;
}

template unsigned int *readFile<unsigned int>(const char *inFileName, unsigned int &len, unsigned int addLen);
template unsigned long long *readFile<unsigned long long>(const char *inFileName, unsigned long long &len, unsigned long long addLen);

unsigned char **readFilePatterns(const char *inFileName, unsigned int m, unsigned int queriesNum) {
        unsigned int len = 0;
        FILE *InFile = openFile(inFileName, 1, len);
        if (len != m * queriesNum) {
            cout << "Error reading file " << inFileName << endl;
            exit(1);
        }
	unsigned char **patterns = new unsigned char *[queriesNum];
        for (unsigned int i = 0; i < queriesNum; ++i) {
                patterns[i] = new unsigned char[m + 1];
                patterns[i][m] = '\0';
                if (fread(patterns[i], (size_t)sizeof(unsigned char), (size_t)m, InFile) != (size_t)m) {
                        cout << "Error reading file " << inFileName << endl;
                        exit(1);
                }
        }
	fclose(InFile);
	return patterns;
}

bool fileExists(const char *inFileName) {
	FILE *InFile;
	InFile = fopen(inFileName, "rb");
	if (InFile == NULL) {
		return false;
	}
	if (InFile == NULL) {
		return false;
	}
	if (fseek(InFile, 0, SEEK_END) != 0) {
		return false;
	}
	long long fileSize = ftell(InFile);
	if (fileSize == -1) {
		return false;
	}
	if (fileSize == 0) {
		return false;
	}
	fclose(InFile);
	return true;
}

template<typename DATATYPE> unsigned char *readText(const char *inFileName, DATATYPE &textLen, unsigned char eof) {
	unsigned char *S = readFileChar(inFileName, textLen, (DATATYPE)1);
	S[textLen] = eof;
	return S;
}

template unsigned char *readText<unsigned int>(const char *inFileName, unsigned int &textLen, unsigned char eof);
template unsigned char *readText<unsigned long long>(const char *inFileName, unsigned long long &textLen, unsigned char eof);

template<typename DATATYPE> void checkNullChar(unsigned char *text, DATATYPE textLen) {
	for (DATATYPE i = 0; i < textLen; ++i) {
		if (text[i] == '\0') {
			cout << "Error: text contains at least one 0 character" << endl;
			exit(1);
		}
	}
}

template void checkNullChar<unsigned int>(unsigned char *text, unsigned int textLen);
template void checkNullChar<unsigned long long>(unsigned char *text, unsigned long long textLen);

unsigned int *getSA(unsigned char *text, unsigned int textLen, unsigned int &saLen, unsigned int addLen) {
	saLen = textLen + 1;
	cout << "Building SA32 ... " << flush;
	unsigned int *sa = new unsigned int[saLen + addLen];
	sa[0] = textLen;
	++sa;
	sais(text, sa, textLen);
	--sa;
	cout << "Done" << endl;
	return sa;
}

unsigned long long *getSA(unsigned char *text, unsigned long long textLen, unsigned long long &saLen, unsigned long long addLen) {
	saLen = textLen + 1;
	cout << "Building SA64 ... " << flush;
	unsigned long long *sa = new unsigned long long[saLen + addLen];
	sa[0] = textLen;
	++sa;
	sais64(text, sa, textLen);
	--sa;
	cout << "Done" << endl;
	return sa;
}

template<typename DATATYPE> DATATYPE *getSA(const char *textFileName, DATATYPE &saLen, DATATYPE addLen) {
        int bitType = sizeof(DATATYPE) * 8;
	stringstream ss;
	ss << "SA" << bitType << "-" << textFileName << ".dat";
	string s = ss.str();
	char *saFileName = (char *)(s.c_str());
	DATATYPE *sa;
	if (!fileExists(saFileName)) {
                DATATYPE textLen;
                unsigned char *text = readFileChar(textFileName, textLen, (DATATYPE)0);
                sa = getSA(text, textLen, saLen, addLen);
                delete[] text;
                cout << "Saving SA" << bitType << " in " << saFileName << " ... " << flush;
                FILE *outFile;
                outFile = fopen(saFileName, "w");
                fwrite(sa, (size_t)(sizeof(DATATYPE)), (size_t)saLen, outFile);
                fclose(outFile);
	} 
	else {
                cout << "Loading SA" << bitType << " from " << saFileName << " ... " << flush;
                sa = readFile(saFileName, saLen, addLen);
	}
	cout << "Done" << endl;
	return sa;
}

template unsigned int *getSA<unsigned int>(const char *textFileName, unsigned int &saLen, unsigned int addLen);
template unsigned long long *getSA<unsigned long long>(const char *textFileName, unsigned long long &saLen, unsigned long long addLen);

template<typename DATATYPE> DATATYPE *getSA(const char *textFileName, unsigned char *text, DATATYPE textLen, DATATYPE &saLen, DATATYPE addLen) {
	int bitType = sizeof(DATATYPE) * 8;
        stringstream ss;
	ss << "SA" << bitType << "-" << textFileName << ".dat";
	string s = ss.str();
	char *saFileName = (char *)(s.c_str());
	DATATYPE *sa;
	if (!fileExists(saFileName)) {
                sa = getSA(text, textLen, saLen, addLen);
                cout << "Saving SA" << bitType << " in " << saFileName << " ... " << flush;
                FILE *outFile;
                outFile = fopen(saFileName, "w");
                fwrite(sa, (size_t)(sizeof(DATATYPE)), (size_t)saLen, outFile);
                fclose(outFile);
	} 
	else {
                cout << "Loading SA" << bitType << " from " << saFileName << " ... " << flush;
                sa = readFile(saFileName, saLen, addLen);
	}
	cout << "Done" << endl;
	return sa;
}

template unsigned int *getSA<unsigned int>(const char *textFileName, unsigned char *text, unsigned int textLen, unsigned int &saLen, unsigned int addLen);
template unsigned long long *getSA<unsigned long long>(const char *textFileName, unsigned char *text, unsigned long long textLen, unsigned long long &saLen, unsigned long long addLen);

template<typename DATATYPE> unsigned char *getBWT(unsigned char *text, DATATYPE textLen, DATATYPE *sa, DATATYPE saLen, DATATYPE &bwtLen, DATATYPE addLen) {
	cout << "Building BWT ... " << flush;
	bwtLen = textLen + 1;
	unsigned char *bwt = new unsigned char[bwtLen + 1 + addLen];
	bwt[bwtLen + addLen] = '\0';
	bwt[0] = text[textLen - 1];
	for (DATATYPE i = 1; i < saLen; ++i) {
		if (sa[i] == 0) bwt[i] = '\0';
		else bwt[i] = text[sa[i] - 1];
	}
	cout << "Done" << endl;
	return bwt;
}

template unsigned char *getBWT<unsigned int>(unsigned char *text, unsigned int textLen, unsigned int *sa, unsigned int saLen, unsigned int &bwtLen, unsigned int addLen);
template unsigned char *getBWT<unsigned long long>(unsigned char *text, unsigned long long textLen, unsigned long long *sa, unsigned long long saLen, unsigned long long &bwtLen, unsigned long long addLen);

template<typename DATATYPE> unsigned char *getBWT(unsigned char *text, DATATYPE textLen, DATATYPE &bwtLen, DATATYPE addLen) {
	DATATYPE saLen;
	DATATYPE *sa = getSA(text, textLen, saLen, (DATATYPE)0);
	unsigned char *bwt = getBWT(text, textLen, sa, saLen, bwtLen, addLen);
	delete[] sa;
	return bwt;
}

template unsigned char *getBWT<unsigned int>(unsigned char *text, unsigned int textLen, unsigned int &bwtLen, unsigned int addLen);
template unsigned char *getBWT<unsigned long long>(unsigned char *text, unsigned long long textLen, unsigned long long &bwtLen, unsigned long long addLen);

template<typename DATATYPE> unsigned char *getBWT(const char *textFileName, unsigned char *text, DATATYPE textLen, DATATYPE &bwtLen, DATATYPE addLen) {
	DATATYPE saLen;
	DATATYPE *sa = getSA(textFileName, text, textLen, saLen, (DATATYPE)0);
	unsigned char *bwt = getBWT(text, textLen, sa, saLen, bwtLen, addLen);
	delete[] sa;
	return bwt;
}

template unsigned char *getBWT<unsigned int>(const char *textFileName, unsigned char *text, unsigned int textLen, unsigned int &bwtLen, unsigned int addLen);
template unsigned char *getBWT<unsigned long long>(const char *textFileName, unsigned char *text, unsigned long long textLen, unsigned long long &bwtLen, unsigned long long addLen);

template<typename DATATYPE> void fillArrayC(unsigned char *text, DATATYPE textLen, DATATYPE* C) {
	cout << "Building array C ... " << flush;
	for (int i = 0; i < 257; ++i) C[i] = 0;
	for (DATATYPE i = 0; i < textLen; ++i) {
		++C[text[i] + 1];
	}
	C[1] = 1;
	for (int i = 0; i < 256; ++i) {
		C[i + 1] += C[i];
	}
	cout << "Done" << endl;
}

template void fillArrayC<unsigned int>(unsigned char *text, unsigned int textLen, unsigned int* C);
template void fillArrayC<unsigned long long>(unsigned char *text, unsigned long long textLen, unsigned long long* C);

string getStringFromSelectedChars(vector<unsigned char> selectedChars, string separator) {
	if (selectedChars.size() == 0) return "all";
	stringstream ss;
	for (vector<unsigned char>::iterator it = selectedChars.begin(); it != selectedChars.end(); ++it) {
		ss << (unsigned int)(*it);
		if (it + 1 != selectedChars.end()) ss << separator;
	}
	string s = ss.str();
	return s;
}

template<typename DATATYPE> void binarySearchAStrcmp(DATATYPE *sa, unsigned char *text, DATATYPE lStart, DATATYPE rStart, unsigned char *pattern, int patternLength, DATATYPE &beg, DATATYPE &end) {
	DATATYPE l = lStart;
	DATATYPE r = rStart;
	DATATYPE mid;
	while (l < r) {
		mid = (l + r) / 2;
		if (A_strcmp((const char*)pattern, (const char*)(text + sa[mid])) > 0) {
			l = mid + 1;
		}
		else {
			r = mid;
		}
	}
	beg = l;
	r = rStart;
	++pattern[patternLength - 1];
	while (l < r) {
		mid = (l + r) / 2;
		if (A_strcmp((const char*)pattern, (const char*)(text + sa[mid])) <= 0) {
			r = mid;
		}
		else {
			l = mid + 1;
		}
	}
	--pattern[patternLength - 1];
	end = r;
}

template void binarySearchAStrcmp<unsigned int>(unsigned int *sa, unsigned char *text, unsigned int lStart, unsigned int rStart, unsigned char *pattern, int patternLength, unsigned int &beg, unsigned int &end);
template void binarySearchAStrcmp<unsigned long long>(unsigned long long *sa, unsigned char *text, unsigned long long lStart, unsigned long long rStart, unsigned char *pattern, int patternLength, unsigned long long &beg, unsigned long long &end);

template<typename DATATYPE> void binarySearchStrncmp(DATATYPE *sa, unsigned char *text, DATATYPE lStart, DATATYPE rStart, unsigned char *pattern, int patternLength, DATATYPE &beg, DATATYPE &end) {
	DATATYPE l = lStart;
	DATATYPE r = rStart;
	DATATYPE mid;
	while (l < r) {
		mid = (l + r) / 2;
		if (strncmp((const char*)pattern, (const char*)(text + sa[mid]), patternLength) > 0) {
			l = mid + 1;
		}
		else {
			r = mid;
		}
	}
	beg = l;
	r = rStart;
	while (l < r) {
		mid = (l + r) / 2;
		if (strncmp((const char*)pattern, (const char*)(text + sa[mid]), patternLength) < 0) {
			r = mid;
		}
		else {
			l = mid + 1;
		}
	}
	end = r;
}

template void binarySearchStrncmp<unsigned int>(unsigned int *sa, unsigned char *text, unsigned int lStart, unsigned int rStart, unsigned char *pattern, int patternLength, unsigned int &beg, unsigned int &end);
template void binarySearchStrncmp<unsigned long long>(unsigned long long *sa, unsigned char *text, unsigned long long lStart, unsigned long long rStart, unsigned char *pattern, int patternLength, unsigned long long &beg, unsigned long long &end);

template<typename DATATYPE> void binarySearch(DATATYPE *sa, unsigned char *text, DATATYPE lStart, DATATYPE rStart, unsigned char *pattern, int patternLength, DATATYPE &beg, DATATYPE &end) {
	if (pattern[patternLength - 1] == 255) binarySearchStrncmp(sa, text, lStart, rStart, pattern, patternLength, beg, end);
	else binarySearchAStrcmp(sa, text, lStart, rStart, pattern, patternLength, beg, end);
}

template void binarySearch<unsigned int>(unsigned int *sa, unsigned char *text, unsigned int lStart, unsigned int rStart, unsigned char *pattern, int patternLength, unsigned int &beg, unsigned int &end);
template void binarySearch<unsigned long long>(unsigned long long *sa, unsigned char *text, unsigned long long lStart, unsigned long long rStart, unsigned char *pattern, int patternLength, unsigned long long &beg, unsigned long long &end);

template<typename DATATYPE> DATATYPE getSACount(DATATYPE *sa, unsigned char *text, DATATYPE saLen, unsigned char *pattern, int patternLength) {
	DATATYPE beg = 0, end = 0;
	binarySearch(sa, text, (DATATYPE)0, saLen, pattern, patternLength, beg, end);
	return end - beg;
}

template unsigned int getSACount<unsigned int>(unsigned int *sa, unsigned char *text, unsigned int saLen, unsigned char *pattern, int patternLength);
template unsigned long long getSACount<unsigned long long>(unsigned long long *sa, unsigned char *text, unsigned long long saLen, unsigned char *pattern, int patternLength);

template<typename DATATYPE> void getSALocate(DATATYPE *sa, unsigned char *text, DATATYPE saLen, unsigned char *pattern, int patternLength, vector<DATATYPE>& res) {
	DATATYPE beg = 0, end = 0;
	binarySearch(sa, text, (DATATYPE)0, saLen, pattern, patternLength, beg, end);
        res.insert(res.end(), sa + beg, sa + end); 
}

template void getSALocate<unsigned int>(unsigned int *sa, unsigned char *text, unsigned int saLen, unsigned char *pattern, int patternLength, vector<unsigned int>& res);
template void getSALocate<unsigned long long>(unsigned long long *sa, unsigned char *text, unsigned long long saLen, unsigned char *pattern, int patternLength, vector<unsigned long long>& res);

template<typename DATATYPE> void fillLUT1(DATATYPE lut1[256][2], unsigned char *text, DATATYPE *sa, DATATYPE saLen) {
	unsigned char lutPattern[2];
	lutPattern[1] = '\0';
	for (int i = 0; i < 256; ++i) {
		lutPattern[0] = (unsigned char)i;
		binarySearch(sa, text, (DATATYPE)0, saLen, lutPattern, 1, lut1[i][0], lut1[i][1]);
	}
}

template void fillLUT1<unsigned int>(unsigned int lut1[256][2], unsigned char *text, unsigned int *sa, unsigned int saLen);
template void fillLUT1<unsigned long long>(unsigned long long lut1[256][2], unsigned char *text, unsigned long long *sa, unsigned long long saLen);

template<typename DATATYPE> void fillLUT2(DATATYPE lut2[256][256][2], unsigned char *text, DATATYPE *sa, DATATYPE saLen) {
	unsigned char lutPattern[3];
	lutPattern[2] = '\0';
	for (int i = 0; i < 256; ++i) {
		lutPattern[0] = (unsigned char)i;
		for (int j = 0; j < 256; ++j) {
			lutPattern[1] = (unsigned char)j;
			binarySearch(sa, text, (DATATYPE)0, saLen, lutPattern, 2, lut2[i][j][0], lut2[i][j][1]);
		}
	}
}

template void fillLUT2<unsigned int>(unsigned int lut2[256][256][2], unsigned char *text, unsigned int *sa, unsigned int saLen);
template void fillLUT2<unsigned long long>(unsigned long long lut2[256][256][2], unsigned char *text, unsigned long long *sa, unsigned long long saLen);

void encode(unsigned char *pattern, unsigned int patternLen, unsigned char *encodedChars, unsigned int *encodedCharsLen, unsigned int maxEncodedCharsLen, unsigned char *encodedPattern, unsigned int &encodedPatternLen) {
	unsigned char* p = pattern;
	encodedPatternLen = 0;
	for (; p < pattern + patternLen; ++p) {
		for (unsigned int i = 0; i < encodedCharsLen[*p]; ++i) encodedPattern[encodedPatternLen++] = encodedChars[(unsigned int)(*p) * maxEncodedCharsLen + i];
	}
	encodedPattern[encodedPatternLen] = '\0';
}
    
}


#endif	/* COMMON_HPP */

