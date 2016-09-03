# Rank&Select library

##What is it?
The Rank&Select library is ...

##Requirements
The Rank&Select library require:
- C++11 ready compiler such as g++ version 4.7 or higher
- a 64-bit operating system
- text size is limited to 512MB (4GB of bits)

##Installation
To download and build the library use the following commands:
```
git clone https://github.com/mranisz/rank-select.git
cd rank-select
make
```

##Usage
To use the Rank&Select library:
- compile your project with "-std=c++11 -O3 -mpopcnt" options and link it with libraries:
  - rank-select/libshared.a
  - rank-select/libs/libaelf64.a (linux) or samsami/libs/libacof64.lib (windows)
- use "shared" namespace

##API (rank and select)
- **build** the rank or select using text and textLen:
```
void build(unsigned char *text, unsigned int textLen);
```
- **save** the rank or select to FILE object:
```
void save(FILE *outFile);
```
- **load** the rank or select from FILE object:
```
void load(FILE *inFile);
```
- **free** memory occupied by rank or select:
```
void free();
```
- get the **size** in bytes (size in memory):
```
unsigned int getSize();
```
- get the size in bytes of the text used to build the rank or select:
```
unsigned int getTextSize();
```
- get the result of **rank** query:
```
unsigned int rank(unsigned int i);
```
- get the result of **select** query:
```
unsigned int select(unsigned int i);
```

##RankBasic\<RankBasicType T\>

Parameters:
- T:
      - RankBasicType::RANK_STANDARD
      - RankBasicType::RANK_WITH_COMPRESSED_HEADERS

Constructors:
```
RankBasic<RankBasicType T>();
```

##RankCF

Constructors:
```
RankCF<RankBasicType T>();
```

##RankMPE\<RankMPEType T\>

Parameters:
- T:
      - RankMPEType::RANK_V1
      - RankMPEType::RANK_V2
      - RankMPEType::RANK_V3

Constructors:
```
RankMPE<RankMPEType T>();
```

##SelectBasic\<SelectBasicType T, unsigned int L, unsigned int THRESHOLD\>

Parameters:
- T:
      - SelectBasicType::SELECT_STANDARD
      - SelectBasicType::SELECT_WITH_COMPRESSED_HEADERS
- L - ...
- THRESHOLD - ...

Limitations: 
- THRESHOLD > L
- L < 1094 and THRESHOLD < 34954 (for SelectBasicType::WITH_COMPRESSED_HEADERS)

Constructors:
```
SelectBasic<SelectBasicType T, unsigned int L, unsigned int THRESHOLD>();
```

##SelectMPE\<SelectMPEType T, unsigned int L, unsigned int THRESHOLD\>

Parameters:
- T:
      - SelectMPEType::SELECT_V1
      - SelectMPEType::SELECT_V2
      - SelectMPEType::SELECT_V3
- L - ...
- THRESHOLD - ...

Limitations: 
- THRESHOLD > L
- L < 548 and THRESHOLD < 17474 (for SelectMPEType::V1)
- L < 275 and THRESHOLD < 8770 (for SelectMPEType::V2 and SelectMPEType::V3)

Constructors:
```
SelectMPE<SelectMPEType T, unsigned int L, unsigned int THRESHOLD>();
```

##Rank usage example
```
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "rank-select/shared/common.h"
#include "rank-select/shared/rank.hpp"

using namespace std;
using namespace shared;

int main(int argc, char *argv[]) {

	RankBasic<RankBasicType::RANK_WITH_COMPRESSED_HEADERS> *rank = new RankBasic<RankBasicType::RANK_WITH_COMPRESSED_HEADERS>();
        const char *textFileName = "english.200MB";
	const char *rankFileName = "english.200MB-bch.rank";

	if (fileExists(rankFileName)) {
		FILE *inFile;
		inFile = fopen(rankFileName, "rb");
		rank->load(inFile);
		fclose(inFile);
	} else {
		unsigned int textLen;
                unsigned char* text = readText(textFileName, textLen, 0);
		rank->build(text, textLen);
                delete[] text;
		FILE *outFile;
		outFile = fopen(rankFileName, "w");
		rank->save(outFile);
		fclose(outFile);
	}
	
	double rankSize = (double)rank->getSize();
	cout << "Rank size: " << rankSize << "B (" << (rankSize / (double)rank->getSize()) << "n)" << endl << endl;

	for (int i = 0; i < 100; ++i) cout << "rank(" << i << ") = " << rank->rank(i) << endl;

	delete rank;
}
```
Using other types of ranks is analogous.

##Select usage example
```
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "rank-select/shared/common.h"
#include "rank-select/shared/select.hpp"

using namespace std;
using namespace shared;

int main(int argc, char *argv[]) {

	SelectMPE<SelectMPEType::SELECT_V2, 128, 4096> *select = new SelectMPE<SelectMPEType::SELECT_V2, 128, 4096>();
        const char *textFileName = "english.200MB";
	const char *selectFileName = "english.200MB-bch.select";

	if (fileExists(selectFileName)) {
		FILE *inFile;
		inFile = fopen(selectFileName, "rb");
		select->load(inFile);
		fclose(inFile);
	} else {
		unsigned int textLen;
                unsigned char* text = readText(textFileName, textLen, 0);
		select->build(text, textLen);
                delete[] text;
		FILE *outFile;
		outFile = fopen(selectFileName, "w");
		select->save(outFile);
		fclose(outFile);
	}
	
	double selectSize = (double)select->getSize();
	cout << "Select size: " << selectSize << "B (" << (selectSize / (double)select->getSize()) << "n)" << endl << endl;

	for (int i = 0; i < 100; ++i) cout << "select(" << i << ") = " << select->select(i) << endl;

	delete select;
}
```
Using other types of selects is analogous.

##API (FMHWT index)
- **build** the index using text file called textFileName:
```
void build(const char *textFileName);
```
- **save** the index to file called fileName:
```
void save(const char *fileName);
```
- **load** the index from file called fileName:
```
void load(const char *fileName);
```
- **free** memory occupied by index:
```
void free();
```
- get the **index size** in bytes (size in memory):
```
unsigned int getIndexSize();
```
- get the size in bytes of the text used to build the index:
```
unsigned int getTextSize();
```
- get the result of **count** query:
```
unsigned int count(unsigned char *pattern, unsigned int patternLen);
```
- set **verbose** mode:
```
void setVerbose(bool verbose);
```

##WT\<class RANK\>
WT is a class representing wavelet tree structure.

Parameters:
- RANK class:
      - RankBasic\<RankBasicType T\>
      - RankCF
      - RankMPE\<RankMPEType T\>

Constructors:
```
WT<class RANK>();
```

##FMHWT\<class WT\>

Parameters:
- WT class

Constructors:
```
FMHWT<class WT>();
```

##FMHWTHash\<class WT\>
FMHWTHash is FMHWT with hashed k-symbol prefixes of suffixes from suffix array to speed up searches (k ≥ 2). This variant is particularly efficient in speed for short patterns (not much longer than k).

Parameters:
- WT class
- k - length of prefixes of suffixes from suffix array
- loadFactor - hash table load factor

Limitations: 
- pattern length ≥ k (patterns shorter than k are handled by standard variant of FMHWT index)
- k ≥ 2
- 0.0 < loadFactor < 1.0

Constructors:
```
FMHWTHash<class WT>();
```

##FMHWT usage example
```
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "rank-select/shared/patterns.h"
#include "rank-select/shared/fm.hpp"

using namespace std;
using namespace shared;

int main(int argc, char *argv[]) {

	unsigned int queriesNum = 1000000;
	unsigned int patternLen = 20;
	FMHWT<WT<RankBasic<RankBasicType::RANK_WITH_COMPRESSED_HEADERS>>> *fm = new FMHWT<WT<RankBasic<RankBasicType::RANK_WITH_COMPRESSED_HEADERS>>>();
	const char *textFileName = "dna";
	const char *indexFileName = "dna-FMHWT.idx";

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->setVerbose(true);
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	double indexSize = (double)fm->getIndexSize();
	cout << "Index size: " << indexSize << "B (" << (indexSize / (double)fm->getTextSize()) << "n)" << endl << endl;

	Patterns *P = new Patterns(textFileName, queriesNum, patternLen);
	unsigned char **patterns = P->getPatterns();

	for (unsigned int i = 0; i < queriesNum; ++i) {
		cout << "Pattern |" << patterns[i] << "| occurs " << fm->count(patterns[i], patternLen) << " times." << endl;
	}

	delete fm;
	delete P;
}
```
Using other types of FMHWT indexes is analogous.

##External resources used in Rank&Select project
- Suffix array building by Yuta Mori (sais)
- A multi-platform library of highly optimized functions for C and C++ by Agner Fog (asmlib)
- A very fast hash function by Yann Collet (xxHash)

##Authors
- Szymon Grabowski
- [Marcin Raniszewski](https://github.com/mranisz)
