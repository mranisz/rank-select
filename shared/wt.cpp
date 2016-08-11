#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <tuple>
#include "wt.h"

using namespace std;

namespace fmdummy {

unsigned int WT::getWTSize() {
	unsigned int size = sizeof(this->bitsLen) + sizeof(this->nodesLen) + sizeof(unsigned long long *);
	size += ((this->bitsLen + 16) * sizeof(unsigned long long) + this->nodesLen * sizeof(WT *));
	for (unsigned int i = 0; i < this->nodesLen; ++i) if (this->nodes[i] != NULL) size += this->nodes[i]->getWTSize();
	return size;
}

void WT::initialize() {
	this->bits = NULL;
	this->bitsLen = 0;
	this->alignedBits = NULL;
	this->nodes = NULL;
	this->nodesLen = 0;
}

void WT::freeMemory() {
	for (unsigned int i = 0; i < this->nodesLen; ++i) if (this->nodes[i] != NULL) this->nodes[i]->freeMemory();
	if (this->nodes != NULL) delete[] this->nodes;
	if (this->bits != NULL) delete[] this->bits;
}

void WT::free() {
	this->freeMemory();
	this->initialize();
}

void WT::save(FILE *outFile) {
	bool nullNode = false;
	bool notNullNode = true;
	fwrite(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
	if (this->bitsLen > 0) fwrite(this->alignedBits, (size_t)sizeof(unsigned long long), (size_t)this->bitsLen, outFile);
	fwrite(&this->nodesLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
	for (unsigned int i = 0; i < this->nodesLen; ++i) {
		if (this->nodes[i] == NULL) fwrite(&nullNode, (size_t)sizeof(bool), (size_t)1, outFile);
		else {
			fwrite(&notNullNode, (size_t)sizeof(bool), (size_t)1, outFile);
			this->nodes[i]->save(outFile);
		}
	}
}

void WT::load(FILE *inFile) {
	this->free();
	bool isNotNullNode;
	size_t result;
	result = fread(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
	if (result != 1) {
		cout << "Error loading index" << endl;
		exit(1);
	}
	if (this->bitsLen > 0) {
		this->bits = new unsigned long long[this->bitsLen + 16];
		this->alignedBits = this->bits;
		while ((unsigned long long)(this->alignedBits) % 128) ++(this->alignedBits);
		result = fread(this->alignedBits, (size_t)sizeof(unsigned long long), (size_t)this->bitsLen, inFile);
		if (result != this->bitsLen) {
			cout << "Error loading index" << endl;
			exit(1);
		}
	}
	result = fread(&this->nodesLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
	if (result != 1) {
		cout << "Error loading index" << endl;
		exit(1);
	}
	this->nodes = new WT *[this->nodesLen];
	for (unsigned int i = 0; i < this->nodesLen; ++i) {
		result = fread(&isNotNullNode, (size_t)sizeof(bool), (size_t)1, inFile);
		if (result != 1) {
			cout << "Error loading index" << endl;
			exit(1);
		}
		if (isNotNullNode) {
			this->nodes[i] = new WT();
			this->nodes[i]->load(inFile);
		} else this->nodes[i] = NULL;
	}
}

unsigned long long bitMask[64] = {
	0x0000000000000000ULL,
	0x8000000000000000ULL,
	0xC000000000000000ULL,
	0xE000000000000000ULL,
	0xF000000000000000ULL,
	0xF800000000000000ULL,
	0xFC00000000000000ULL,
	0xFE00000000000000ULL,
	0xFF00000000000000ULL,
	0xFF80000000000000ULL,
	0xFFC0000000000000ULL,
	0xFFE0000000000000ULL,
	0xFFF0000000000000ULL,
	0xFFF8000000000000ULL,
	0xFFFC000000000000ULL,
	0xFFFE000000000000ULL,
	0xFFFF000000000000ULL,
	0xFFFF800000000000ULL,
	0xFFFFC00000000000ULL,
	0xFFFFE00000000000ULL,
	0xFFFFF00000000000ULL,
	0xFFFFF80000000000ULL,
	0xFFFFFC0000000000ULL,
	0xFFFFFE0000000000ULL,
	0xFFFFFF0000000000ULL,
	0xFFFFFF8000000000ULL,
	0xFFFFFFC000000000ULL,
	0xFFFFFFE000000000ULL,
	0xFFFFFFF000000000ULL,
	0xFFFFFFF800000000ULL,
	0xFFFFFFFC00000000ULL,
	0xFFFFFFFE00000000ULL,
	0xFFFFFFFF00000000ULL,
	0xFFFFFFFF80000000ULL,
	0xFFFFFFFFC0000000ULL,
	0xFFFFFFFFE0000000ULL,
	0xFFFFFFFFF0000000ULL,
	0xFFFFFFFFF8000000ULL,
	0xFFFFFFFFFC000000ULL,
	0xFFFFFFFFFE000000ULL,
	0xFFFFFFFFFF000000ULL,
	0xFFFFFFFFFF800000ULL,
	0xFFFFFFFFFFC00000ULL,
	0xFFFFFFFFFFE00000ULL,
	0xFFFFFFFFFFF00000ULL,
	0xFFFFFFFFFFF80000ULL,
	0xFFFFFFFFFFFC0000ULL,
	0xFFFFFFFFFFFE0000ULL,
	0xFFFFFFFFFFFF0000ULL,
	0xFFFFFFFFFFFF8000ULL,
	0xFFFFFFFFFFFFC000ULL,
	0xFFFFFFFFFFFFE000ULL,
	0xFFFFFFFFFFFFF000ULL,
	0xFFFFFFFFFFFFF800ULL,
	0xFFFFFFFFFFFFFC00ULL,
	0xFFFFFFFFFFFFFE00ULL,
	0xFFFFFFFFFFFFFF00ULL,
	0xFFFFFFFFFFFFFF80ULL,
	0xFFFFFFFFFFFFFFC0ULL,
	0xFFFFFFFFFFFFFFE0ULL,
	0xFFFFFFFFFFFFFFF0ULL,
	0xFFFFFFFFFFFFFFF8ULL,
	0xFFFFFFFFFFFFFFFCULL,
	0xFFFFFFFFFFFFFFFEULL
};

unsigned long long interlacedMask2[64] = {
	0x0000000000000000ULL,
	0x4000000000000000ULL,
	0x5000000000000000ULL,
	0x5400000000000000ULL,
	0x5500000000000000ULL,
	0x5540000000000000ULL,
	0x5550000000000000ULL,
	0x5554000000000000ULL,
	0x5555000000000000ULL,
	0x5555400000000000ULL,
	0x5555500000000000ULL,
	0x5555540000000000ULL,
	0x5555550000000000ULL,
	0x5555554000000000ULL,
	0x5555555000000000ULL,
	0x5555555400000000ULL,
	0x5555555500000000ULL,
	0x5555555540000000ULL,
	0x5555555550000000ULL,
	0x5555555554000000ULL,
	0x5555555555000000ULL,
	0x5555555555400000ULL,
	0x5555555555500000ULL,
	0x5555555555540000ULL,
	0x5555555555550000ULL,
	0x5555555555554000ULL,
	0x5555555555555000ULL,
	0x5555555555555400ULL,
	0x5555555555555500ULL,
	0x5555555555555540ULL,
	0x5555555555555550ULL,
	0x5555555555555554ULL,
	0x5555555555555555ULL,
	0xD555555555555555ULL,
	0xF555555555555555ULL,
	0xFD55555555555555ULL,
	0xFF55555555555555ULL,
	0xFFD5555555555555ULL,
	0xFFF5555555555555ULL,
	0xFFFD555555555555ULL,
	0xFFFF555555555555ULL,
	0xFFFFD55555555555ULL,
	0xFFFFF55555555555ULL,
	0xFFFFFD5555555555ULL,
	0xFFFFFF5555555555ULL,
	0xFFFFFFD555555555ULL,
	0xFFFFFFF555555555ULL,
	0xFFFFFFFD55555555ULL,
	0xFFFFFFFF55555555ULL,
	0xFFFFFFFFD5555555ULL,
	0xFFFFFFFFF5555555ULL,
	0xFFFFFFFFFD555555ULL,
	0xFFFFFFFFFF555555ULL,
	0xFFFFFFFFFFD55555ULL,
	0xFFFFFFFFFFF55555ULL,
	0xFFFFFFFFFFFD5555ULL,
	0xFFFFFFFFFFFF5555ULL,
	0xFFFFFFFFFFFFD555ULL,
	0xFFFFFFFFFFFFF555ULL,
	0xFFFFFFFFFFFFFD55ULL,
	0xFFFFFFFFFFFFFF55ULL,
	0xFFFFFFFFFFFFFFD5ULL,
	0xFFFFFFFFFFFFFFF5ULL,
	0xFFFFFFFFFFFFFFFDULL,
};

unsigned long long interlacedMask3[63] = {
	0x0000000000000000ULL,
	0x1000000000000000ULL,
	0x1200000000000000ULL,
	0x1240000000000000ULL,
	0x1248000000000000ULL,
	0x1249000000000000ULL,
	0x1249200000000000ULL,
	0x1249240000000000ULL,
	0x1249248000000000ULL,
	0x1249249000000000ULL,
	0x1249249200000000ULL,
	0x1249249240000000ULL,
	0x1249249248000000ULL,
	0x1249249249000000ULL,
	0x1249249249200000ULL,
	0x1249249249240000ULL,
	0x1249249249248000ULL,
	0x1249249249249000ULL,
	0x1249249249249200ULL,
	0x1249249249249240ULL,
	0x1249249249249248ULL,
	0x1249249249249249ULL,
	0x3249249249249249ULL,
	0x3649249249249249ULL,
	0x36C9249249249249ULL,
	0x36D9249249249249ULL,
	0x36DB249249249249ULL,
	0x36DB649249249249ULL,
	0x36DB6C9249249249ULL,
	0x36DB6D9249249249ULL,
	0x36DB6DB249249249ULL,
	0x36DB6DB649249249ULL,
	0x36DB6DB6C9249249ULL,
	0x36DB6DB6D9249249ULL,
	0x36DB6DB6DB249249ULL,
	0x36DB6DB6DB649249ULL,
	0x36DB6DB6DB6C9249ULL,
	0x36DB6DB6DB6D9249ULL,
	0x36DB6DB6DB6DB249ULL,
	0x36DB6DB6DB6DB649ULL,
	0x36DB6DB6DB6DB6C9ULL,
	0x36DB6DB6DB6DB6D9ULL,
	0x36DB6DB6DB6DB6DBULL,
	0x76DB6DB6DB6DB6DBULL,
	0x7EDB6DB6DB6DB6DBULL,
	0x7FDB6DB6DB6DB6DBULL,
	0x7FFB6DB6DB6DB6DBULL,
	0x7FFF6DB6DB6DB6DBULL,
	0x7FFFEDB6DB6DB6DBULL,
	0x7FFFFDB6DB6DB6DBULL,
	0x7FFFFFB6DB6DB6DBULL,
	0x7FFFFFF6DB6DB6DBULL,
	0x7FFFFFFEDB6DB6DBULL,
	0x7FFFFFFFDB6DB6DBULL,
	0x7FFFFFFFFB6DB6DBULL,
	0x7FFFFFFFFF6DB6DBULL,
	0x7FFFFFFFFFEDB6DBULL,
	0x7FFFFFFFFFFDB6DBULL,
	0x7FFFFFFFFFFFB6DBULL,
	0x7FFFFFFFFFFFF6DBULL,
	0x7FFFFFFFFFFFFEDBULL,
	0x7FFFFFFFFFFFFFDBULL,
	0x7FFFFFFFFFFFFFFBULL,
};

WT *createWT2_512_counter40(unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen) {
	if (textLen == 0) return NULL;

	unsigned int textLengths[2];
	unsigned char *texts[2];
	for (int i = 0; i < 2; ++i) {
		textLengths[i] = 0;
		texts[i] = new unsigned char[textLen];
	}

	bool childExists = false;
	for (unsigned int i = 0; i < textLen; ++i) {
		if (codeLen[text[i]] > wtLevel) {
			childExists = true;
			int nextNode = (code[text[i]] >> wtLevel) & 1;
			texts[nextNode][textLengths[nextNode]++] = text[i];
		}
	}

	if (!childExists) return NULL;

	WT *node = new WT(2);

	for (int i = 0; i < 2; ++i) {
		node->nodes[i] = createWT2_512_counter40(texts[i], textLengths[i], wtLevel + 1, code, codeLen);
		delete[] texts[i];
	}

	unsigned long long nodeBitLenTemp = textLen / 64;
	if (textLen % 64 > 0) ++nodeBitLenTemp;
	node->bitsLen = nodeBitLenTemp / 7;
	if (nodeBitLenTemp % 7 > 0) ++(node->bitsLen);
	node->bitsLen *= 8;
	if (textLen % 448 == 0) ++(node->bitsLen);
	node->bits = new unsigned long long[node->bitsLen + 16];
	node->alignedBits = node->bits;
	while ((unsigned long long)(node->alignedBits) % 128) ++(node->alignedBits);
	unsigned long long ranks[4];
	for (int i = 0; i < 4; ++i) ranks[i] = 0;
	unsigned long long rank = 0;
	int currRank = 0, j = 0;
	unsigned long long packedBits = 0;
	long long packedBitsCounter = 0, lastRankCounter = 0;
	for (unsigned int i = 0; i < textLen; ++i, ++j) {
		if (j % 128 == 0) ranks[currRank++] = rank;
		if (i % 448 == 0) {
			if (j > 0) node->alignedBits[lastRankCounter] += ((ranks[1] - ranks[0]) << 56) + ((ranks[2] - ranks[1]) << 48) + ((ranks[3] - ranks[2]) << 40);
			lastRankCounter = packedBitsCounter;
			node->alignedBits[packedBitsCounter++] = rank;
			ranks[0] = rank;
			currRank = 1;
			j = 0;
		}
		int nextNode = (code[text[i]] >> wtLevel) & 1;
		if (nextNode == 1) ++rank;
		packedBits = (packedBits << 1) + nextNode;
		if (i % 64 == 63) {
			node->alignedBits[packedBitsCounter++] = packedBits;
			packedBits = 0;
		}
	}
	if (textLen % 64 != 0) {
		for (int i = textLen % 64; i % 64 != 0; ++i) {
			packedBits = (packedBits << 1);
		}
		node->alignedBits[packedBitsCounter++] = packedBits;
	}
	if (j > 0) {
		ranks[currRank++] = rank;
		for (int i = currRank; i < 4; ++i) ranks[i] = ranks[i - 1];
		node->alignedBits[lastRankCounter] += ((ranks[1] - ranks[0]) << 56) + ((ranks[2] - ranks[1]) << 48) + ((ranks[3] - ranks[2]) << 40);
	}
	if (textLen % 448 == 0) {
		node->alignedBits[packedBitsCounter++] = rank;
	}
	return node;
}

WT *createWT2_1024_counter32(unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen) {
	if (textLen == 0) return NULL;

	unsigned int textLengths[2];
	unsigned char *texts[2];
	for (int i = 0; i < 2; ++i) {
		textLengths[i] = 0;
		texts[i] = new unsigned char[textLen];
	}

	bool childExists = false;
	for (unsigned int i = 0; i < textLen; ++i) {
		if (codeLen[text[i]] > wtLevel) {
			childExists = true;
			int nextNode = (code[text[i]] >> wtLevel) & 1;
			texts[nextNode][textLengths[nextNode]++] = text[i];
		}
	}

	if (!childExists) return NULL;

	WT *node = new WT(2);

	for (int i = 0; i < 2; ++i) {
		node->nodes[i] = createWT2_1024_counter32(texts[i], textLengths[i], wtLevel + 1, code, codeLen);
		delete[] texts[i];
	}

	unsigned long long nodeBitLenTemp = textLen / 64;
	if (textLen % 64 > 0) ++nodeBitLenTemp;
	node->bitsLen = nodeBitLenTemp / 15;
	if (nodeBitLenTemp % 15 > 0) ++(node->bitsLen);
	node->bitsLen *= 16;
	if (textLen % 960 == 0) ++(node->bitsLen);
	node->bits = new unsigned long long[node->bitsLen + 16];
	node->alignedBits = node->bits;
	while ((unsigned long long)(node->alignedBits) % 128) ++(node->alignedBits);
	unsigned long long ranks[6];
	for (int i = 0; i < 6; ++i) ranks[i] = 0;
	unsigned long long rank = 0;
	int currRank = 0, j = 0;
	unsigned long long packedBits = 0;
	long long packedBitsCounter = 0, lastRankCounter = 0;
	for (unsigned int i = 0; i < textLen; ++i, ++j) {
		if (j % 192 == 0) ranks[currRank++] = rank;
		if (i % 960 == 0) {
			if (j > 0) node->alignedBits[lastRankCounter] += ((ranks[1] - ranks[0]) << 56) + ((ranks[2] - ranks[1]) << 48) + ((ranks[3] - ranks[2]) << 40) + ((ranks[4] - ranks[3]) << 32);
			lastRankCounter = packedBitsCounter;
			node->alignedBits[packedBitsCounter++] = rank;
			ranks[0] = rank;
			currRank = 1;
			j = 0;
		}
		int nextNode = (code[text[i]] >> wtLevel) & 1;
		if (nextNode == 1) ++rank;
		packedBits = (packedBits << 1) + nextNode;
		if (i % 64 == 63) {
			node->alignedBits[packedBitsCounter++] = packedBits;
			packedBits = 0;
		}
	}
	if (textLen % 64 != 0) {
		for (int i = textLen % 64; i % 64 != 0; ++i) {
			packedBits = (packedBits << 1);
		}
		node->alignedBits[packedBitsCounter++] = packedBits;
	}
	if (j > 0) {
		ranks[currRank++] = rank;
		for (int i = currRank; i < 6; ++i) ranks[i] = ranks[i - 1];
		node->alignedBits[lastRankCounter] += ((ranks[1] - ranks[0]) << 56) + ((ranks[2] - ranks[1]) << 48) + ((ranks[3] - ranks[2]) << 40) + ((ranks[4] - ranks[3]) << 32);
	}
	if (textLen % 960 == 0) {
		node->alignedBits[packedBitsCounter++] = rank;
	}
	return node;
}

WT *createWT4(int type, unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen){
        if (type != WT::TYPE_512 && type != WT::TYPE_1024) {
		cout << "Error: not valid WT type" << endl;
		exit(1);
	}
	if (textLen == 0) return NULL;

	unsigned int textLengths[4];
	unsigned int rank[4];
	unsigned char *texts[4];
	for (int i = 0; i < 4; ++i) {
		textLengths[i] = 0;
		rank[i] = 0;
		texts[i] = new unsigned char[textLen];
	}

	bool childExists = false;
	for (unsigned int i = 0; i < textLen; ++i) {
		if (codeLen[text[i]] > wtLevel) {
			childExists = true;
			int nextNode = (code[text[i]] >> (2 * wtLevel)) & 3;
			texts[nextNode][textLengths[nextNode]++] = text[i];
		}
	}

	if (!childExists) return NULL;

	WT *node = new WT(4);

	for (int i = 0; i < 4; ++i) {
		node->nodes[i] = createWT4(type, texts[i], textLengths[i], wtLevel + 1, code, codeLen);
		delete[] texts[i];
	}

	unsigned long long nodeDibitLenTemp = (2 * textLen) / 64;
	if ((2 * textLen) % 64 > 0) ++nodeDibitLenTemp;
	node->bitsLen = nodeDibitLenTemp / (type - 2);
	if (nodeDibitLenTemp % (type - 2) > 0) ++(node->bitsLen);
	node->bitsLen *= type;
	if (textLen % ((type - 2) * 32) == 0) node->bitsLen += 2;
	node->bits = new unsigned long long[node->bitsLen + 16];
	node->alignedBits = node->bits;
	while ((unsigned long long)(node->alignedBits) % 128) ++(node->alignedBits);
	unsigned long long packedDibits = 0;
	long long packedDibitsCounter = 0;
	for (unsigned int i = 0; i < textLen; ++i) {
		if (i % ((type - 2) * 32) == 0) {
			for (int j = 0; j < 2; ++j) {
				node->alignedBits[packedDibitsCounter++] = ((unsigned long long)rank[2 * j] << 32) + rank[2 * j + 1];
			}
		}
		int nextNode = (code[text[i]] >> (2 * wtLevel)) & 3;
		++rank[nextNode];
		packedDibits = (packedDibits << 2) + nextNode;
		if (i % 32 == 31) {
			node->alignedBits[packedDibitsCounter++] = packedDibits;
			packedDibits = 0;
		}
	}
	if (textLen % 32 != 0) {
		for (int i = textLen % 32; i % 32 != 0; ++i) {
			packedDibits = (packedDibits << 2);
		}
		node->alignedBits[packedDibitsCounter++] = packedDibits;
	}
	if (textLen % ((type - 2) * 32) == 0) {
		for (int j = 0; j < 2; ++j) {
			node->alignedBits[packedDibitsCounter++] = ((unsigned long long)rank[2 * j] << 32) + rank[2 * j + 1];
		}
	}
	return node;
}
WT *createWT8(int type, unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen) {
        if (type != WT::TYPE_512 && type != WT::TYPE_1024) {
		cout << "Error: not valid WT type" << endl;
		exit(1);
	}
	if (textLen == 0) return NULL;

	unsigned int textLengths[8];
	unsigned int rank[8];
	unsigned char *texts[8];
	for (int i = 0; i < 8; ++i) {
		textLengths[i] = 0;
		rank[i] = 0;
		texts[i] = new unsigned char[textLen];
	}

	bool childExists = false;
	for (unsigned int i = 0; i < textLen; ++i) {
		if (codeLen[text[i]] > wtLevel) {
			childExists = true;
			int nextNode = (code[text[i]] >> (3 * wtLevel)) & 7;
			texts[nextNode][textLengths[nextNode]++] = text[i];
		}
	}

	if (!childExists) return NULL;

	WT *node = new WT(8);

	for (int i = 0; i < 8; ++i) {
		node->nodes[i] = createWT8(type, texts[i], textLengths[i], wtLevel + 1, code, codeLen);
		delete[] texts[i];
	}

	unsigned long long nodeTripleLenTemp = (3 * textLen) / 63;
	if ((3 * textLen) % 63 > 0) ++nodeTripleLenTemp;
	node->bitsLen = nodeTripleLenTemp / (type - 4);
	if (nodeTripleLenTemp % (type - 4) > 0) ++(node->bitsLen);
	node->bitsLen *= type;
	if (textLen % ((type - 4) * 21) == 0) node->bitsLen += 4;
	node->bits = new unsigned long long[node->bitsLen + 16];
	node->alignedBits = node->bits;
	while ((unsigned long long)(node->alignedBits) % 128) ++(node->alignedBits);
	unsigned long long packedTriples = 0;
	long long packedTripleCounter = 0;
	for (unsigned int i = 0; i < textLen; ++i) {
		if (i % ((type - 4) * 21) == 0) {
			for (int j = 0; j < 4; ++j) {
				node->alignedBits[packedTripleCounter++] = ((unsigned long long)rank[2 * j] << 32) + rank[2 * j + 1];
			}
		}
		int nextNode = (code[text[i]] >> (3 * wtLevel)) & 7;
		++rank[nextNode];
		packedTriples = (packedTriples << 3) + nextNode;
		if (i % 21 == 20) {
			node->alignedBits[packedTripleCounter++] = packedTriples;
			packedTriples = 0;
		}
	}
	if (textLen % 21 != 0) {
		for (int i = textLen % 21; i % 21 != 0; ++i) {
			packedTriples = (packedTriples << 3);
		}
		node->alignedBits[packedTripleCounter++] = packedTriples;
	}
	if (textLen % ((type - 4) * 21) == 0) {
		for (int j = 0; j < 4; ++j) {
			node->alignedBits[packedTripleCounter++] = ((unsigned long long)rank[2 * j] << 32) + rank[2 * j + 1];
		}
	}
	return node;
}

pair<unsigned int, unsigned int> getRankWT2_512_counters40(unsigned long long code, unsigned int codeLen, unsigned int iFirst, unsigned int iLast, WT *wt, unsigned int wtLevel) {
    unsigned int rank;

    for (wtLevel = 0; wtLevel < codeLen; ++wtLevel) {
            int nextNode = (code >> wtLevel) & 1;

            // *** first
            unsigned int j = iFirst / 448;
            unsigned long long *p = wt->alignedBits + 8 * j;
            //rank = (*p) & 0x00000000FFFFFFFFULL;
            rank = (unsigned int)(*p);

            //unsigned int b1 = ((*p) >> 56) & 0x00000000000000FFULL;  // popcount for 128-bit prefix
            unsigned int b1 = ((*p) >> 56);  // popcount for 128-bit prefix
            unsigned int b2 = b1 + (((*p) >> 48) & 0x00000000000000FFULL);  // popcount for 256-bit prefix
            unsigned int b3 = b2 + (((*p) >> 40) & 0x00000000000000FFULL);  // popcount for 384-bit prefix
            unsigned int temp1, temp2;

            unsigned int k = iFirst - (j * 448);
            switch (k / 64) {
            case 6:
                    rank += b3 + __builtin_popcountll(*(p + 7) & bitMask[k % 64]);
                    break;
            case 5:
                    temp1 = __builtin_popcountll(*(p + 5));
                    temp2 = __builtin_popcountll(*(p + 6) & bitMask[k % 64]);
                    rank += b2 + temp1 + temp2;
                    break;
            case 4:
                    rank += b2 + __builtin_popcountll(*(p + 5) & bitMask[k % 64]);
                    break;
            case 3:
                    temp1 = __builtin_popcountll(*(p + 3));
                    temp2 = __builtin_popcountll(*(p + 4) & bitMask[k % 64]);
                    rank += b1 + temp1 + temp2;
                    break;
            case 2:
                    rank += b1 + __builtin_popcountll(*(p + 3) & bitMask[k % 64]);
                    break;
            case 1:
                    temp1 = __builtin_popcountll(*(p + 1));
                    temp2 = __builtin_popcountll(*(p + 2) & bitMask[k % 64]);
                    rank += temp1 + temp2;
                    break;
            case 0:
                    rank += __builtin_popcountll(*(p+1) & bitMask[k % 64]);
            }
            if (nextNode == 0) rank = iFirst - rank;

            if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 8 * (rank / 448), 0, 3);
            iFirst = rank;

            // *** last
            j = iLast / 448;
            p = wt->alignedBits + 8 * j;
            //rank = (*p) & 0x00000000FFFFFFFFULL;
            rank = (unsigned int)(*p);

            //b1 = ((*p) >> 56) & 0x00000000000000FFULL;  // popcount for 128-bit prefix
            b1 = ((*p) >> 56);  // popcount for 128-bit prefix
            b2 = b1 + (((*p) >> 48) & 0x00000000000000FFULL);  // popcount for 256-bit prefix
            b3 = b2 + (((*p) >> 40) & 0x00000000000000FFULL);  // popcount for 384-bit prefix

            k = iLast - (j * 448);
            switch (k / 64) {
            case 6:
                    rank += b3 + __builtin_popcountll(*(p + 7) & bitMask[k % 64]);
                    break;
            case 5:
                    temp1 = __builtin_popcountll(*(p + 5));
                    temp2 = __builtin_popcountll(*(p + 6) & bitMask[k % 64]);
                    rank += b2 + temp1 + temp2;
                    break;
            case 4:
                    rank += b2 + __builtin_popcountll(*(p + 5) & bitMask[k % 64]);
                    break;
            case 3:
                    temp1 = __builtin_popcountll(*(p + 3));
                    temp2 = __builtin_popcountll(*(p + 4) & bitMask[k % 64]);
                    rank += b1 + temp1 + temp2;
                    break;
            case 2:
                    rank += b1 + __builtin_popcountll(*(p + 3) & bitMask[k % 64]);
                    break;
            case 1:
                    temp1 = __builtin_popcountll(*(p + 1));
                    temp2 = __builtin_popcountll(*(p + 2) & bitMask[k % 64]);
                    rank += temp1 + temp2;
                    break;
            case 0:
                    rank += __builtin_popcountll(*(p + 1) & bitMask[k % 64]);
            }
            if (nextNode == 0) rank = iLast - rank;

            if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 8 * (rank / 448), 0, 3);
            iLast = rank;

            wt = wt->nodes[nextNode];
    }

    return make_pair(iFirst, iLast);
}


unsigned int count_WT2_512_counter40(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen) {
        unsigned char c;
	__builtin_prefetch(wt->alignedBits + 8 * ((firstVal - 1) / 448), 0, 3);
	__builtin_prefetch(wt->alignedBits + 8 * (lastVal / 448), 0, 3);

        while (firstVal <= lastVal && i > 1) {
                c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;

                tie(firstVal, lastVal) = getRankWT2_512_counters40(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
                firstVal += C[c] + 1;
                __builtin_prefetch(wt->alignedBits + 8 * ((firstVal - 1) / 448), 0, 3);
                lastVal  += C[c];
                __builtin_prefetch(wt->alignedBits + 8 * (lastVal / 448), 0, 3);
                --i;
        }

        if (firstVal <= lastVal) {
                c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;
                tie(firstVal, lastVal) = getRankWT2_512_counters40(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
                firstVal += C[c] + 1;
                lastVal  += C[c];
        }

        if (firstVal > lastVal) return 0;
        else return lastVal - firstVal + 1;
}

pair<unsigned int, unsigned int>  getRankWT2_1024_counters32(unsigned long long code, unsigned int codeLen, unsigned int iFirst, unsigned int iLast, WT *wt, unsigned int wtLevel) {
        unsigned int rank;

        for (wtLevel = 0; wtLevel < codeLen; ++wtLevel) {
                int nextNode = (code >> wtLevel) & 1;

                // *** first
                unsigned int j = iFirst / 960;
                unsigned long long *p = wt->alignedBits + 16 * j;
                //unsigned int rank = (*p) & 0x00000000FFFFFFFFULL;
                rank = (unsigned int)(*p);
                //unsigned int b1 = ((*p) >> 56) & 0x00000000000000FFULL;  // popcount for 192-bit prefix
                unsigned int b1 = ((*p) >> 56);
                unsigned int b2 = b1 + (((*p) >> 48) & 0x00000000000000FFULL);  // popcount for 384-bit prefix
                unsigned int b3 = b2 + (((*p) >> 40) & 0x00000000000000FFULL);  // popcount for 576-bit prefix
                unsigned int b4 = b3 + (((*p) >> 32) & 0x00000000000000FFULL);  // popcount for 768-bit prefix
                unsigned int temp1, temp2, temp3;

                unsigned int k = iFirst - (j * 960);
                switch (k / 64) {
                case 14:
                        temp1 = __builtin_popcountll(*(p + 13));
                        temp2 = __builtin_popcountll(*(p + 14));
                        temp3 = __builtin_popcountll(*(p + 15) & bitMask[k % 64]);
                        rank += b4 + temp1 + temp2 + temp3;
                        break;
                case 13:
                        temp1 = __builtin_popcountll(*(p + 13));
                        temp2 = __builtin_popcountll(*(p + 14) & bitMask[k % 64]);
                        rank += b4 + temp1 + temp2;
                        break;
                case 12:
                        rank += b4 + __builtin_popcountll(*(p + 13) & bitMask[k % 64]);
                        break;
                case 11:
                        temp1 = __builtin_popcountll(*(p + 10));
                        temp2 = __builtin_popcountll(*(p + 11));
                        temp3 = __builtin_popcountll(*(p + 12) & bitMask[k % 64]);
                        rank += b3 + temp1 + temp2 + temp3;
                        break;
                case 10:
                        temp1 = __builtin_popcountll(*(p + 10));
                        temp2 = __builtin_popcountll(*(p + 11) & bitMask[k % 64]);
                        rank += b3 + temp1 + temp2;
                        break;
                case 9:
                        rank += b3 + __builtin_popcountll(*(p + 10) & bitMask[k % 64]);
                        break;
                case 8:
                        temp1 = __builtin_popcountll(*(p + 7));
                        temp2 = __builtin_popcountll(*(p + 8));
                        temp3 = __builtin_popcountll(*(p + 9) & bitMask[k % 64]);
                        rank += b2 + temp1 + temp2 + temp3;
                        break;
                case 7:
                        temp1 = __builtin_popcountll(*(p + 7));
                        temp2 = __builtin_popcountll(*(p + 8) & bitMask[k % 64]);
                        rank += b2 + temp1 + temp2;
                        break;
                case 6:
                        rank += b2 + __builtin_popcountll(*(p + 7) & bitMask[k % 64]);
                        break;
                case 5:
                        temp1 = __builtin_popcountll(*(p + 4));
                        temp2 = __builtin_popcountll(*(p + 5));
                        temp3 = __builtin_popcountll(*(p + 6) & bitMask[k % 64]);
                        rank += b1 + temp1 + temp2 + temp3;
                        break;
                case 4:
                        temp1 = __builtin_popcountll(*(p + 4));
                        temp2 = __builtin_popcountll(*(p + 5) & bitMask[k % 64]);
                        rank += b1 + temp1 + temp2;
                        break;
                case 3:
                        rank += b1 + __builtin_popcountll(*(p + 4) & bitMask[k % 64]);
                        break;
                case 2:
                        temp1 = __builtin_popcountll(*(p + 1));
                        temp2 = __builtin_popcountll(*(p + 2));
                        temp3 = __builtin_popcountll(*(p + 3) & bitMask[k % 64]);
                        rank += temp1 + temp2 + temp3;
                        break;
                case 1:
                        temp1 = __builtin_popcountll(*(p + 1));
                        temp2 = __builtin_popcountll(*(p + 2) & bitMask[k % 64]);
                        rank += temp1 + temp2;
                        break;
                case 0:
                        rank += __builtin_popcountll(*(p + 1) & bitMask[k % 64]);
                }

                if (nextNode == 0) rank = iFirst - rank;

                if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 16 * (rank / 960), 0, 3);
                iFirst = rank;
                
                // *** last
                j = iLast / 960;
                p = wt->alignedBits + 16 * j;
                //unsigned int rank = (*p) & 0x00000000FFFFFFFFULL;
                rank = (unsigned int)(*p);
                //b1 = ((*p) >> 56) & 0x00000000000000FFULL;  // popcount for 192-bit prefix
                b1 = ((*p) >> 56);
                b2 = b1 + (((*p) >> 48) & 0x00000000000000FFULL);  // popcount for 384-bit prefix
                b3 = b2 + (((*p) >> 40) & 0x00000000000000FFULL);  // popcount for 576-bit prefix
                b4 = b3 + (((*p) >> 32) & 0x00000000000000FFULL);  // popcount for 768-bit prefix

                k = iLast - (j * 960);
                switch (k / 64) {
                case 14:
                        temp1 = __builtin_popcountll(*(p + 13));
                        temp2 = __builtin_popcountll(*(p + 14));
                        temp3 = __builtin_popcountll(*(p + 15) & bitMask[k % 64]);
                        rank += b4 + temp1 + temp2 + temp3;
                        break;
                case 13:
                        temp1 = __builtin_popcountll(*(p + 13));
                        temp2 = __builtin_popcountll(*(p + 14) & bitMask[k % 64]);
                        rank += b4 + temp1 + temp2;
                        break;
                case 12:
                        rank += b4 + __builtin_popcountll(*(p + 13) & bitMask[k % 64]);
                        break;
                case 11:
                        temp1 = __builtin_popcountll(*(p + 10));
                        temp2 = __builtin_popcountll(*(p + 11));
                        temp3 = __builtin_popcountll(*(p + 12) & bitMask[k % 64]);
                        rank += b3 + temp1 + temp2 + temp3;
                        break;
                case 10:
                        temp1 = __builtin_popcountll(*(p + 10));
                        temp2 = __builtin_popcountll(*(p + 11) & bitMask[k % 64]);
                        rank += b3 + temp1 + temp2;
                        break;
                case 9:
                        rank += b3 + __builtin_popcountll(*(p + 10) & bitMask[k % 64]);
                        break;
                case 8:
                        temp1 = __builtin_popcountll(*(p + 7));
                        temp2 = __builtin_popcountll(*(p + 8));
                        temp3 = __builtin_popcountll(*(p + 9) & bitMask[k % 64]);
                        rank += b2 + temp1 + temp2 + temp3;
                        break;
                case 7:
                        temp1 = __builtin_popcountll(*(p + 7));
                        temp2 = __builtin_popcountll(*(p + 8) & bitMask[k % 64]);
                        rank += b2 + temp1 + temp2;
                        break;
                case 6:
                        rank += b2 + __builtin_popcountll(*(p + 7) & bitMask[k % 64]);
                        break;
                case 5:
                        temp1 = __builtin_popcountll(*(p + 4));
                        temp2 = __builtin_popcountll(*(p + 5));
                        temp3 = __builtin_popcountll(*(p + 6) & bitMask[k % 64]);
                        rank += b1 + temp1 + temp2 + temp3;
                        break;
                case 4:
                        temp1 = __builtin_popcountll(*(p + 4));
                        temp2 = __builtin_popcountll(*(p + 5) & bitMask[k % 64]);
                        rank += b1 + temp1 + temp2;
                        break;
                case 3:
                        rank += b1 + __builtin_popcountll(*(p + 4) & bitMask[k % 64]);
                        break;
                case 2:
                        temp1 = __builtin_popcountll(*(p + 1));
                        temp2 = __builtin_popcountll(*(p + 2));
                        temp3 = __builtin_popcountll(*(p + 3) & bitMask[k % 64]);
                        rank += temp1 + temp2 + temp3;
                        break;
                case 1:
                        temp1 = __builtin_popcountll(*(p + 1));
                        temp2 = __builtin_popcountll(*(p + 2) & bitMask[k % 64]);
                        rank += temp1 + temp2;
                        break;
                case 0:
                        rank += __builtin_popcountll(*(p + 1) & bitMask[k % 64]);
                }

                if (nextNode == 0) rank = iLast - rank;

                if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 16 * (rank / 960), 0, 3);
                iLast = rank;

                wt = wt->nodes[nextNode];
        }

    return make_pair(iFirst, iLast);
}

unsigned int count_WT2_1024_counter32(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen) {
	unsigned char c;
	__builtin_prefetch(wt->alignedBits + 16 * ((firstVal - 1) / 960), 0, 3);
	__builtin_prefetch(wt->alignedBits + 16 * (lastVal / 960), 0, 3);

	while (firstVal <= lastVal && i > 1) {
		c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;
		tie(firstVal, lastVal) = getRankWT2_1024_counters32(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
		firstVal += C[c] + 1;
		__builtin_prefetch(wt->alignedBits + 16 * ((firstVal - 1) / 960), 0, 3);
		lastVal  += C[c];
		__builtin_prefetch(wt->alignedBits + 16 * (lastVal / 960), 0, 3);
		--i;
	}

	if (firstVal <= lastVal) {
		c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;
		tie(firstVal, lastVal) = getRankWT2_1024_counters32(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
		firstVal += C[c] + 1;
                lastVal  += C[c];
	}

	if (firstVal > lastVal) return 0;
	else return lastVal - firstVal + 1;

}

unsigned long long getWT4BitVector0R(unsigned long long b) {
	b = ~b;
	return (b & (b >> 1)) & 0x5555555555555555ULL;
}

unsigned long long getWT4BitVector0L(unsigned long long b) {
	b = ~b;
	return (b & (b << 1)) & 0xAAAAAAAAAAAAAAAAULL;
}

unsigned long long getWT4BitVector1R(unsigned long long b) {
	b ^= 0xAAAAAAAAAAAAAAAAULL;
	return (b & (b >> 1)) & 0x5555555555555555ULL;
}

unsigned long long getWT4BitVector1L(unsigned long long b) {
	b ^= 0xAAAAAAAAAAAAAAAAULL;
	return (b & (b << 1)) & 0xAAAAAAAAAAAAAAAAULL;
}

unsigned long long getWT4BitVector2R(unsigned long long b) {
	b ^= 0x5555555555555555ULL;
	return (b & (b >> 1)) & 0x5555555555555555ULL;
}

unsigned long long getWT4BitVector2L(unsigned long long b) {
	b ^= 0x5555555555555555ULL;
	return (b & (b << 1)) & 0xAAAAAAAAAAAAAAAAULL;
}

unsigned long long getWT4BitVector3R(unsigned long long b) {
	return (b & (b >> 1)) & 0x5555555555555555ULL;
}

unsigned long long getWT4BitVector3L(unsigned long long b) {
	return (b & (b << 1)) & 0xAAAAAAAAAAAAAAAAULL;
}

pair<unsigned int, unsigned int> getRankWT4_512(unsigned long long code, unsigned int codeLen, unsigned int iFirst, unsigned int iLast, WT *wt, unsigned int wtLevel) {
    unsigned int rank;

    for (wtLevel = 0; wtLevel < codeLen; ++wtLevel) {
            int nextNode = (code >> (2 * wtLevel)) & 3;

            // *** first
            unsigned int j = iFirst / 192;
            unsigned long long *p = wt->alignedBits + 8 * j + nextNode / 2;
            if ((nextNode & 1) == 0) rank = (*p) >> 32;
            //else rank = (*p) & 0x00000000FFFFFFFFULL;
            else rank = (unsigned int)(*p);
            p += (2 - nextNode / 2);

            iFirst -= (j * 192);
            unsigned int temp1 = 0;
            unsigned int temp2 = 0;
            unsigned int temp3 = 0;
            switch (nextNode) {
            case 0:
                    switch (iFirst / 64) {
                    case 2:
                            temp3 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                            p += 2;
                    case 1:
                            temp2 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                            p += 2;
                    case 0:
                            temp1 = __builtin_popcountll((getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1))) & interlacedMask2[iFirst % 64]);
                    }
                    break;
            case 1:
                    switch (iFirst / 64) {
                    case 2:
                            temp3 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                            p += 2;
                    case 1:
                            temp2 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                            p += 2;
                    case 0:
                            temp1 = __builtin_popcountll((getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1))) & interlacedMask2[iFirst % 64]);
                    }
                    break;
            case 2:
                    switch (iFirst / 64) {
                    case 2:
                            temp3 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                            p += 2;
                    case 1:
                            temp2 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                            p += 2;
                    case 0:
                            temp1 = __builtin_popcountll((getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1))) & interlacedMask2[iFirst % 64]);
                    }
                    break;
            default:
                    switch (iFirst / 64) {
                    case 2:
                            temp3 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                            p += 2;
                    case 1:
                            temp2 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                            p += 2;
                    case 0:
                            temp1 = __builtin_popcountll((getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1))) & interlacedMask2[iFirst % 64]);
                    }
            }
            rank += temp1 + temp2 + temp3;

            if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 8 * (rank / 192), 0, 3);
            iFirst = rank;

            // *** last
            j = iLast / 192;
            p = wt->alignedBits + 8 * j + nextNode / 2;
            if ((nextNode & 1) == 0) rank = (*p) >> 32;
            //else rank = (*p) & 0x00000000FFFFFFFFULL;
            else rank = (unsigned int)(*p);
            p += (2 - nextNode / 2);

            iLast -= (j * 192);
            temp1 = 0;
            temp2 = 0;
            temp3 = 0;
            switch (nextNode) {
            case 0:
                    switch (iLast / 64) {
                    case 2:
                            temp3 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                            p += 2;
                    case 1:
                            temp2 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                            p += 2;
                    case 0:
                            temp1 = __builtin_popcountll((getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1))) & interlacedMask2[iLast % 64]);
                    }
                    break;
            case 1:
                    switch (iLast / 64) {
                    case 2:
                            temp3 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                            p += 2;
                    case 1:
                            temp2 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                            p += 2;
                    case 0:
                            temp1 = __builtin_popcountll((getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1))) & interlacedMask2[iLast % 64]);
                    }
                    break;
            case 2:
                    switch (iLast / 64) {
                    case 2:
                            temp3 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                            p += 2;
                    case 1:
                            temp2 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                            p += 2;
                    case 0:
                            temp1 = __builtin_popcountll((getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1))) & interlacedMask2[iLast % 64]);
                    }
                    break;
            default:
                    switch (iLast / 64) {
                    case 2:
                            temp3 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                            p += 2;
                    case 1:
                            temp2 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                            p += 2;
                    case 0:
                            temp1 = __builtin_popcountll((getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1))) & interlacedMask2[iLast % 64]);
                    }
            }
            rank += temp1 + temp2 + temp3;

            if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 8 * (rank / 192), 0, 3);
            iLast = rank;

            wt = wt->nodes[nextNode];
    }
   
    return make_pair(iFirst, iLast);
}

unsigned int count_WT4_512(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen) {
    unsigned char c;
	__builtin_prefetch(wt->alignedBits + 8 * ((firstVal - 1) / 192), 0, 3);
	__builtin_prefetch(wt->alignedBits + 8 * (lastVal / 192), 0, 3);

    while (firstVal <= lastVal && i > 1) {
        c = pattern[i - 1];
        if (codeLen[c] == 0) return 0;
        tie(firstVal, lastVal) = getRankWT4_512(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
        firstVal += C[c] + 1;
        __builtin_prefetch(wt->alignedBits + 8 * ((firstVal - 1) / 192), 0, 3);
        lastVal += C[c];
        __builtin_prefetch(wt->alignedBits + 8 * (lastVal / 192), 0, 3);
        --i;
    }

    if (firstVal <= lastVal) {
        c = pattern[i - 1];
        if (codeLen[c] == 0) return 0;
        tie(firstVal, lastVal) = getRankWT4_512(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
        firstVal += C[c] + 1;
        lastVal += C[c];
    }

    if (firstVal > lastVal) return 0;
    else return lastVal - firstVal + 1;
}

pair<unsigned int, unsigned int> getRankWT4_1024(unsigned long long code, unsigned int codeLen, unsigned int iFirst, unsigned int iLast, WT *wt, unsigned int wtLevel) {
        unsigned int rank;
        
        for (wtLevel = 0; wtLevel < codeLen; ++wtLevel) {
                int nextNode = (code >> (2 * wtLevel)) & 3;
                
                // *** first
                unsigned int j = iFirst / 448;
                unsigned long long *p = wt->alignedBits + 16 * j + nextNode / 2;
                if ((nextNode & 1) == 0) rank = (*p) >> 32;
                //else rank = (*p) & 0x00000000FFFFFFFFULL;
                else rank = (unsigned int)(*p);
                p += (2 - nextNode / 2);

                iFirst -= (j * 448);
                unsigned int temp1 = 0;
                unsigned int temp2 = 0;
                unsigned int temp3 = 0;
                unsigned int temp4 = 0;
                unsigned int temp5 = 0;
                unsigned int temp6 = 0;
                unsigned int temp7 = 0;
                switch (nextNode) {
                case 0:
                        switch (iFirst / 64) {
                        case 6:
                                temp7 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 5:
                                temp6 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 4:
                                temp5 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 3:
                                temp4 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 2:
                                temp3 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 1:
                                temp2 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1))) & interlacedMask2[iFirst % 64]);
                        }
                        break;
                case 1:
                        switch (iFirst / 64) {
                        case 6:
                                temp7 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 5:
                                temp6 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 4:
                                temp5 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 3:
                                temp4 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 2:
                                temp3 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 1:
                                temp2 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1))) & interlacedMask2[iFirst % 64]);
                        }
                        break;
                case 2:
                        switch (iFirst / 64) {
                        case 6:
                                temp7 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 5:
                                temp6 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 4:
                                temp5 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 3:
                                temp4 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 2:
                                temp3 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 1:
                                temp2 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1))) & interlacedMask2[iFirst % 64]);
                        }
                        break;
                default:
                        switch (iFirst / 64) {
                        case 6:
                                temp7 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 5:
                                temp6 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 4:
                                temp5 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 3:
                                temp4 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 2:
                                temp3 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 1:
                                temp2 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1))) & interlacedMask2[iFirst % 64]);
                        }
                }
                rank += temp1 + temp2 + temp3 + temp4 + temp5 + temp6 + temp7;

                if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 16 * (rank / 448), 0, 3);
                iFirst = rank;
                
                // *** last
                j = iLast / 448;
                p = wt->alignedBits + 16 * j + nextNode / 2;
                if ((nextNode & 1) == 0) rank = (*p) >> 32;
                //else rank = (*p) & 0x00000000FFFFFFFFULL;
                else rank = (unsigned int)(*p);
                p += (2 - nextNode / 2);

                iLast -= (j * 448);
                temp1 = 0;
                temp2 = 0;
                temp3 = 0;
                temp4 = 0;
                temp5 = 0;
                temp6 = 0;
                temp7 = 0;
                switch (nextNode) {
                case 0:
                        switch (iLast / 64) {
                        case 6:
                                temp7 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 5:
                                temp6 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 4:
                                temp5 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 3:
                                temp4 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 2:
                                temp3 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 1:
                                temp2 = __builtin_popcountll(getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT4BitVector0R(*p) | getWT4BitVector0L(*(p + 1))) & interlacedMask2[iLast % 64]);
                        }
                        break;
                case 1:
                        switch (iLast / 64) {
                        case 6:
                                temp7 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 5:
                                temp6 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 4:
                                temp5 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 3:
                                temp4 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 2:
                                temp3 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 1:
                                temp2 = __builtin_popcountll(getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT4BitVector1R(*p) | getWT4BitVector1L(*(p + 1))) & interlacedMask2[iLast % 64]);
                        }
                        break;
                case 2:
                        switch (iLast / 64) {
                        case 6:
                                temp7 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 5:
                                temp6 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 4:
                                temp5 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 3:
                                temp4 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 2:
                                temp3 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 1:
                                temp2 = __builtin_popcountll(getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT4BitVector2R(*p) | getWT4BitVector2L(*(p + 1))) & interlacedMask2[iLast % 64]);
                        }
                        break;
                default:
                        switch (iLast / 64) {
                        case 6:
                                temp7 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 5:
                                temp6 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 4:
                                temp5 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 3:
                                temp4 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 2:
                                temp3 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 1:
                                temp2 = __builtin_popcountll(getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT4BitVector3R(*p) | getWT4BitVector3L(*(p + 1))) & interlacedMask2[iLast % 64]);
                        }
                }
                rank += temp1 + temp2 + temp3 + temp4 + temp5 + temp6 + temp7;

                if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 16 * (rank / 448), 0, 3);
                iLast = rank;
                
                wt = wt->nodes[nextNode];
        }
	return make_pair(iFirst, iLast);
}

unsigned int count_WT4_1024(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen) {
	unsigned char c;
	__builtin_prefetch(wt->alignedBits + 16 * ((firstVal - 1) / 448), 0, 3);
	__builtin_prefetch(wt->alignedBits + 16 * (lastVal / 448), 0, 3);

	while (firstVal <= lastVal && i > 1) {
		c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;
                tie(firstVal, lastVal) = getRankWT4_1024(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
		firstVal += C[c] + 1;
		__builtin_prefetch(wt->alignedBits + 16 * ((firstVal - 1) / 448), 0, 3);
		lastVal += C[c];
		__builtin_prefetch(wt->alignedBits + 16 * (lastVal / 448), 0, 3);
		--i;
	}

	if (firstVal <= lastVal) {
		c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;
		tie(firstVal, lastVal) = getRankWT4_1024(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
                firstVal += C[c] + 1;
                lastVal += C[c];
	}

	if (firstVal > lastVal) return 0;
	else return lastVal - firstVal + 1;

}

unsigned long long getWT8BitVector0R(unsigned long long b) {
	b = ~b;
	return (b & (b >> 1) & (b >> 2)) & 0x1249249249249249ULL;
}

unsigned long long getWT8BitVector1R(unsigned long long b) {
	b ^= 0x6DB6DB6DB6DB6DB6ULL;
	return (b & (b >> 1) & (b >> 2)) & 0x1249249249249249ULL;
}

unsigned long long getWT8BitVector2R(unsigned long long b) {
	b ^= 0x5B6DB6DB6DB6DB6DULL;
	return (b & (b >> 1) & (b >> 2)) & 0x1249249249249249ULL;
}

unsigned long long getWT8BitVector3R(unsigned long long b) {
	b ^= 0x4924924924924924ULL;
	return (b & (b >> 1) & (b >> 2)) & 0x1249249249249249ULL;
}

unsigned long long getWT8BitVector4R(unsigned long long b) {
	b ^= 0x36DB6DB6DB6DB6DBULL;
	return (b & (b >> 1) & (b >> 2)) & 0x1249249249249249ULL;
}

unsigned long long getWT8BitVector5R(unsigned long long b) {
	b ^= 0x2492492492492492ULL;
	return (b & (b >> 1) & (b >> 2)) & 0x1249249249249249ULL;
}

unsigned long long getWT8BitVector6R(unsigned long long b) {
	b ^= 0x1249249249249249ULL;
	return (b & (b >> 1) & (b >> 2)) & 0x1249249249249249ULL;
}

unsigned long long getWT8BitVector7R(unsigned long long b) {
	return (b & (b >> 1) & (b >> 2)) & 0x1249249249249249ULL;
}

unsigned long long getWT8BitVector0M(unsigned long long b) {
	b = ~b;
	return (b & (b >> 1) & (b << 1)) & 0x2492492492492492ULL;
}

unsigned long long getWT8BitVector1M(unsigned long long b) {
	b ^= 0x6DB6DB6DB6DB6DB6ULL;
	return (b & (b >> 1) & (b << 1)) & 0x2492492492492492ULL;
}

unsigned long long getWT8BitVector2M(unsigned long long b) {
	b ^= 0x5B6DB6DB6DB6DB6DULL;
	return (b & (b >> 1) & (b << 1)) & 0x2492492492492492ULL;
}

unsigned long long getWT8BitVector3M(unsigned long long b) {
	b ^= 0x4924924924924924ULL;
	return (b & (b >> 1) & (b << 1)) & 0x2492492492492492ULL;
}

unsigned long long getWT8BitVector4M(unsigned long long b) {
	b ^= 0x36DB6DB6DB6DB6DBULL;
	return (b & (b >> 1) & (b << 1)) & 0x2492492492492492ULL;
}

unsigned long long getWT8BitVector5M(unsigned long long b) {
	b ^= 0x2492492492492492ULL;
	return (b & (b >> 1) & (b << 1)) & 0x2492492492492492ULL;
}

unsigned long long getWT8BitVector6M(unsigned long long b) {
	b ^= 0x1249249249249249ULL;
	return (b & (b >> 1) & (b << 1)) & 0x2492492492492492ULL;
}

unsigned long long getWT8BitVector7M(unsigned long long b) {
	return (b & (b >> 1) & (b << 1)) & 0x2492492492492492ULL;
}

unsigned long long getWT8BitVector0L(unsigned long long b) {
	b = ~b;
	return (b & (b << 1) & (b << 2)) & 0x4924924924924924ULL;
}

unsigned long long getWT8BitVector1L(unsigned long long b) {
	b ^= 0x6DB6DB6DB6DB6DB6ULL;
	return (b & (b << 1) & (b << 2)) & 0x4924924924924924ULL;
}

unsigned long long getWT8BitVector2L(unsigned long long b) {
	b ^= 0x5B6DB6DB6DB6DB6DULL;
	return (b & (b << 1) & (b << 2)) & 0x4924924924924924ULL;
}

unsigned long long getWT8BitVector3L(unsigned long long b) {
	b ^= 0x4924924924924924ULL;
	return (b & (b << 1) & (b << 2)) & 0x4924924924924924ULL;
}

unsigned long long getWT8BitVector4L(unsigned long long b) {
	b ^= 0x36DB6DB6DB6DB6DBULL;
	return (b & (b << 1) & (b << 2)) & 0x4924924924924924ULL;
}

unsigned long long getWT8BitVector5L(unsigned long long b) {
	b ^= 0x2492492492492492ULL;
	return (b & (b << 1) & (b << 2)) & 0x4924924924924924ULL;
}

unsigned long long getWT8BitVector6L(unsigned long long b) {
	b ^= 0x1249249249249249ULL;
	return (b & (b << 1) & (b << 2)) & 0x4924924924924924ULL;
}

unsigned long long getWT8BitVector7L(unsigned long long b) {
	return (b & (b << 1) & (b << 2)) & 0x4924924924924924ULL;
}

pair<unsigned int, unsigned int> getRankWT8_512(unsigned long long code, unsigned int codeLen, unsigned int iFirst, unsigned int iLast, WT *wt, unsigned int wtLevel) {
        unsigned int rank;
        
        for (wtLevel = 0; wtLevel < codeLen; ++wtLevel) {
                int nextNode = (code >> (3 * wtLevel)) & 7;

                // *** first
                unsigned int j = iFirst / 84;
                unsigned long long *p = wt->alignedBits + 8 * j + nextNode / 2;
                if ((nextNode & 1) == 0) rank = (*p) >> 32;
                //else rank = (*p) & 0x00000000FFFFFFFFULL;
                else rank = (unsigned int)(*p);
                p += (4 - nextNode / 2);

                iFirst -= (j * 84);
                unsigned int temp1 = 0;
                unsigned int temp2 = 0;
                switch (nextNode) {
                case 0:
                        switch (iFirst / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1))) & interlacedMask3[iFirst % 42]);
                        }
                        break;
                case 1:
                        switch (iFirst / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1))) & interlacedMask3[iFirst % 42]);
                        }
                        break;
                case 2:
                        switch (iFirst / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1))) & interlacedMask3[iFirst % 42]);
                        }
                        break;
                case 3:
                        switch (iFirst / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1))) & interlacedMask3[iFirst % 42]);
                        }
                        break;
                case 4:
                        switch (iFirst / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1))) & interlacedMask3[iFirst % 42]);
                        }
                        break;
                case 5:
                        switch (iFirst / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1))) & interlacedMask3[iFirst % 42]);
                        }
                        break;
                case 6:
                        switch (iFirst / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1))) & interlacedMask3[iFirst % 42]);
                        }
                        break;
                default:
                        switch (iFirst / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1))) & interlacedMask3[iFirst % 42]);
                        }
                }
                rank += temp1 + temp2;

                if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 8 * (rank / 84), 0, 3);
                iFirst = rank;
                
                // *** last
                j = iLast / 84;
                p = wt->alignedBits + 8 * j + nextNode / 2;
                if ((nextNode & 1) == 0) rank = (*p) >> 32;
                //else rank = (*p) & 0x00000000FFFFFFFFULL;
                else rank = (unsigned int)(*p);
                p += (4 - nextNode / 2);

                iLast -= (j * 84);
                temp1 = 0;
                temp2 = 0;
                switch (nextNode) {
                case 0:
                        switch (iLast / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1))) & interlacedMask3[iLast % 42]);
                        }
                        break;
                case 1:
                        switch (iLast / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1))) & interlacedMask3[iLast % 42]);
                        }
                        break;
                case 2:
                        switch (iLast / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1))) & interlacedMask3[iLast % 42]);
                        }
                        break;
                case 3:
                        switch (iLast / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1))) & interlacedMask3[iLast % 42]);
                        }
                        break;
                case 4:
                        switch (iLast / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1))) & interlacedMask3[iLast % 42]);
                        }
                        break;
                case 5:
                        switch (iLast / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1))) & interlacedMask3[iLast % 42]);
                        }
                        break;
                case 6:
                        switch (iLast / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1))) & interlacedMask3[iLast % 42]);
                        }
                        break;
                default:
                        switch (iLast / 42) {
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)));
                                p += 2;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1))) & interlacedMask3[iLast % 42]);
                        }
                }
                rank += temp1 + temp2;

                if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 8 * (rank / 84), 0, 3);
                iLast = rank;
                
                wt = wt->nodes[nextNode];
        }
	return make_pair(iFirst, iLast);
}

unsigned int count_WT8_512(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen) {
	unsigned char c;
	__builtin_prefetch(wt->alignedBits + 8 * ((firstVal - 1) / 84), 0, 3);
	__builtin_prefetch(wt->alignedBits + 8 * (lastVal / 84), 0, 3);

	while (firstVal <= lastVal && i > 1) {
		c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;
                tie(firstVal, lastVal) = getRankWT8_512(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
		firstVal += C[c] + 1;
		__builtin_prefetch(wt->alignedBits + 8 * ((firstVal - 1) / 84), 0, 3);
		lastVal += C[c];
		__builtin_prefetch(wt->alignedBits + 8 * (lastVal / 84), 0, 3);
		--i;
	}

	if (firstVal <= lastVal) {
		c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;
		tie(firstVal, lastVal) = getRankWT8_512(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
                firstVal += C[c] + 1;
                lastVal += C[c];
	}

	if (firstVal > lastVal) return 0;
	else return lastVal - firstVal + 1;

}

pair<unsigned int, unsigned int> getRankWT8_1024(unsigned long long code, unsigned int codeLen, unsigned int iFirst, unsigned int iLast, WT *wt, unsigned int wtLevel) {
        unsigned int rank;
        
        for (wtLevel = 0; wtLevel < codeLen; ++wtLevel) {
                int nextNode = (code >> (3 * wtLevel)) & 7;

                // *** first
                unsigned int j = iFirst / 252;
                unsigned long long *p = wt->alignedBits + 16 * j + nextNode / 2;
                if ((nextNode & 1) == 0) rank = (*p) >> 32;
                //else rank = (*p) & 0x00000000FFFFFFFFULL;
                else rank = (unsigned int)(*p);
                p += (4 - nextNode / 2);

                iFirst -= (j * 252);
                unsigned int temp1 = 0;
                unsigned int temp2 = 0;
                unsigned int temp3 = 0;
                unsigned int temp4 = 0;
                switch (nextNode) {
                case 0:
                        switch (iFirst / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)) | getWT8BitVector0L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)) | getWT8BitVector0L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)) | getWT8BitVector0L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)) | getWT8BitVector0L(*(p + 2))) & interlacedMask3[iFirst % 63]);
                        }
                        break;
                case 1:
                        switch (iFirst / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)) | getWT8BitVector1L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)) | getWT8BitVector1L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)) | getWT8BitVector1L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)) | getWT8BitVector1L(*(p + 2))) & interlacedMask3[iFirst % 63]);
                        }
                        break;
                case 2:
                        switch (iFirst / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)) | getWT8BitVector2L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)) | getWT8BitVector2L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)) | getWT8BitVector2L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)) | getWT8BitVector2L(*(p + 2))) & interlacedMask3[iFirst % 63]);
                        }
                        break;
                case 3:
                        switch (iFirst / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)) | getWT8BitVector3L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)) | getWT8BitVector3L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)) | getWT8BitVector3L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)) | getWT8BitVector3L(*(p + 2))) & interlacedMask3[iFirst % 63]);
                        }
                        break;
                case 4:
                        switch (iFirst / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)) | getWT8BitVector4L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)) | getWT8BitVector4L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)) | getWT8BitVector4L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)) | getWT8BitVector4L(*(p + 2))) & interlacedMask3[iFirst % 63]);
                        }
                        break;
                case 5:
                        switch (iFirst / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)) | getWT8BitVector5L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)) | getWT8BitVector5L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)) | getWT8BitVector5L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)) | getWT8BitVector5L(*(p + 2))) & interlacedMask3[iFirst % 63]);
                        }
                        break;
                case 6:
                        switch (iFirst / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)) | getWT8BitVector6L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)) | getWT8BitVector6L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)) | getWT8BitVector6L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)) | getWT8BitVector6L(*(p + 2))) & interlacedMask3[iFirst % 63]);
                        }
                        break;
                default:
                        switch (iFirst / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)) | getWT8BitVector7L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)) | getWT8BitVector7L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)) | getWT8BitVector7L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)) | getWT8BitVector7L(*(p + 2))) & interlacedMask3[iFirst % 63]);
                        }
                }
                rank += temp1 + temp2 + temp3 + temp4;

                if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 16 * (rank / 252), 0, 3);
                iFirst = rank;
                
                // *** last
                j = iLast / 252;
                p = wt->alignedBits + 16 * j + nextNode / 2;
                if ((nextNode & 1) == 0) rank = (*p) >> 32;
                //else rank = (*p) & 0x00000000FFFFFFFFULL;
                else rank = (unsigned int)(*p);
                p += (4 - nextNode / 2);

                iLast -= (j * 252);
                temp1 = 0;
                temp2 = 0;
                temp3 = 0;
                temp4 = 0;
                switch (nextNode) {
                case 0:
                        switch (iLast / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)) | getWT8BitVector0L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)) | getWT8BitVector0L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)) | getWT8BitVector0L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector0R(*p) | getWT8BitVector0M(*(p + 1)) | getWT8BitVector0L(*(p + 2))) & interlacedMask3[iLast % 63]);
                        }
                        break;
                case 1:
                        switch (iLast / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)) | getWT8BitVector1L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)) | getWT8BitVector1L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)) | getWT8BitVector1L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector1R(*p) | getWT8BitVector1M(*(p + 1)) | getWT8BitVector1L(*(p + 2))) & interlacedMask3[iLast % 63]);
                        }
                        break;
                case 2:
                        switch (iLast / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)) | getWT8BitVector2L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)) | getWT8BitVector2L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)) | getWT8BitVector2L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector2R(*p) | getWT8BitVector2M(*(p + 1)) | getWT8BitVector2L(*(p + 2))) & interlacedMask3[iLast % 63]);
                        }
                        break;
                case 3:
                        switch (iLast / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)) | getWT8BitVector3L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)) | getWT8BitVector3L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)) | getWT8BitVector3L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector3R(*p) | getWT8BitVector3M(*(p + 1)) | getWT8BitVector3L(*(p + 2))) & interlacedMask3[iLast % 63]);
                        }
                        break;
                case 4:
                        switch (iLast / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)) | getWT8BitVector4L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)) | getWT8BitVector4L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)) | getWT8BitVector4L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector4R(*p) | getWT8BitVector4M(*(p + 1)) | getWT8BitVector4L(*(p + 2))) & interlacedMask3[iLast % 63]);
                        }
                        break;
                case 5:
                        switch (iLast / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)) | getWT8BitVector5L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)) | getWT8BitVector5L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)) | getWT8BitVector5L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector5R(*p) | getWT8BitVector5M(*(p + 1)) | getWT8BitVector5L(*(p + 2))) & interlacedMask3[iLast % 63]);
                        }
                        break;
                case 6:
                        switch (iLast / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)) | getWT8BitVector6L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)) | getWT8BitVector6L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)) | getWT8BitVector6L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector6R(*p) | getWT8BitVector6M(*(p + 1)) | getWT8BitVector6L(*(p + 2))) & interlacedMask3[iLast % 63]);
                        }
                        break;
                default:
                        switch (iLast / 63) {
                        case 3:
                                temp4 = __builtin_popcountll(getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)) | getWT8BitVector7L(*(p + 2)));
                                p += 3;
                        case 2:
                                temp3 = __builtin_popcountll(getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)) | getWT8BitVector7L(*(p + 2)));
                                p += 3;
                        case 1:
                                temp2 = __builtin_popcountll(getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)) | getWT8BitVector7L(*(p + 2)));
                                p += 3;
                        case 0:
                                temp1 = __builtin_popcountll((getWT8BitVector7R(*p) | getWT8BitVector7M(*(p + 1)) | getWT8BitVector7L(*(p + 2))) & interlacedMask3[iLast % 63]);
                        }
                }
                rank += temp1 + temp2 + temp3 + temp4;

                if (wtLevel + 1 < codeLen) __builtin_prefetch(wt->nodes[nextNode]->alignedBits + 16 * (rank / 252), 0, 3);
                iLast = rank;
                
                wt = wt->nodes[nextNode];
        }
	return make_pair(iFirst, iLast);
}

unsigned int count_WT8_1024(unsigned char *pattern, unsigned int i, unsigned int *C, WT *wt, unsigned int firstVal, unsigned int lastVal, unsigned long long *code, unsigned int *codeLen) {
	unsigned char c;
	__builtin_prefetch(wt->alignedBits + 16 * ((firstVal - 1) / 252), 0, 3);
	__builtin_prefetch(wt->alignedBits + 16 * (lastVal / 252), 0, 3);

	while (firstVal <= lastVal && i > 1) {
		c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;
                tie(firstVal, lastVal) = getRankWT8_1024(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
		firstVal += C[c] + 1;
		__builtin_prefetch(wt->alignedBits + 16 * ((firstVal - 1) / 252), 0, 3);
		lastVal += C[c];
		__builtin_prefetch(wt->alignedBits + 16 * (lastVal / 252), 0, 3);
		--i;
	}

	if (firstVal <= lastVal) {
		c = pattern[i - 1];
                if (codeLen[c] == 0) return 0;
		tie(firstVal, lastVal) = getRankWT8_1024(code[c], codeLen[c], firstVal - 1, lastVal, wt, 0);
                firstVal += C[c] + 1;
                lastVal += C[c];
	}

	if (firstVal > lastVal) return 0;
	else return lastVal - firstVal + 1;

}

}
