# Rank&Select library

## What is it?
The Rank&Select library contains several implementations of the primitive operations rank and select on binary sequences. The operation rank(B, i) (which can also be called rank_1(B, i)) returns the number of set bits in B[1..i], where B is a binary sequence, while select(B, j) tells the position of the j-th set bit in B (that is, returns such i that rank(B, i) = j and rank(B, i - 1) = j - 1).

Note that rank_0(B, i) = i - rank_1(B, i), yet rank_0 is not yet directly supported in our API. There is no similar relation between select_0 and select_1.

Efficient rank/select implementations replace B with a compressed/compact representation while still answering queries fast. Our implementations are described in \[[1](#references)\]

Additionally, we incorporated our rank implementations into a standard FM-index with a Huffman-shaped wavelet tree.

## Requirements
The Rank&Select library require:
- C++11 ready compiler such as g++ version 4.7 or higher
- a 64-bit operating system
- text size < 512MB (4GB of bits) for 32bit versions of rank, select and FMHWT

## Installation
To download and build the library use the following commands:
```
git clone https://github.com/mranisz/rank-select.git
cd rank-select
make
```

## Usage
To use the Rank&Select library:
- compile your project with "-std=c++11 -O3 -mpopcnt" options and link it with libraries:
  - rank-select/libshared.a
  - rank-select/libs/libaelf64.a (linux) or rank-select/libs/libacof64.lib (windows)
- use "shared" namespace

## API (32bit rank and 32bit select)
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
unsigned long long getSize();
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

## API (64bit rank and 64bit select)
- **build** the rank or select using text and textLen:
```
void build(unsigned char *text, unsigned long long textLen);
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
unsigned long long getSize();
```
- get the size in bytes of the text used to build the rank or select:
```
unsigned long long getTextSize();
```
- get the result of **rank** query:
```
unsigned long long rank(unsigned long long i);
```
- get the result of **select** query:
```
unsigned long long select(unsigned long long i);
```

## RankBasic32\<RankBasicType T\>

Parameters:
- T:
      - RANK_BASIC_STANDARD
      - RANK_BASIC_COMPRESSED_HEADERS

Constructors:
```
RankBasic32<RankBasicType T>();
```

## RankCF32

Constructors:
```
RankCF32<RankBasicType T>();
```

## RankMPE32\<RankMPEType T\>

Parameters:
- T:
      - RANK_MPE1
      - RANK_MPE2
      - RANK_MPE3

Constructors:
```
RankMPE32<RankMPEType T>();
```

## SelectBasic32\<SelectBasicType T, unsigned int L, unsigned int THRESHOLD\>

Parameters:
- T:
      - SELECT_BASIC_STANDARD
      - SELECT_BASIC_COMPRESSED_HEADERS
- L and THRESHOLD - some "internal" parameters, which in practice can be set to (L = 128, THRESHOLD = 4096) or (512, 8192); more details can be found in Sect. 3.2 of \[[1](#references)\]

Limitations: 
- THRESHOLD > L
- L < 1094 and THRESHOLD < 34954 (for SELECT_BASIC_COMPRESSED_HEADERS)

Constructors:
```
SelectBasic32<SelectBasicType T, unsigned int L, unsigned int THRESHOLD>();
```

## SelectMPE32\<SelectMPEType T, unsigned int L, unsigned int THRESHOLD\>

Parameters:
- T:
      - SELECT_MPE1
      - SELECT_MPE2
      - SELECT_MPE3
- L and THRESHOLD - some "internal" parameters, which in practice can be set to (L = 128, THRESHOLD = 4096); more details can be found in Sect. 3.2 of \[[1](#references)\]

Limitations: 
- THRESHOLD > L
- L < 548 and THRESHOLD < 17474 (for SELECT_MPE1)
- L < 275 and THRESHOLD < 8770 (for SELECT_MPE2 or SELECT_MPE3)

Constructors:
```
SelectMPE32<SelectMPEType T, unsigned int L, unsigned int THRESHOLD>();
```

## RankBasic64\<RankBasicType T\>

Parameters:
- T:
      - RANK_BASIC_STANDARD
      - RANK_BASIC_COMPRESSED_HEADERS

Constructors:
```
RankBasic64<RankBasicType T>();
```

## RankCF64

Constructors:
```
RankCF64<RankBasicType T>();
```

## RankMPE64\<RankMPEType T\>

Parameters:
- T:
      - RANK_MPE1
      - RANK_MPE2
      - RANK_MPE3

Constructors:
```
RankMPE64<RankMPEType T>();
```

## SelectBasic64\<SelectBasicType T, unsigned int L, unsigned int THRESHOLD\>

Parameters:
- T:
      - SELECT_BASIC_STANDARD
      - SELECT_BASIC_COMPRESSED_HEADERS
- L and THRESHOLD - some "internal" parameters, which in practice can be set to (L = 128, THRESHOLD = 4096) or (512, 8192); more details can be found in Sect. 3.2 of \[[1](#references)\]

Limitations: 
- THRESHOLD > L
- L < 547 and THRESHOLD < 34954 (for SELECT_BASIC_COMPRESSED_HEADERS)

Constructors:
```
SelectBasic64<SelectBasicType T, unsigned int L, unsigned int THRESHOLD>();
```

## SelectMPE64\<SelectMPEType T, unsigned int L, unsigned int THRESHOLD\>

Parameters:
- T:
      - SELECT_MPE1
      - SELECT_MPE2
      - SELECT_MPE3
- L and THRESHOLD - some "internal" parameters, which in practice can be set to (L = 128, THRESHOLD = 4096); more details can be found in Sect. 3.2 of \[[1](#references)\]

Limitations: 
- THRESHOLD > L
- L < 275 and THRESHOLD < 17474 (for SELECT_MPE1)
- L < 138 and THRESHOLD < 8770 (for SELECT_MPE2 or SELECT_MPE3)

Constructors:
```
SelectMPE64<SelectMPEType T, unsigned int L, unsigned int THRESHOLD>();
```

## Rank usage example
```
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "rank-select/shared/common.hpp"
#include "rank-select/shared/rank.hpp"

using namespace std;
using namespace shared;

int main(int argc, char *argv[]) {

	RankBasic32<RANK_BASIC_COMPRESSED_HEADERS> *rank = new RankBasic32<RANK_BASIC_COMPRESSED_HEADERS>();
	const char *textFileName = "english.200MB";
	const char *rankFileName = "english.200MB-bch-32.rank";

	if (fileExists(rankFileName)) {
		FILE *inFile = fopen(rankFileName, "rb");
		rank->load(inFile);
		fclose(inFile);
	} else {
		unsigned int textLen;
		unsigned char* text = readText(textFileName, textLen, 0);
		rank->build(text, textLen);
		delete[] text;
		FILE *outFile = fopen(rankFileName, "w");
		rank->save(outFile);
		fclose(outFile);
	}
	
	double rankSize = (double)rank->getSize();
	cout << "Rank size: " << rankSize << "B (" << (rankSize / (double)rank->getSize()) << "n)" << endl << endl;

	for (int i = 0; i < 100; ++i) cout << "rank(" << i << ") = " << rank->rank(i) << endl;

	delete rank;
}
```
Using other types of rank is analogous.

## Select usage example
```
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "rank-select/shared/common.hpp"
#include "rank-select/shared/select.hpp"

using namespace std;
using namespace shared;

int main(int argc, char *argv[]) {

	SelectMPE32<SELECT_MPE2, 128, 4096> *select = new SelectMPE32<SELECT_MPE2, 128, 4096>();
	const char *textFileName = "english.200MB";
	const char *selectFileName = "english.200MB-bch-32.select";

	if (fileExists(selectFileName)) {
		FILE *inFile = fopen(selectFileName, "rb");
		select->load(inFile);
		fclose(inFile);
	} else {
		unsigned int textLen;
		unsigned char* text = readText(textFileName, textLen, 0);
		select->build(text, textLen);
		delete[] text;
		FILE *outFile = fopen(selectFileName, "w");
		select->save(outFile);
		fclose(outFile);
	}
	
	double selectSize = (double)select->getSize();
	cout << "Select size: " << selectSize << "B (" << (selectSize / (double)select->getSize()) << "n)" << endl << endl;

	for (int i = 0; i < 100; ++i) cout << "select(" << i << ") = " << select->select(i) << endl;

	delete select;
}
```
Using other types of select is analogous.

## API (32bit FMHWT index)
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
unsigned long long getIndexSize();
```
- get the size in bytes of the text used to build the index:
```
unsigned int getTextSize();
```
- get the result of **count** query:
```
unsigned int count(unsigned char *pattern, unsigned int patternLen);
```

## API (64bit FMHWT index)
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
unsigned long long getIndexSize();
```
- get the size in bytes of the text used to build the index:
```
unsigned long long getTextSize();
```
- get the result of **count** query:
```
unsigned long long count(unsigned char *pattern, unsigned int patternLen);
```

## FMHWT32\<class RANK32\>

Parameters:
- RANK32 class:
      - RankBasic32\<RankBasicType T\>
      - RankCF32
      - RankMPE32\<RankMPEType T\>

Constructors:
```
FMHWT32<class RANK32>();
```

## FMHWT32Hash\<class RANK32\>
FMHWT32Hash is FMHWT32 with hashed k-symbol prefixes of suffixes from suffix array to speed up searches (k ≥ 2). This variant is particularly efficient in speed for short patterns (not much longer than k).

Parameters:
- RANK32 class:
      - RankBasic32\<RankBasicType T\>
      - RankCF32
      - RankMPE32\<RankMPEType T\>
- k - length of prefixes of suffixes from suffix array
- loadFactor - hash table load factor

Limitations: 
- pattern length ≥ k (patterns shorter than k are handled by standard variant of FMHWT index)
- k ≥ 2
- 0.0 < loadFactor < 1.0

Constructors:
```
FMHWT32Hash<class RANK32>();
```

## FMHWT64\<class RANK64\>

Parameters:
- RANK64 class:
      - RankBasic64\<RankBasicType T\>
      - RankCF64
      - RankMPE64\<RankMPEType T\>

Constructors:
```
FMHWT64<class RANK64>();
```

## FMHWT64Hash\<class RANK64\>
FMHWT64Hash is FMHWT64 with hashed k-symbol prefixes of suffixes from suffix array to speed up searches (k ≥ 2). This variant is particularly efficient in speed for short patterns (not much longer than k).

Parameters:
- RANK64 class:
      - RankBasic64\<RankBasicType T\>
      - RankCF64
      - RankMPE64\<RankMPEType T\>
- k - length of prefixes of suffixes from suffix array
- loadFactor - hash table load factor

Limitations: 
- pattern length ≥ k (patterns shorter than k are handled by standard variant of FMHWT index)
- k ≥ 2
- 0.0 < loadFactor < 1.0

Constructors:
```
FMHWT64Hash<class RANK64>();
```

## FMHWT usage example
```
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "rank-select/shared/patterns.hpp"
#include "rank-select/shared/fm.hpp"

using namespace std;
using namespace shared;

int main(int argc, char *argv[]) {

	unsigned int queriesNum = 1000000;
	unsigned int patternLen = 20;
	FMHWT32<RankBasic<RANK_BASIC_COMPRESSED_HEADERS>> *fm = new FMHWT32<RankBasic<RANK_BASIC_COMPRESSED_HEADERS>>();
	const char *textFileName = "dna";
	const char *indexFileName = "dna-FMHWT-32.idx";

	if (fileExists(indexFileName)) {
		fm->load(indexFileName);
	} else {
		fm->build(textFileName);
		fm->save(indexFileName);
	}

	double indexSize = (double)fm->getIndexSize();
	cout << "Index size: " << indexSize << "B (" << (indexSize / (double)fm->getTextSize()) << "n)" << endl << endl;

	Patterns32 *P = new Patterns32(textFileName, queriesNum, patternLen);
	unsigned char **patterns = P->getPatterns();

	for (unsigned int i = 0; i < queriesNum; ++i) {
		cout << "Pattern |" << patterns[i] << "| occurs " << fm->count(patterns[i], patternLen) << " times." << endl;
	}

	delete fm;
	delete P;
}
```
Using other types of FMHWT index is analogous.

## External resources used in Rank&Select project
- Suffix array building by Yuta Mori (sais)
- A multi-platform library of highly optimized functions for C and C++ by Agner Fog (asmlib)
- A very fast hash function by Yann Collet (xxHash)

## References
1. Sz. Grabowski, M. Raniszewski. Rank and select: Another lesson learned. CoRR, abs/1605.01539, 2016.

## Authors
- Szymon Grabowski
- [Marcin Raniszewski](https://github.com/mranisz)
