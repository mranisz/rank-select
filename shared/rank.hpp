#ifndef RANK_HPP
#define	RANK_HPP

#include <iostream>

using namespace std;
    
namespace shared {
    
enum RankBasicType {
        STANDARD = 1,
        WITH_COMPRESSED_HEADERS = 2
};

template<RankBasicType T> class RankBasic {
private:
	void freeMemory() {
            if (this->ranks != NULL) delete[] this->ranks;
            if (this->bits != NULL) delete[] this->bits;
        }
        
	void initialize() {
            this->ranks = NULL;
            this->alignedRanks = NULL;
            this->ranksLen = 0;
            this->bits = NULL;
            this->alignedBits = NULL;
            this->bitsLen = 0;
            this->textLen = 0;
            this->pointer = NULL;
            this->pointer2 = NULL;
        }
        
        void build_std(unsigned char *text, unsigned int extendedTextLen, bool *emptyBlock, unsigned int notEmptyBlocks) {
            this->bitsLen = notEmptyBlocks * (BLOCK_LEN / sizeof(unsigned long long));
            this->bits = new unsigned long long[this->bitsLen + 16];
            this->alignedBits = this->bits;
            while ((unsigned long long)this->alignedBits % 128) ++this->alignedBits;

            this->ranksLen = 2 * (extendedTextLen / BLOCK_LEN) + 1;
            this->ranks = new unsigned int[this->ranksLen + 32];
            this->alignedRanks = this->ranks;
            while ((unsigned long long)this->alignedRanks % 128) ++this->alignedRanks;

            unsigned int rank = 0, rankCounter = 0, bitsCounter = 0;
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                this->alignedRanks[rankCounter++] = rank;
                this->alignedRanks[rankCounter++] = bitsCounter;
                if (emptyBlock[i / BLOCK_LEN]) {
                    if ((unsigned int)text[i] == 255) rank += BLOCK_IN_BITS;
                    continue;
                }
                for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                    if (j % sizeof(unsigned long long) == 0) this->alignedBits[bitsCounter] = 0;
                    if (i + j < this->textLen) this->alignedBits[bitsCounter] += (((unsigned long long)text[i + j]) << (8 * (sizeof(unsigned long long) - j % sizeof(unsigned long long) - 1)));
                    if (j % sizeof(unsigned long long) == 7) rank += __builtin_popcountll(this->alignedBits[bitsCounter++]);
                }
            }
            this->alignedRanks[rankCounter++] = rank;
        }
        
        void build_bch(unsigned char *text, unsigned int extendedTextLen, bool *emptyBlock, unsigned int notEmptyBlocks) {
            this->bitsLen = notEmptyBlocks * sizeof(unsigned long long);
            this->bits = new unsigned long long[this->bitsLen + 16];
            this->alignedBits = this->bits;
            while ((unsigned long long)this->alignedBits % 128) ++this->alignedBits;

            unsigned int ranksTempLen = 2 * (extendedTextLen / BLOCK_LEN) + 2;
            if (ranksTempLen % (2 * SUPERBLOCKLEN) > 0) ranksTempLen += ((2 * SUPERBLOCKLEN) - ranksTempLen % (2 * SUPERBLOCKLEN));
            unsigned int *ranksTemp = new unsigned int[ranksTempLen];

            unsigned int rank = 0, rankCounter = 0, bitsCounter = 0;
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                ranksTemp[rankCounter++] = rank;
                ranksTemp[rankCounter++] = bitsCounter;
                if (emptyBlock[i / BLOCK_LEN]) {
                    if ((unsigned int)text[i] == 255) rank += BLOCK_IN_BITS;
                    continue;
                }
                for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                    if (j % sizeof(unsigned long long) == 0) this->alignedBits[bitsCounter] = 0;
                    if (i + j < this->textLen) this->alignedBits[bitsCounter] += (((unsigned long long)text[i + j]) << (sizeof(unsigned long long) * (sizeof(unsigned long long) - j % sizeof(unsigned long long) - 1)));
                    if (j % sizeof(unsigned long long) == 7) rank += __builtin_popcountll(this->alignedBits[bitsCounter++]);
                }
            }
            ranksTemp[rankCounter++] = rank;
            ranksTemp[rankCounter++] = 0;

            for (unsigned int i = rankCounter; i < ranksTempLen; ++i) {
                ranksTemp[i] = 0;
            }

            this->ranksLen = ranksTempLen / 2;
            this->ranksLen = (this->ranksLen / SUPERBLOCKLEN) * INTSINSUPERBLOCK;
            ranksTempLen /= 2;

            this->ranks = new unsigned int[this->ranksLen + 32];
            this->alignedRanks = this->ranks;
            while ((unsigned long long)this->alignedRanks % 128) ++this->alignedRanks;

            unsigned int bucket[INTSINSUPERBLOCK];
            rankCounter = 0;
            for (unsigned int j = 0; j < INTSINSUPERBLOCK; ++j) bucket[j] = 0;

            for (unsigned int i = 0; i < ranksTempLen; i += SUPERBLOCKLEN) {
                bucket[0] = ranksTemp[2 * i];
                for (unsigned int j = 0; j < SUPERBLOCKLEN - 1; ++j) {
                    bucket[j / 2 + 1] += ((ranksTemp[2 * (i + j + 1)] - bucket[0]) << (16 * (j % 2)));
                }
                bucket[OFFSETSTARTINSUPERBLOCK] = ranksTemp[2 * i + 1];
                for (unsigned int j = 0; j < SUPERBLOCKLEN - 1; ++j) {
                    if (!emptyBlock[i + j]) bucket[OFFSETSTARTINSUPERBLOCK + 1] += (1U << (31 - j));
                }
                for (unsigned int j = 0; j < INTSINSUPERBLOCK; ++j) {
                    this->alignedRanks[rankCounter++] = bucket[j];
                    bucket[j] = 0;
                }
            }
            delete[] ranksTemp;
        }
        
        unsigned int getRank_std(unsigned int i) {
            unsigned int start = i / BLOCK_IN_BITS;
            unsigned int rank = this->alignedRanks[2 * start];
            i %= BLOCK_IN_BITS;
            unsigned int rankDiff = this->alignedRanks[2 * start + 2] - rank;
            if ((rankDiff & BLOCK_MASK) == 0) return rank + i * (rankDiff >> 9);
            this->pointer = this->alignedBits + this->alignedRanks[2 * start + 1];
            switch (i / 64) {
            case 7:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        rank += __builtin_popcountll(this->pointer[3]);
                        rank += __builtin_popcountll(this->pointer[4]);
                        rank += __builtin_popcountll(this->pointer[5]);
                        rank += __builtin_popcountll(this->pointer[6]);
                        return rank + __builtin_popcountll(this->pointer[7] & masks[i - 448]);
            case 6:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        rank += __builtin_popcountll(this->pointer[3]);
                        rank += __builtin_popcountll(this->pointer[4]);
                        rank += __builtin_popcountll(this->pointer[5]);
                        return rank + __builtin_popcountll(this->pointer[6] & masks[i - 384]);
            case 5:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        rank += __builtin_popcountll(this->pointer[3]);
                        rank += __builtin_popcountll(this->pointer[4]);
                        return rank + __builtin_popcountll(this->pointer[5] & masks[i - 320]);
            case 4:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        rank += __builtin_popcountll(this->pointer[3]);
                        return rank + __builtin_popcountll(this->pointer[4] & masks[i - 256]);
            case 3:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        return rank + __builtin_popcountll(this->pointer[3] & masks[i - 192]);
            case 2:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        return rank + __builtin_popcountll(this->pointer[2] & masks[i - 128]);
            case 1:
                        rank += __builtin_popcountll(this->pointer[0]);
                        return rank + __builtin_popcountll(this->pointer[1] & masks[i - 64]);
            default:
                        return rank + __builtin_popcountll(this->pointer[0] & masks[i]);
            }
        }

        unsigned int getRank_bch(unsigned int i) {
            unsigned int start = i / BLOCK_IN_BITS;
            this->pointer2 = this->alignedRanks + INTSINSUPERBLOCK * (start / SUPERBLOCKLEN);
            unsigned int rank = *(this->pointer2);
            unsigned int rankNumInCL = (start % SUPERBLOCKLEN);
            if (rankNumInCL > 0) {
                if (rankNumInCL % 2 == 1) rank += (this->pointer2[(rankNumInCL + 1) / 2] & 0xFFFF);
                else rank += (this->pointer2[(rankNumInCL + 1) / 2] >> 16);
            }
            i %= BLOCK_IN_BITS;
            unsigned int rank2;
            if (rankNumInCL == (SUPERBLOCKLEN - 1)) rank2 = this->pointer2[INTSINSUPERBLOCK];
            else {
                rank2 = *(this->pointer2);
                if (rankNumInCL % 2 == 0) rank2 += (this->pointer2[(rankNumInCL + 2) / 2] & 0xFFFF);
                else rank2 += (this->pointer2[(rankNumInCL + 2) / 2] >> 16);
            }
            unsigned int rankDiff = rank2 - rank;
            if ((rankDiff & BLOCK_MASK) == 0) return rank + i * (rankDiff >> 9);
            if (rankNumInCL > 0) this->pointer = this->alignedBits + this->pointer2[OFFSETSTARTINSUPERBLOCK] + 8 * __builtin_popcount(this->pointer2[OFFSETSTARTINSUPERBLOCK + 1] & masks2[rankNumInCL]);
            else this->pointer = this->alignedBits + this->pointer2[OFFSETSTARTINSUPERBLOCK];
            switch (i / 64) {
            case 7:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        rank += __builtin_popcountll(this->pointer[3]);
                        rank += __builtin_popcountll(this->pointer[4]);
                        rank += __builtin_popcountll(this->pointer[5]);
                        rank += __builtin_popcountll(this->pointer[6]);
                        return rank + __builtin_popcountll(this->pointer[7] & masks[i - 448]);
            case 6:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        rank += __builtin_popcountll(this->pointer[3]);
                        rank += __builtin_popcountll(this->pointer[4]);
                        rank += __builtin_popcountll(this->pointer[5]);
                        return rank + __builtin_popcountll(this->pointer[6] & masks[i - 384]);
            case 5:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        rank += __builtin_popcountll(this->pointer[3]);
                        rank += __builtin_popcountll(this->pointer[4]);
                        return rank + __builtin_popcountll(this->pointer[5] & masks[i - 320]);
            case 4:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        rank += __builtin_popcountll(this->pointer[3]);
                        return rank + __builtin_popcountll(this->pointer[4] & masks[i - 256]);
            case 3:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        rank += __builtin_popcountll(this->pointer[2]);
                        return rank + __builtin_popcountll(this->pointer[3] & masks[i - 192]);
            case 2:
                        rank += __builtin_popcountll(this->pointer[0]);
                        rank += __builtin_popcountll(this->pointer[1]);
                        return rank + __builtin_popcountll(this->pointer[2] & masks[i - 128]);
            case 1:
                        rank += __builtin_popcountll(this->pointer[0]);
                        return rank + __builtin_popcountll(this->pointer[1] & masks[i - 64]);
            default:
                        return rank + __builtin_popcountll(this->pointer[0] & masks[i]);
            }
        }

public:
	unsigned int *ranks;
        unsigned int *alignedRanks;
        unsigned int ranksLen;
        unsigned long long *bits;
        unsigned long long *alignedBits;
        unsigned int bitsLen;
        unsigned int textLen;
        unsigned long long *pointer;
        unsigned int* pointer2;
        
        const static unsigned int BLOCK_LEN = sizeof(unsigned long long) * 8;
        const static unsigned int BLOCK_IN_BITS = 8 * BLOCK_LEN;

        const static unsigned int BLOCK_MASK = (1 << 9) - 1;

        const static unsigned int SUPERBLOCKLEN = 32;
        const static unsigned int OFFSETSTARTINSUPERBLOCK = 17;
        const static unsigned int INTSINSUPERBLOCK = 19;
        /*
        INTSINSUPERBLOCK = 2 + (SUPERBLOCKLEN - 1) / 2 + (SUPERBLOCKLEN - 1) / 32;
        if ((SUPERBLOCKLEN - 1) % 2 == 1) ++INTSINSUPERBLOCK;
        if ((SUPERBLOCKLEN - 1) % 32 > 0) ++INTSINSUPERBLOCK;
        * 
        OFFSETSTARTINSUPERBLOCK = 1 + (SUPERBLOCKLEN - 1) / 2;
        if ((SUPERBLOCKLEN - 1) % 2 == 1) ++OFFSETSTARTINSUPERBLOCK;
        */

        const unsigned long long masks[64] = {
            0x0000000000000000,
            0x8000000000000000,
            0xC000000000000000,
            0xE000000000000000,
            0xF000000000000000,
            0xF800000000000000,
            0xFC00000000000000,
            0xFE00000000000000,
            0xFF00000000000000,
            0xFF80000000000000,
            0xFFC0000000000000,
            0xFFE0000000000000,
            0xFFF0000000000000,
            0xFFF8000000000000,
            0xFFFC000000000000,
            0xFFFE000000000000,
            0xFFFF000000000000,
            0xFFFF800000000000,
            0xFFFFC00000000000,
            0xFFFFE00000000000,
            0xFFFFF00000000000,
            0xFFFFF80000000000,
            0xFFFFFC0000000000,
            0xFFFFFE0000000000,
            0xFFFFFF0000000000,
            0xFFFFFF8000000000,
            0xFFFFFFC000000000,
            0xFFFFFFE000000000,
            0xFFFFFFF000000000,
            0xFFFFFFF800000000,
            0xFFFFFFFC00000000,
            0xFFFFFFFE00000000,
            0xFFFFFFFF00000000,
            0xFFFFFFFF80000000,
            0xFFFFFFFFC0000000,
            0xFFFFFFFFE0000000,
            0xFFFFFFFFF0000000,
            0xFFFFFFFFF8000000,
            0xFFFFFFFFFC000000,
            0xFFFFFFFFFE000000,
            0xFFFFFFFFFF000000,
            0xFFFFFFFFFF800000,
            0xFFFFFFFFFFC00000,
            0xFFFFFFFFFFE00000,
            0xFFFFFFFFFFF00000,
            0xFFFFFFFFFFF80000,
            0xFFFFFFFFFFFC0000,
            0xFFFFFFFFFFFE0000,
            0xFFFFFFFFFFFF0000,
            0xFFFFFFFFFFFF8000,
            0xFFFFFFFFFFFFC000,
            0xFFFFFFFFFFFFE000,
            0xFFFFFFFFFFFFF000,
            0xFFFFFFFFFFFFF800,
            0xFFFFFFFFFFFFFC00,
            0xFFFFFFFFFFFFFE00,
            0xFFFFFFFFFFFFFF00,
            0xFFFFFFFFFFFFFF80,
            0xFFFFFFFFFFFFFFC0,
            0xFFFFFFFFFFFFFFE0,
            0xFFFFFFFFFFFFFFF0,
            0xFFFFFFFFFFFFFFF8,
            0xFFFFFFFFFFFFFFFC,
            0xFFFFFFFFFFFFFFFE
        };
        
        const unsigned int masks2[32] = {
            0x00000000,
            0x80000000,
            0xC0000000,
            0xE0000000,
            0xF0000000,
            0xF8000000,
            0xFC000000,
            0xFE000000,
            0xFF000000,
            0xFF800000,
            0xFFC00000,
            0xFFE00000,
            0xFFF00000,
            0xFFF80000,
            0xFFFC0000,
            0xFFFE0000,
            0xFFFF0000,
            0xFFFF8000,
            0xFFFFC000,
            0xFFFFE000,
            0xFFFFF000,
            0xFFFFF800,
            0xFFFFFC00,
            0xFFFFFE00,
            0xFFFFFF00,
            0xFFFFFF80,
            0xFFFFFFC0,
            0xFFFFFFE0,
            0xFFFFFFF0,
            0xFFFFFFF8,
            0xFFFFFFFC,
            0xFFFFFFFE
        };

	RankBasic() {
		this->initialize();
	}

	~RankBasic() {
		this->freeMemory();
	}

        void build(unsigned char *text, unsigned int textLen) {
            this->free();
            this->textLen = textLen;
            unsigned int extendedTextLen = this->textLen;
            if (this->textLen % BLOCK_LEN > 0) extendedTextLen += (BLOCK_LEN - this->textLen % BLOCK_LEN);

            bool *emptyBlock = new bool[extendedTextLen / BLOCK_LEN];
            bool homoblock;
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                homoblock = true;
                for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                    if (i + j >= this->textLen || (unsigned int)text[i + j] > 0) {
                        homoblock = false;
                        break;
                    }
                }
                if (!homoblock) {
                    homoblock = true;
                    for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                        if (i + j >= this->textLen || (unsigned int)text[i + j] < 255) {
                            homoblock = false;
                            break;
                        }
                    }
                }
                emptyBlock[i / BLOCK_LEN] = homoblock;
            }

            unsigned int notEmptyBlocks = 0;
            for (unsigned int i = 0; i < extendedTextLen / BLOCK_LEN; ++i) if (!emptyBlock[i]) ++notEmptyBlocks;

            switch(T) {
                case RankBasicType::WITH_COMPRESSED_HEADERS:
                    this->build_bch(text, extendedTextLen, emptyBlock, notEmptyBlocks);
                    break;
                default:
                    this->build_std(text, extendedTextLen, emptyBlock, notEmptyBlocks);
                    break;      
            }

            delete[] emptyBlock;
        }
        
        unsigned int rank(unsigned int i) {
                switch(T) {
                case RankBasicType::WITH_COMPRESSED_HEADERS:
                        return this->getRank_bch(i);
                        break;
                default:
                        return this->getRank_std(i);
                        break;
                }
        } 
           
	void save(FILE *outFile) {
            fwrite(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(&this->ranksLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            if (this->ranksLen > 0) fwrite(this->alignedRanks, (size_t)sizeof(unsigned int), (size_t)this->ranksLen, outFile);
            fwrite(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            if (this->bitsLen > 0) fwrite(this->alignedBits, (size_t)sizeof(unsigned long long), (size_t)this->bitsLen, outFile);
        }
	
        void load(FILE *inFile) {
            this->free();
            size_t result;
            result = fread(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            result = fread(&this->ranksLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            if (this->ranksLen > 0) {
                    this->ranks = new unsigned int[this->ranksLen + 32];
                    this->alignedRanks = this->ranks;
                    while ((unsigned long long)(this->alignedRanks) % 128) ++(this->alignedRanks);
                    result = fread(this->alignedRanks, (size_t)sizeof(unsigned int), (size_t)this->ranksLen, inFile);
                    if (result != this->ranksLen) {
                            cout << "Error loading rank" << endl;
                            exit(1);
                    }
            }
            result = fread(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            if (this->bitsLen > 0) {
                    this->bits = new unsigned long long[this->bitsLen + 16];
                    this->alignedBits = this->bits;
                    while ((unsigned long long)(this->alignedBits) % 128) ++(this->alignedBits);
                    result = fread(this->alignedBits, (size_t)sizeof(unsigned long long), (size_t)this->bitsLen, inFile);
                    if (result != this->bitsLen) {
                            cout << "Error loading rank" << endl;
                            exit(1);
                    }
            }
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
	
        unsigned int getSize() {
            unsigned int size = sizeof(this->ranksLen) + sizeof(this->bitsLen) + sizeof(this->pointer) + sizeof(this->pointer2);
            if (this->ranksLen > 0) size += (this->ranksLen + 32) * sizeof(unsigned int);
            if (this->bitsLen > 0) size += (this->bitsLen + 16) * sizeof(unsigned long long);
            return size;
        }
        
        unsigned int getTextSize() {
            return this->textLen * sizeof(unsigned char);
        }
        
        void testRank(unsigned char* text, unsigned int textLen) {
            unsigned int rankTest = 0;

            for (unsigned int i = 0; i < textLen; ++i) {
                unsigned int bits = (unsigned int)text[i];
                for (unsigned int j = 0; j < 8; ++j) {
                    if (((bits >> (7 - j)) & 1) == 1) ++rankTest;
                    if (rankTest != this->rank(8 * i + j + 1)) {
                        cout << (8 * i + j + 1) << " " << rankTest << " " << this->rank(8 * i + j + 1);
                        cin.ignore();
                    }
                }
            }
        }
};

class RankCF {
private:
	void freeMemory() {
            if (this->bits != NULL) delete[] this->bits;
        }
        
	void initialize() {
            this->bits = NULL;
            this->alignedBits = NULL;
            this->bitsLen = 0;
            this->textLen = 0;
            this->border = 0;
            this->blockBorder = 0;
            this->pointer = NULL;
        }

public:
        unsigned int *bits;
        unsigned int *alignedBits;
        unsigned int bitsLen;
        unsigned int textLen;
        unsigned int border;
        unsigned int blockBorder;
        unsigned int *pointer;
        
        const static unsigned int INT_BLOCK_LEN = 16;
        const static unsigned int INT_BLOCK_LEN_WITH_RANK = INT_BLOCK_LEN + 1;
        const static unsigned int BLOCK_LEN = sizeof(unsigned int) * 16;
        const static unsigned int BLOCK_IN_BITS = 8 * BLOCK_LEN;

        const static unsigned int BLOCK_MASK = (1 << 9) - 1;

        unsigned long long masks[64] = {
            0x0000000000000000ULL,
            0x0000000080000000ULL,
            0x00000000C0000000ULL,
            0x00000000E0000000ULL,
            0x00000000F0000000ULL,
            0x00000000F8000000ULL,
            0x00000000FC000000ULL,
            0x00000000FE000000ULL,
            0x00000000FF000000ULL,
            0x00000000FF800000ULL,
            0x00000000FFC00000ULL,
            0x00000000FFE00000ULL,
            0x00000000FFF00000ULL,
            0x00000000FFF80000ULL,
            0x00000000FFFC0000ULL,
            0x00000000FFFE0000ULL,
            0x00000000FFFF0000ULL,
            0x00000000FFFF8000ULL,
            0x00000000FFFFC000ULL,
            0x00000000FFFFE000ULL,
            0x00000000FFFFF000ULL,
            0x00000000FFFFF800ULL,
            0x00000000FFFFFC00ULL,
            0x00000000FFFFFE00ULL,
            0x00000000FFFFFF00ULL,
            0x00000000FFFFFF80ULL,
            0x00000000FFFFFFC0ULL,
            0x00000000FFFFFFE0ULL,
            0x00000000FFFFFFF0ULL,
            0x00000000FFFFFFF8ULL,
            0x00000000FFFFFFFCULL,
            0x00000000FFFFFFFEULL,
            0x00000000FFFFFFFFULL,
            0x80000000FFFFFFFFULL,
            0xC0000000FFFFFFFFULL,
            0xE0000000FFFFFFFFULL,
            0xF0000000FFFFFFFFULL,
            0xF8000000FFFFFFFFULL,
            0xFC000000FFFFFFFFULL,
            0xFE000000FFFFFFFFULL,
            0xFF000000FFFFFFFFULL,
            0xFF800000FFFFFFFFULL,
            0xFFC00000FFFFFFFFULL,
            0xFFE00000FFFFFFFFULL,
            0xFFF00000FFFFFFFFULL,
            0xFFF80000FFFFFFFFULL,
            0xFFFC0000FFFFFFFFULL,
            0xFFFE0000FFFFFFFFULL,
            0xFFFF0000FFFFFFFFULL,
            0xFFFF8000FFFFFFFFULL,
            0xFFFFC000FFFFFFFFULL,
            0xFFFFE000FFFFFFFFULL,
            0xFFFFF000FFFFFFFFULL,
            0xFFFFF800FFFFFFFFULL,
            0xFFFFFC00FFFFFFFFULL,
            0xFFFFFE00FFFFFFFFULL,
            0xFFFFFF00FFFFFFFFULL,
            0xFFFFFF80FFFFFFFFULL,
            0xFFFFFFC0FFFFFFFFULL,
            0xFFFFFFE0FFFFFFFFULL,
            0xFFFFFFF0FFFFFFFFULL,
            0xFFFFFFF8FFFFFFFFULL,
            0xFFFFFFFCFFFFFFFFULL,
            0xFFFFFFFEFFFFFFFFULL   
        };

	RankCF() {
		this->initialize();
	}

	~RankCF() {
		this->freeMemory();
	}

        void build(unsigned char *text, unsigned int textLen) {
            this->free();
            this->textLen = textLen;
            unsigned int extendedTextLen = this->textLen;
            if (this->textLen % BLOCK_LEN > 0) extendedTextLen += (BLOCK_LEN - this->textLen % BLOCK_LEN);

            bool *emptyBlock = new bool[extendedTextLen / BLOCK_LEN];
            bool homoblock;
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                homoblock = true;
                for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                    if (i + j >= this->textLen || (unsigned int)text[i + j] > 0) {
                        homoblock = false;
                        break;
                    }
                }
                if (!homoblock) {
                    homoblock = true;
                    for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                        if (i + j >= this->textLen || (unsigned int)text[i + j] < 255) {
                            homoblock = false;
                            break;
                        }
                    }
                }
                emptyBlock[i / BLOCK_LEN] = homoblock;
            }

            unsigned int notEmptyBlocks = 0;
            for (unsigned int i = 0; i < extendedTextLen / BLOCK_LEN; ++i) if (!emptyBlock[i]) ++notEmptyBlocks;

            unsigned int ranksLen = 2 * (extendedTextLen / BLOCK_LEN) + 1;
            unsigned int *ranks = new unsigned int[ranksLen];

            unsigned int bitsTempLen = notEmptyBlocks * (BLOCK_LEN / sizeof(unsigned int));
            unsigned int *bitsTemp = new unsigned int[bitsTempLen];

            unsigned int rank = 0, rankCounter = 0, bitsCounter = 0;
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                ranks[rankCounter++] = rank;
                ranks[rankCounter++] = bitsCounter;
                if (emptyBlock[i / BLOCK_LEN]) {
                    if ((unsigned int)text[i] == 255) rank += BLOCK_IN_BITS;
                    continue;
                }
                for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                    if (j % sizeof(unsigned int) == 0) bitsTemp[bitsCounter] = 0;
                    if (i + j < this->textLen) bitsTemp[bitsCounter] += (((unsigned int)text[i + j]) << (8 * (sizeof(unsigned int) - j % sizeof(unsigned int) - 1)));
                    if (j % sizeof(unsigned int) == 3) rank += __builtin_popcount(bitsTemp[bitsCounter++]);
                }
            }
            ranks[rankCounter++] = rank;

            unsigned int curEmptyBlocksL = 0;
            unsigned int curNotEmptyBlocksR = notEmptyBlocks;

            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                if (emptyBlock[i / BLOCK_LEN]) ++curEmptyBlocksL;
                else --curNotEmptyBlocksR;
                if (curEmptyBlocksL >= curNotEmptyBlocksR) {
                    this->border = i / BLOCK_LEN + 1;
                    this->blockBorder = this->border * INT_BLOCK_LEN_WITH_RANK;
                    break;
                }
            }

            this->bitsLen = (this->border - 1) * ((BLOCK_LEN / sizeof(unsigned int)) + 1) + (extendedTextLen / BLOCK_LEN - (this->border - 1)) * 2 + 16;
            this->bits = new unsigned int[this->bitsLen + 32];
            this->alignedBits = this->bits;
            while ((unsigned long long)this->alignedBits % 128) ++this->alignedBits;

            unsigned int LCounter = 0;
            unsigned int RCounter = this->border;
            rankCounter = 0;
            unsigned int offsetCounter = 2 * this->border + 1;

            for (unsigned int i = 0; i < (this->border * BLOCK_LEN); i += BLOCK_LEN) {
                this->alignedBits[LCounter++] = ranks[rankCounter];
                if (emptyBlock[i / BLOCK_LEN]) {
                    while(true) {
                        if (!emptyBlock[RCounter]) break;
                        ++RCounter;
                        offsetCounter += 2;
                    }
                    unsigned int offset = ranks[offsetCounter];
                    ranks[offsetCounter] = LCounter;
                    for (unsigned int j = 0; j < (BLOCK_LEN / sizeof(unsigned int)); ++j) {
                        this->alignedBits[LCounter++] = bitsTemp[offset + j];
                    }
                    ++RCounter;
                    offsetCounter += 2;
                } else {
                    for (unsigned int j = 0; j < (BLOCK_LEN / sizeof(unsigned int)); ++j) {
                        this->alignedBits[LCounter++] = bitsTemp[ranks[rankCounter + 1] + j];
                    }
                }
                rankCounter += 2;
            }
            for (unsigned int i = (2 * this->border); i < ranksLen; ++i) this->alignedBits[LCounter++] = ranks[i];

            delete[] ranks;
            delete[] bitsTemp;
            delete[] emptyBlock;
        }
        
        unsigned int rank(unsigned int i) {
            unsigned int start = i / BLOCK_IN_BITS;
            unsigned int rank;
            if (start < this->border) {
                unsigned int LStart = start * INT_BLOCK_LEN + start; 
                this->pointer = this->alignedBits + LStart;
                rank = *(this->pointer);
                i %= BLOCK_IN_BITS;
                unsigned int rankDiff = *(this->pointer + INT_BLOCK_LEN_WITH_RANK) - rank;
                if ((rankDiff & BLOCK_MASK) == 0) {
                  return rank + i * (rankDiff >> 9);
                }
                ++this->pointer;
            } else {
                unsigned int RStart = this->blockBorder + 2 * (start - this->border);
                rank = this->alignedBits[RStart];
                i %= BLOCK_IN_BITS;
                unsigned int rankDiff = this->alignedBits[RStart + 2] - rank;
                if ((rankDiff & BLOCK_MASK) == 0) {
                  return rank + i * (rankDiff >> 9);
                }
                this->pointer = this->alignedBits + this->alignedBits[RStart + 1];
            }
            switch (i / 64) {
            case 7:
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 2)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 4)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 6)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 10)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 12)));
              return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 14))) & masks[i - 448]);
            case 6:
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 2)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 4)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 6)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 10)));
              return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 12))) & masks[i - 384]);
            case 5:
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 2)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 4)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 6)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
              return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 10))) & masks[i - 320]);
            case 4:
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 2)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 4)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 6)));
              return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 8))) & masks[i - 256]);
            case 3:
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 2)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 4)));
              return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 6))) & masks[i - 192]);
            case 2:
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 2)));
              return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 4))) & masks[i - 128]);
            case 1:
              rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
              return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 2))) & masks[i - 64]);
            default:
              return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer))) & masks[i]);
            }
        } 
           
	void save(FILE *outFile) {
            fwrite(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(&this->border, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(&this->blockBorder, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            if (this->bitsLen > 0) fwrite(this->alignedBits, (size_t)sizeof(unsigned int), (size_t)this->bitsLen, outFile);
        }
	
        void load(FILE *inFile) {
            this->free();
            size_t result;
            result = fread(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            result = fread(&this->border, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            result = fread(&this->blockBorder, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            result = fread(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            if (this->bitsLen > 0) {
                    this->bits = new unsigned int[this->bitsLen + 32];
                    this->alignedBits = this->bits;
                    while ((unsigned long long)(this->alignedBits) % 128) ++(this->alignedBits);
                    result = fread(this->alignedBits, (size_t)sizeof(unsigned int), (size_t)this->bitsLen, inFile);
                    if (result != this->bitsLen) {
                            cout << "Error loading rank" << endl;
                            exit(1);
                    }
            }
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
	
        unsigned int getSize() {
            unsigned int size = sizeof(this->border) + sizeof(this->blockBorder) + sizeof(this->bitsLen) + sizeof(this->textLen) + sizeof(this->pointer);
            if (this->bitsLen > 0) size += (this->bitsLen + 32) * sizeof(unsigned int);
            return size;
        }
        
        unsigned int getTextSize() {
            return this->textLen * sizeof(unsigned char);
        }
        
        void testRank(unsigned char* text, unsigned int textLen) {
            unsigned int rankTest = 0;

            for (unsigned int i = 0; i < textLen; ++i) {
                unsigned int bits = (unsigned int)text[i];
                for (unsigned int j = 0; j < 8; ++j) {
                    if (((bits >> (7 - j)) & 1) == 1) ++rankTest;
                    if (rankTest != this->rank(8 * i + j + 1)) {
                        cout << (8 * i + j + 1) << " " << rankTest << " " << this->rank(8 * i + j + 1);
                        cin.ignore();
                    }
                }
            }
        }
};

enum RankMPEType {
        V1 = 1,
        V2 = 2,
        V3 = 3
};

template<RankMPEType T> class RankMPE {
private:
	void freeMemory() {
            if (this->ranks != NULL) delete[] this->ranks;
            if (this->bits != NULL) delete[] this->bits;
        }
        
	void initialize() {
            this->ranks = NULL;
            this->alignedRanks = NULL;
            this->ranksLen = 0;
            this->bits = NULL;
            this->alignedBits = NULL;
            this->bitsLen = 0;
            this->textLen = 0;
            this->pointer = NULL;
            this->pointer2 = NULL;
        }
        
        void build_v1(unsigned char* text, unsigned int textLen) {
            this->free();
            this->textLen = textLen;
            unsigned int extendedTextLen = this->textLen;
            if (this->textLen % BLOCK_LEN > 0) extendedTextLen += (BLOCK_LEN - this->textLen % BLOCK_LEN);

            bool *emptyBlock = new bool[extendedTextLen / BLOCK_LEN];
            bool homoblock;
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                homoblock = true;
                for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                    if (i + j >= this->textLen || (unsigned int)text[i + j] > 0) {
                        homoblock = false;
                        break;
                    }
                }
                if (!homoblock) {
                    homoblock = true;
                    for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                        if (i + j >= this->textLen || (unsigned int)text[i + j] < 255) {
                            homoblock = false;
                            break;
                        }
                    }
                }
                emptyBlock[i / BLOCK_LEN] = homoblock;
            }

            unsigned int notEmptyBlocks = 0;
            for (unsigned int i = 0; i < extendedTextLen / BLOCK_LEN; ++i) if (!emptyBlock[i]) ++notEmptyBlocks;

            unsigned int bitsTempLen = notEmptyBlocks * BLOCK_LEN;
            unsigned char *bitsTemp = new unsigned char[bitsTempLen];

            unsigned int ranksTempLen = 2 * (extendedTextLen / BLOCK_LEN) + 2;
            if (ranksTempLen % (2 * SUPERBLOCKLEN) > 0) ranksTempLen += ((2 * SUPERBLOCKLEN) - ranksTempLen % (2 * SUPERBLOCKLEN));
            unsigned int *ranksTemp = new unsigned int[ranksTempLen];

            unsigned int rank = 0, rankCounter = 0, bitsCounter = 0;
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                ranksTemp[rankCounter++] = rank;
                ranksTemp[rankCounter++] = bitsCounter;
                if (emptyBlock[i / BLOCK_LEN]) {
                    if ((unsigned int)text[i] == 255) rank += BLOCK_IN_BITS;
                    continue;
                }
                for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                    if (i + j < this->textLen) {
                        bitsTemp[bitsCounter++] = (unsigned int)text[i + j];
                        rank += __builtin_popcount((unsigned int)text[i + j]);
                    } else {
                        bitsTemp[bitsCounter++] = 0;
                    }
                }
            }
            ranksTemp[rankCounter++] = rank;
            ranksTemp[rankCounter++] = 0;

            for (unsigned int i = rankCounter; i < ranksTempLen; ++i) {
                ranksTemp[i] = 0;
            }

            unsigned int pairsToRemove = 0;
            unsigned int pairsToRemoveInBlock = 0;
            unsigned int compressedBlocks = 0;
            unsigned int offset = 0;
            bool *blockToCompress = new bool[extendedTextLen / BLOCK_LEN];
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                blockToCompress[i / BLOCK_LEN] = false;
                if (emptyBlock[i / BLOCK_LEN]) continue;
                pairsToRemoveInBlock = 0;
                for (unsigned int j = 0; j < BLOCK_LEN; j += 2) {
                    if ((bitsTemp[offset + j] == 0 && bitsTemp[offset + j + 1] == 0) || (bitsTemp[offset + j] == 255 && bitsTemp[offset + j + 1] == 255)) ++pairsToRemoveInBlock;
                }
                if (pairsToRemoveInBlock > 4) {
                    pairsToRemove += pairsToRemoveInBlock;
                    ++compressedBlocks;
                    blockToCompress[i / BLOCK_LEN] = true;
                }
                offset += BLOCK_LEN;
            }

            this->bitsLen = compressedBlocks * (((BLOCK_LEN / 2) * 2) / 8) + (bitsTempLen - 2 * pairsToRemove) + sizeof(unsigned long long);

            this->bits = new unsigned char[this->bitsLen + 128];
            this->alignedBits = this->bits;
            while ((unsigned long long)this->alignedBits % 128) ++this->alignedBits;

            unsigned int bits1, bits2;
            bitsCounter = 0;
            rankCounter = 1;
            offset = 0;

            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                ranksTemp[rankCounter] = bitsCounter;
                rankCounter += 2;
                if (emptyBlock[i / BLOCK_LEN]) continue;
                if (blockToCompress[i / BLOCK_LEN]) {
                    bits1 = 0;
                    bits2 = 0;
                    bitsCounter += 8;
                    for (unsigned int j = 0; j < BLOCK_LEN; j += 2) {
                        if (bitsTemp[offset + j] == 255 && bitsTemp[offset + j + 1] == 255) {
                            bits2 += (1 << (31 - (j / 2)));
                        }
                        if ((bitsTemp[offset + j] == 0 && bitsTemp[offset + j + 1] == 0) || (bitsTemp[offset + j] == 255 && bitsTemp[offset + j + 1] == 255)) {
                            bits1 += (1 << (31 - (j / 2)));
                        } else {
                            this->alignedBits[bitsCounter++] = bitsTemp[offset + j];
                            this->alignedBits[bitsCounter++] = bitsTemp[offset + j + 1];
                        }
                    }
                    this->alignedBits[ranksTemp[rankCounter - 2]] = (bits1 & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 1] = ((bits1 >> 8) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 2] = ((bits1 >> 16) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 3] = (bits1 >> 24);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 4] = (bits2 & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 5] = ((bits2 >> 8) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 6] = ((bits2 >> 16) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 7] = (bits2 >> 24);
                } else {
                    for (unsigned int j = 0; j < BLOCK_LEN; ++j) this->alignedBits[bitsCounter++] = bitsTemp[offset + j];
                }
                offset += BLOCK_LEN;
            }
            for (unsigned int i = rankCounter; i < ranksTempLen; i += 2) ranksTemp[i] = ranksTemp[rankCounter - 2];

            delete[] bitsTemp;

            this->ranksLen = (ranksTempLen / (2 * SUPERBLOCKLEN)) * INTSINSUPERBLOCK;
            ranksTempLen /= 2;

            this->ranks = new unsigned int[this->ranksLen + 32];
            this->alignedRanks = this->ranks;
            while ((unsigned long long)this->alignedRanks % 128) ++this->alignedRanks;

            unsigned int bucket[INTSINSUPERBLOCK];
            rankCounter = 0;
            for (unsigned int j = 0; j < INTSINSUPERBLOCK; ++j) bucket[j] = 0;

            for (unsigned int i = 0; i < ranksTempLen; i += SUPERBLOCKLEN) {
                bucket[0] = ranksTemp[2 * i];
                for (unsigned int j = 0; j < SUPERBLOCKLEN - 1; ++j) {
                    bucket[j / 2 + 1] += ((ranksTemp[2 * (i + j + 1)] - bucket[0]) << (16 * (j % 2)));
                }
                bucket[OFFSETSTARTINSUPERBLOCK] = ranksTemp[2 * i + 1];
                for (unsigned int j = 0; j < SUPERBLOCKLEN - 1; ++j) {
                    unsigned int offsetDiff = (ranksTemp[2 * (i + j + 1) + 1] - bucket[OFFSETSTARTINSUPERBLOCK]);
                    if (blockToCompress[i + j + 1]) offsetDiff += (1 << 15);
                    bucket[OFFSETSTARTINSUPERBLOCK + 1 + j / 2] += (offsetDiff << (16 * (j % 2)));
                }
                if (blockToCompress[i]) bucket[OFFSETSTARTINSUPERBLOCK + 1] += (1 << 30);
                for (unsigned int j = 0; j < INTSINSUPERBLOCK; ++j) {
                    this->alignedRanks[rankCounter++] = bucket[j];
                    bucket[j] = 0;
                }
            }

            delete[] blockToCompress;
            delete[] emptyBlock;
            delete[] ranksTemp;
        }
        
        void build_v2(unsigned char* text, unsigned int textLen) {
            this->free();
            this->textLen = textLen;
            unsigned int extendedTextLen = this->textLen;
            if (this->textLen % BLOCK_LEN > 0) extendedTextLen += (BLOCK_LEN - this->textLen % BLOCK_LEN);

            bool *emptyBlock = new bool[extendedTextLen / BLOCK_LEN];
            bool homoblock;
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                homoblock = true;
                for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                    if (i + j >= this->textLen || (unsigned int)text[i + j] > 0) {
                        homoblock = false;
                        break;
                    }
                }
                if (!homoblock) {
                    homoblock = true;
                    for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                        if (i + j >= this->textLen || (unsigned int)text[i + j] < 255) {
                            homoblock = false;
                            break;
                        }
                    }
                }
                emptyBlock[i / BLOCK_LEN] = homoblock;
            }

            unsigned int notEmptyBlocks = 0;
            for (unsigned int i = 0; i < extendedTextLen / BLOCK_LEN; ++i) if (!emptyBlock[i]) ++notEmptyBlocks;

            unsigned int bitsTempLen = notEmptyBlocks * BLOCK_LEN;
            unsigned char *bitsTemp = new unsigned char[bitsTempLen];

            unsigned int ranksTempLen = 2 * (extendedTextLen / BLOCK_LEN) + 2;
            if (ranksTempLen % (2 * SUPERBLOCKLEN) > 0) ranksTempLen += ((2 * SUPERBLOCKLEN) - ranksTempLen % (2 * SUPERBLOCKLEN));
            unsigned int *ranksTemp = new unsigned int[ranksTempLen];

            unsigned int rank = 0, rankCounter = 0, bitsCounter = 0;
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                ranksTemp[rankCounter++] = rank;
                ranksTemp[rankCounter++] = bitsCounter;
                if (emptyBlock[i / BLOCK_LEN]) {
                    if ((unsigned int)text[i] == 255) rank += BLOCK_IN_BITS;
                    continue;
                }
                for (unsigned int j = 0; j < BLOCK_LEN; ++j) {
                    if (i + j < this->textLen) {
                        bitsTemp[bitsCounter++] = (unsigned int)text[i + j];
                        rank += __builtin_popcount((unsigned int)text[i + j]);
                    } else {
                        bitsTemp[bitsCounter++] = 0;
                    }
                }
            }
            ranksTemp[rankCounter++] = rank;
            ranksTemp[rankCounter++] = 0;

            for (unsigned int i = rankCounter; i < ranksTempLen; ++i) {
                ranksTemp[i] = 0;
            }

            unsigned int pairsToRemove = 0;
            unsigned int pairs0ToRemoveInBlock = 0;
            unsigned int pairs255ToRemoveInBlock = 0;
            unsigned int compressedBlocks = 0;
            unsigned int compressed0Blocks = 0;
            unsigned int compressed255Blocks = 0;
            unsigned int offset = 0;
            unsigned char *blockToCompress = new unsigned char[extendedTextLen / BLOCK_LEN];
            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                blockToCompress[i / BLOCK_LEN] = 0;
                if (emptyBlock[i / BLOCK_LEN]) continue;
                pairs0ToRemoveInBlock = 0;
                pairs255ToRemoveInBlock = 0;
                for (unsigned int j = 0; j < BLOCK_LEN; j += 2) {
                    if ((bitsTemp[offset + j] == 0 && bitsTemp[offset + j + 1] == 0)) ++pairs0ToRemoveInBlock;
                    if ((bitsTemp[offset + j] == 255 && bitsTemp[offset + j + 1] == 255)) ++pairs255ToRemoveInBlock;
                }
                switch(T) {
                    case RankMPEType::V2:
                        if (pairs0ToRemoveInBlock > 2 && pairs255ToRemoveInBlock > 2) {
                            pairsToRemove += (pairs0ToRemoveInBlock + pairs255ToRemoveInBlock);
                            ++compressedBlocks;
                            blockToCompress[i / BLOCK_LEN] = 3;
                        } else if (pairs0ToRemoveInBlock > 2 && pairs255ToRemoveInBlock <= 2) {
                            pairsToRemove += pairs0ToRemoveInBlock;
                            ++compressed0Blocks;
                            blockToCompress[i / BLOCK_LEN] = 1;
                        } else if (pairs0ToRemoveInBlock <= 2 && pairs255ToRemoveInBlock > 2) {
                            pairsToRemove += pairs255ToRemoveInBlock;
                            ++compressed255Blocks;
                            blockToCompress[i / BLOCK_LEN] = 2;
                        }
                        break;
                    default:
                        if (pairs0ToRemoveInBlock > PAIRS0THR && pairs255ToRemoveInBlock > PAIRS255THR) {
                            pairsToRemove += (pairs0ToRemoveInBlock + pairs255ToRemoveInBlock);
                            ++compressedBlocks;
                            blockToCompress[i / BLOCK_LEN] = 3;
                        } else if (pairs0ToRemoveInBlock >= PAIRS0THR * 2 && pairs255ToRemoveInBlock <= PAIRS255THR) {
                            pairsToRemove += pairs0ToRemoveInBlock;
                            ++compressed0Blocks;
                            blockToCompress[i / BLOCK_LEN] = 1;
                        } else if (pairs0ToRemoveInBlock <= PAIRS0THR && pairs255ToRemoveInBlock >= PAIRS255THR * 2) {
                            pairsToRemove += pairs255ToRemoveInBlock;
                            ++compressed255Blocks;
                            blockToCompress[i / BLOCK_LEN] = 2;
                        }
                        break;
                }
                offset += BLOCK_LEN;
            }

            this->bitsLen = compressedBlocks * (((BLOCK_LEN / 2) * 2) / 8) + compressed0Blocks * ((BLOCK_LEN / 2) / 8) + compressed255Blocks * ((BLOCK_LEN / 2) / 8) + (bitsTempLen - 2 * pairsToRemove) + sizeof(unsigned long long);

            this->bits = new unsigned char[this->bitsLen + 128];
            this->alignedBits = this->bits;
            while ((unsigned long long)this->alignedBits % 128) ++this->alignedBits;

            unsigned int bits1, bits2, bits;
            bitsCounter = 0;
            rankCounter = 1;
            offset = 0;

            for (unsigned int i = 0; i < extendedTextLen; i += BLOCK_LEN) {
                ranksTemp[rankCounter] = bitsCounter;
                rankCounter += 2;
                if (emptyBlock[i / BLOCK_LEN]) continue;
                if (blockToCompress[i / BLOCK_LEN] == 3) {
                    bits1 = 0;
                    bits2 = 0;
                    bitsCounter += 8;
                    for (unsigned int j = 0; j < BLOCK_LEN; j += 2) {
                        if (bitsTemp[offset + j] == 255 && bitsTemp[offset + j + 1] == 255) {
                            bits2 += (1 << (31 - (j / 2)));
                        }
                        if ((bitsTemp[offset + j] == 0 && bitsTemp[offset + j + 1] == 0) || (bitsTemp[offset + j] == 255 && bitsTemp[offset + j + 1] == 255)) {
                            bits1 += (1 << (31 - (j / 2)));
                        } else {
                            this->alignedBits[bitsCounter++] = bitsTemp[offset + j];
                            this->alignedBits[bitsCounter++] = bitsTemp[offset + j + 1];
                        }
                    }
                    this->alignedBits[ranksTemp[rankCounter - 2]] = (bits1 & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 1] = ((bits1 >> 8) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 2] = ((bits1 >> 16) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 3] = (bits1 >> 24);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 4] = (bits2 & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 5] = ((bits2 >> 8) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 6] = ((bits2 >> 16) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 7] = (bits2 >> 24);
                } else if (blockToCompress[i / BLOCK_LEN] == 1) {
                    bits = 0;
                    bitsCounter += 4;
                    for (unsigned int j = 0; j < BLOCK_LEN; j += 2) {
                        if (bitsTemp[offset + j] == 0 && bitsTemp[offset + j + 1] == 0) {
                            bits += (1 << (31 - (j / 2)));
                        } else {
                            this->alignedBits[bitsCounter++] = bitsTemp[offset + j];
                            this->alignedBits[bitsCounter++] = bitsTemp[offset + j + 1];
                        }
                    }
                    this->alignedBits[ranksTemp[rankCounter - 2]] = (bits & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 1] = ((bits >> 8) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 2] = ((bits >> 16) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 3] = (bits >> 24);
                } else if (blockToCompress[i / BLOCK_LEN] == 2) {
                    bits = 0;
                    bitsCounter += 4;
                    for (unsigned int j = 0; j < BLOCK_LEN; j += 2) {
                        if (bitsTemp[offset + j] == 255 && bitsTemp[offset + j + 1] == 255) {
                            bits += (1 << (31 - (j / 2)));
                        } else {
                            this->alignedBits[bitsCounter++] = bitsTemp[offset + j];
                            this->alignedBits[bitsCounter++] = bitsTemp[offset + j + 1];
                        }
                    }
                    this->alignedBits[ranksTemp[rankCounter - 2]] = (bits & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 1] = ((bits >> 8) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 2] = ((bits >> 16) & 0xFF);
                    this->alignedBits[ranksTemp[rankCounter - 2] + 3] = (bits >> 24);
                } else {
                    for (unsigned int j = 0; j < BLOCK_LEN; ++j) this->alignedBits[bitsCounter++] = bitsTemp[offset + j];
                }
                offset += BLOCK_LEN;
            }
            for (unsigned int i = rankCounter; i < ranksTempLen; i += 2) ranksTemp[i] = ranksTemp[rankCounter - 2];

            delete[] bitsTemp;

            this->ranksLen = (ranksTempLen / (2 * SUPERBLOCKLEN)) * INTSINSUPERBLOCK;
            ranksTempLen /= 2;

            this->ranks = new unsigned int[this->ranksLen + 32];
            this->alignedRanks = this->ranks;
            while ((unsigned long long)this->alignedRanks % 128) ++this->alignedRanks;

            unsigned int bucket[INTSINSUPERBLOCK];
            rankCounter = 0;
            for (unsigned int j = 0; j < INTSINSUPERBLOCK; ++j) bucket[j] = 0;

            for (unsigned int i = 0; i < ranksTempLen; i += SUPERBLOCKLEN) {
                bucket[0] = ranksTemp[2 * i];
                for (unsigned int j = 0; j < SUPERBLOCKLEN - 1; ++j) {
                    bucket[j / 2 + 1] += ((ranksTemp[2 * (i + j + 1)] - bucket[0]) << (16 * (j % 2)));
                }
                bucket[OFFSETSTARTINSUPERBLOCK] = ranksTemp[2 * i + 1];
                for (unsigned int j = 0; j < SUPERBLOCKLEN - 1; ++j) {
                    unsigned int offsetDiff = (ranksTemp[2 * (i + j + 1) + 1] - bucket[OFFSETSTARTINSUPERBLOCK]);
                    offsetDiff += (blockToCompress[i + j + 1] << 14);
                    bucket[OFFSETSTARTINSUPERBLOCK + 1 + j / 2] += (offsetDiff << (16 * (j % 2)));
                }
                bucket[OFFSETSTARTINSUPERBLOCK + 1] += (blockToCompress[i] << 28);
                for (unsigned int j = 0; j < INTSINSUPERBLOCK; ++j) {
                    this->alignedRanks[rankCounter++] = bucket[j];
                    bucket[j] = 0;
                }
            }

            delete[] blockToCompress;
            delete[] emptyBlock;
            delete[] ranksTemp;
        }
        
        unsigned int getRank_v1(unsigned int i) {
            unsigned int start = i / BLOCK_IN_BITS;
            this->pointer2 = this->alignedRanks + INTSINSUPERBLOCK * (start / SUPERBLOCKLEN);
            unsigned int rank = *(this->pointer2);
            unsigned int rankNumInCL = (start % SUPERBLOCKLEN);
            if (rankNumInCL > 0) {
                if (rankNumInCL % 2 == 1) rank += (this->pointer2[(rankNumInCL + 1) / 2] & 0xFFFF);
                else rank += (this->pointer2[(rankNumInCL + 1) / 2] >> 16);
            }
            i %= BLOCK_IN_BITS;
            unsigned int rank2;
            if (rankNumInCL == (SUPERBLOCKLEN - 1)) rank2 = this->pointer2[INTSINSUPERBLOCK];
            else {
                rank2 = *(this->pointer2);
                if (rankNumInCL % 2 == 0) rank2 += (this->pointer2[(rankNumInCL + 2) / 2] & 0xFFFF);
                else rank2 += (this->pointer2[(rankNumInCL + 2) / 2] >> 16);
            }
            if (((rank2 - rank) & BLOCK_MASK) == 0) return rank + i * ((rank2 - rank) >> 9);
            bool compressedBlock;
            if (rankNumInCL > 0) {
                unsigned int offset = this->pointer2[OFFSETSTARTINSUPERBLOCK + (rankNumInCL + 1) / 2];
                if (rankNumInCL % 2 == 1) {
                    this->pointer = this->alignedBits + this->pointer2[OFFSETSTARTINSUPERBLOCK] + (offset & 0x3FFF);
                    compressedBlock = (offset >> 15) & 1;
                } else {
                    this->pointer = this->alignedBits + this->pointer2[OFFSETSTARTINSUPERBLOCK] + ((offset >> 16) & 0x3FFF);
                    compressedBlock = (offset >> 31);
                }
            }
            else {
                compressedBlock = ((this->pointer2[OFFSETSTARTINSUPERBLOCK + 1] >> 30) & 1);
                this->pointer = this->alignedBits + this->pointer2[OFFSETSTARTINSUPERBLOCK];
            }
            if (compressedBlock) {
                unsigned int temp1 = i % 16;
                unsigned int temp2 = i / 16;
                unsigned int bits1 = *((unsigned int*)(this->pointer));
                unsigned int bits2 = *((unsigned int*)(this->pointer + 4));
                if ((bits1 & masks2[temp2]) > 0) {
                    if ((bits2 & masks2[temp2]) > 0) rank += temp1;
                    i -= temp1;
                }
                i -= 16 * __builtin_popcount(bits1 & masks3[temp2]);
                rank += 16 * __builtin_popcount(bits2 & masks3[temp2]);
                (this->pointer) += 8;
            }
            switch (i / 64) {
            case 7:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 24)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 32)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 40)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 48)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 56))) & masks[i - 448]);
            case 6:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 24)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 32)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 40)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 48))) & masks[i - 384]);
            case 5:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 24)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 32)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 40))) & masks[i - 320]);
            case 4:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 24)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 32))) & masks[i - 256]);
            case 3:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 24))) & masks[i - 192]);
            case 2:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 16))) & masks[i - 128]);
            case 1:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 8))) & masks[i - 64]);
            default:
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer))) & masks[i]);
            }
        }

        unsigned int getRank_v2(unsigned int i) {
            unsigned int start = i / BLOCK_IN_BITS;
            this->pointer2 = this->alignedRanks + INTSINSUPERBLOCK * (start / SUPERBLOCKLEN);
            unsigned int rank = *(this->pointer2);
            unsigned int rankNumInCL = (start % SUPERBLOCKLEN);
            if (rankNumInCL > 0) {
                if (rankNumInCL % 2 == 1) rank += (this->pointer2[(rankNumInCL + 1) / 2] & 0xFFFF);
                else rank += (this->pointer2[(rankNumInCL + 1) / 2] >> 16);
            }
            i %= BLOCK_IN_BITS;
            unsigned int rank2;
            if (rankNumInCL == (SUPERBLOCKLEN - 1)) rank2 = this->pointer2[INTSINSUPERBLOCK];
            else {
                rank2 = *(this->pointer2);
                if (rankNumInCL % 2 == 0) rank2 += (this->pointer2[(rankNumInCL + 2) / 2] & 0xFFFF);
                else rank2 += (this->pointer2[(rankNumInCL + 2) / 2] >> 16);
            }
            if (((rank2 - rank) & BLOCK_MASK) == 0) return rank + i * ((rank2 - rank) >> 9);
            unsigned int compressedBlock;
            if (rankNumInCL > 0) {
                unsigned int offset = this->pointer2[OFFSETSTARTINSUPERBLOCK + (rankNumInCL + 1) / 2];
                if (rankNumInCL % 2 == 1) {
                    this->pointer = this->alignedBits + this->pointer2[OFFSETSTARTINSUPERBLOCK] + (offset & 0x0FFF);
                    compressedBlock = (offset >> 14) & 3;
                } else {
                    this->pointer = this->alignedBits + this->pointer2[OFFSETSTARTINSUPERBLOCK] + ((offset >> 16) & 0x0FFF);
                    compressedBlock = (offset >> 30);
                }
            }
            else {
                compressedBlock = ((this->pointer2[OFFSETSTARTINSUPERBLOCK + 1] >> 28) & 3);
                this->pointer = this->alignedBits + this->pointer2[OFFSETSTARTINSUPERBLOCK];
            }
            unsigned int temp1, temp2, bits, tempBits;
            switch (compressedBlock) {
                case 1:
                    temp1 = i % 16;
                    temp2 = i / 16;
                    bits = *((unsigned int*)(this->pointer));
                    if ((bits & masks2[temp2])> 0) i -= temp1;
                    i -= 16 * __builtin_popcount(bits & masks3[temp2]);
                    (this->pointer) += 4;
                    break;
                case 2:
                    temp1 = i % 16;
                    temp2 = i / 16;
                    bits = *((unsigned int*)(this->pointer));
                    if ((bits & masks2[temp2])> 0) {
                        rank += temp1;
                        i -= temp1;
                    }
                    tempBits = 16 * __builtin_popcount(bits & masks3[temp2]);
                    i -= tempBits;
                    rank += tempBits;
                    (this->pointer) += 4;
                    break;
                case 3:
                    temp1 = i % 16;
                    temp2 = i / 16;
                    unsigned int bits1 = *((unsigned int*)(this->pointer));
                    unsigned int bits2 = *((unsigned int*)(this->pointer + 4));
                    if ((bits1 & masks2[temp2])> 0) {
                        if ((bits2 & masks2[temp2])> 0) rank += temp1;
                        i -= temp1;
                    }
                    i -= 16 * __builtin_popcount(bits1 & masks3[temp2]);
                    rank += 16 * __builtin_popcount(bits2 & masks3[temp2]);
                    (this->pointer) += 8;
                    break;
            }
            switch (i / 64) {
            case 7:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 24)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 32)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 40)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 48)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 56))) & masks[i - 448]);
            case 6:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 24)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 32)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 40)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 48))) & masks[i - 384]);
            case 5:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 24)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 32)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 40))) & masks[i - 320]);
            case 4:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 24)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 32))) & masks[i - 256]);
            case 3:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 16)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 24))) & masks[i - 192]);
            case 2:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer + 8)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 16))) & masks[i - 128]);
            case 1:
                rank += __builtin_popcountll(*((unsigned long long*)(this->pointer)));
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer + 8))) & masks[i - 64]);
            default:
                return rank + __builtin_popcountll((*((unsigned long long*)(this->pointer))) & masks[i]);
            }
        }

public:
	unsigned int *ranks;
        unsigned int *alignedRanks;
        unsigned int ranksLen;
        unsigned char *bits;
        unsigned char *alignedBits;
        unsigned int bitsLen;
        unsigned int textLen;
        unsigned char *pointer;
        unsigned int* pointer2;
        
        const static unsigned int BLOCK_LEN = sizeof(unsigned long long) * 8;
        const static unsigned int BLOCK_IN_BITS = 8 * BLOCK_LEN;
        const static unsigned int PAIRS0THR = 9;
        const static unsigned int PAIRS255THR = 9;

        const static unsigned int BLOCK_MASK = (1 << 9) - 1;

        const static unsigned int SUPERBLOCKLEN = 16;
        const static unsigned int OFFSETSTARTINSUPERBLOCK = 9;
        const static unsigned int INTSINSUPERBLOCK = 18;
        /*
        INTSINSUPERBLOCK = 2 + (SUPERBLOCKLEN - 1) / 2 + (SUPERBLOCKLEN - 1) / 2;
        if ((SUPERBLOCKLEN - 1) % 2 == 1) INTSINSUPERBLOCK += 2;
        if ((SUPERBLOCKLEN - 1) % 32 > 0) ++INTSINSUPERBLOCK;
        * 
        OFFSETSTARTINSUPERBLOCK = 1 + (SUPERBLOCKLEN - 1) / 2;
        if ((SUPERBLOCKLEN - 1) % 2 == 1) ++OFFSETSTARTINSUPERBLOCK;
        */

        const unsigned long long masks[64] = {
            0x0000000000000000ULL,
            0x0000000000000080ULL,
            0x00000000000000C0ULL,
            0x00000000000000E0ULL,
            0x00000000000000F0ULL,
            0x00000000000000F8ULL,
            0x00000000000000FCULL,
            0x00000000000000FEULL,
            0x00000000000000FFULL,
            0x00000000000080FFULL,
            0x000000000000C0FFULL,
            0x000000000000E0FFULL,
            0x000000000000F0FFULL,
            0x000000000000F8FFULL,
            0x000000000000FCFFULL,
            0x000000000000FEFFULL,
            0x000000000000FFFFULL,
            0x000000000080FFFFULL,
            0x0000000000C0FFFFULL,
            0x0000000000E0FFFFULL,
            0x0000000000F0FFFFULL,
            0x0000000000F8FFFFULL,
            0x0000000000FCFFFFULL,
            0x0000000000FEFFFFULL,
            0x0000000000FFFFFFULL,
            0x0000000080FFFFFFULL,
            0x00000000C0FFFFFFULL,
            0x00000000E0FFFFFFULL,
            0x00000000F0FFFFFFULL,
            0x00000000F8FFFFFFULL,
            0x00000000FCFFFFFFULL,
            0x00000000FEFFFFFFULL,
            0x00000000FFFFFFFFULL,
            0x00000080FFFFFFFFULL,
            0x000000C0FFFFFFFFULL,
            0x000000E0FFFFFFFFULL,
            0x000000F0FFFFFFFFULL,
            0x000000F8FFFFFFFFULL,
            0x000000FCFFFFFFFFULL,
            0x000000FEFFFFFFFFULL,
            0x000000FFFFFFFFFFULL,
            0x000080FFFFFFFFFFULL,
            0x0000C0FFFFFFFFFFULL,
            0x0000E0FFFFFFFFFFULL,
            0x0000F0FFFFFFFFFFULL,
            0x0000F8FFFFFFFFFFULL,
            0x0000FCFFFFFFFFFFULL,
            0x0000FEFFFFFFFFFFULL,
            0x0000FFFFFFFFFFFFULL,
            0x0080FFFFFFFFFFFFULL,
            0x00C0FFFFFFFFFFFFULL,
            0x00E0FFFFFFFFFFFFULL,
            0x00F0FFFFFFFFFFFFULL,
            0x00F8FFFFFFFFFFFFULL,
            0x00FCFFFFFFFFFFFFULL,
            0x00FEFFFFFFFFFFFFULL,
            0x00FFFFFFFFFFFFFFULL,
            0x80FFFFFFFFFFFFFFULL,
            0xC0FFFFFFFFFFFFFFULL,
            0xE0FFFFFFFFFFFFFFULL,
            0xF0FFFFFFFFFFFFFFULL,
            0xF8FFFFFFFFFFFFFFULL,
            0xFCFFFFFFFFFFFFFFULL,
            0xFEFFFFFFFFFFFFFFULL
        };

        const unsigned int masks2[32] = {
            0x80000000,
            0x40000000,
            0x20000000,
            0x10000000,
            0x08000000,
            0x04000000,
            0x02000000,
            0x01000000,
            0x00800000,
            0x00400000,
            0x00200000,
            0x00100000,
            0x00080000,
            0x00040000,
            0x00020000,
            0x00010000,
            0x00008000,
            0x00004000,
            0x00002000,
            0x00001000,
            0x00000800,
            0x00000400,
            0x00000200,
            0x00000100,
            0x00000080,
            0x00000040,
            0x00000020,
            0x00000010,
            0x00000008,
            0x00000004,
            0x00000002,
            0x00000001
        };

        const unsigned int masks3[32] = {
            0x00000000,
            0x80000000,
            0xC0000000,
            0xE0000000,
            0xF0000000,
            0xF8000000,
            0xFC000000,
            0xFE000000,
            0xFF000000,
            0xFF800000,
            0xFFC00000,
            0xFFE00000,
            0xFFF00000,
            0xFFF80000,
            0xFFFC0000,
            0xFFFE0000,
            0xFFFF0000,
            0xFFFF8000,
            0xFFFFC000,
            0xFFFFE000,
            0xFFFFF000,
            0xFFFFF800,
            0xFFFFFC00,
            0xFFFFFE00,
            0xFFFFFF00,
            0xFFFFFF80,
            0xFFFFFFC0,
            0xFFFFFFE0,
            0xFFFFFFF0,
            0xFFFFFFF8,
            0xFFFFFFFC,
            0xFFFFFFFE
        };

	RankMPE() {
		this->initialize();
	}

	~RankMPE() {
		this->freeMemory();
	}

        void build(unsigned char *text, unsigned int textLen) {
            switch(T) {
                case RankMPEType::V1:
                    this->build_v1(text, textLen);
                    break;
                default:
                    this->build_v2(text, textLen);
                    break;      
            }
        }
        
        unsigned int rank(unsigned int i) {
                switch(T) {
                case RankMPEType::V1:
                        return this->getRank_v1(i);
                        break;
                default:
                        return this->getRank_v2(i);
                        break;
                }
        } 
           
	void save(FILE *outFile) {
            fwrite(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(&this->ranksLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            if (this->ranksLen > 0) fwrite(this->alignedRanks, (size_t)sizeof(unsigned int), (size_t)this->ranksLen, outFile);
            fwrite(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            if (this->bitsLen > 0) fwrite(this->alignedBits, (size_t)sizeof(unsigned char), (size_t)this->bitsLen, outFile);
        }
	
        void load(FILE *inFile) {
            this->free();
            size_t result;
            result = fread(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            result = fread(&this->ranksLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            if (this->ranksLen > 0) {
                    this->ranks = new unsigned int[this->ranksLen + 32];
                    this->alignedRanks = this->ranks;
                    while ((unsigned long long)(this->alignedRanks) % 128) ++(this->alignedRanks);
                    result = fread(this->alignedRanks, (size_t)sizeof(unsigned int), (size_t)this->ranksLen, inFile);
                    if (result != this->ranksLen) {
                            cout << "Error loading rank" << endl;
                            exit(1);
                    }
            }
            result = fread(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading rank" << endl;
                    exit(1);
            }
            if (this->bitsLen > 0) {
                    this->bits = new unsigned char[this->bitsLen + 128];
                    this->alignedBits = this->bits;
                    while ((unsigned long long)(this->alignedBits) % 128) ++(this->alignedBits);
                    result = fread(this->alignedBits, (size_t)sizeof(unsigned char), (size_t)this->bitsLen, inFile);
                    if (result != this->bitsLen) {
                            cout << "Error loading rank" << endl;
                            exit(1);
                    }
            }
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
	
        unsigned int getSize() {
            unsigned int size = sizeof(this->ranksLen) + sizeof(this->bitsLen) + sizeof(this->textLen) + sizeof(this->pointer) + sizeof(this->pointer2);
            if (this->ranksLen > 0) size += (this->ranksLen + 32) * sizeof(unsigned int);
            if (this->bitsLen > 0) size += (this->bitsLen + 128) * sizeof(unsigned char);
            return size;
        }
        
        unsigned int getTextSize() {
            return this->textLen * sizeof(unsigned char);
        }
        
        void testRank(unsigned char* text, unsigned int textLen) {
            unsigned int rankTest = 0;

            for (unsigned int i = 0; i < textLen; ++i) {
                unsigned int bits = (unsigned int)text[i];
                for (unsigned int j = 0; j < 8; ++j) {
                    if (((bits >> (7 - j)) & 1) == 1) ++rankTest;
                    if (rankTest != this->rank(8 * i + j + 1)) {
                        cout << (8 * i + j + 1) << " " << rankTest << " " << this->rank(8 * i + j + 1);
                        cin.ignore();
                    }
                }
            }
        }
};

}

#endif	/* RANK_HPP */

