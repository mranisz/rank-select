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

##API
There are several functions you can call on each of the rank or select object:
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
      - RankBasicType::STANDARD
      - RankBasicType::WITH_COMPRESSED_HEADERS

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
      - RankMPEType::V1
      - RankMPEType::V2
      - RankMPEType::V3

Constructors:
```
RankMPE<RankMPEType T>();
```

##SelectBasic\<SelectBasicType T, unsigned int L, unsigned int THRESHOLD\>

Parameters:
- T:
      - SelectBasicType::STANDARD
      - SelectBasicType::WITH_COMPRESSED_HEADERS
- L - ...
- THRESHOLD - ...

Limitations: 
- THRESHOLD > L
- L <= 1093 and THRESHOLD <= 34953 (for SelectBasicType::WITH_COMPRESSED_HEADERS)

Constructors:
```
SelectBasic<SelectBasicType T, unsigned int L, unsigned int THRESHOLD>();
```

##SelectMPE\<SelectMPEType T, unsigned int L, unsigned int THRESHOLD\>

Parameters:
- T:
      - SelectMPEType::V1
      - SelectMPEType::V2
      - SelectMPEType::V3
- L - ...
- THRESHOLD - ...

Limitations: 
- THRESHOLD > L
- L <= 547 and THRESHOLD <= 17473 (for SelectMPEType::V1)
- L <= 274 and THRESHOLD <= 8769 (for SelectMPEType::V2 and SelectMPEType::V3)

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

	RankBasic<RankBasicType::WITH_COMPRESSED_HEADERS> *rank = new RankBasic<RankBasicType::WITH_COMPRESSED_HEADERS>();
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

	SelectMPE<SelectMPEType::V2, 128, 4096> *select = new SelectMPE<SelectMPEType::V2, 128, 4096>();
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

##External resources used in Rank&Select project
- Suffix array building by Yuta Mori (sais)
- A multi-platform library of highly optimized functions for C and C++ by Agner Fog (asmlib)

##Authors
- Szymon Grabowski
- [Marcin Raniszewski](https://github.com/mranisz)
