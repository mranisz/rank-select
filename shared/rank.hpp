#ifndef RANK_HPP
#define	RANK_HPP

#include <iostream>

using namespace std;

namespace fmdummy {
    
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
            //if (__builtin_expect(!!(i == 0), 0)) return rank; 
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
        
        unsigned int getRank(unsigned int i) {
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
	
        unsigned int getRankSize() {
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
                    if (rankTest != this->getRank(8 * i + j + 1)) {
                        cout << (8 * i + j + 1) << " " << rankTest << " " << this->getRank(8 * i + j + 1);
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

            this->bitsLen = (this->border - 1) * ((BLOCK_LEN / sizeof(unsigned int)) + 1) + (extendedTextLen / BLOCK_LEN - (this->border - 1)) * 2 + 1;
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
        
        unsigned int getRank(unsigned int i) {
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
	
        unsigned int getRankSize() {
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
                    if (rankTest != this->getRank(8 * i + j + 1)) {
                        cout << (8 * i + j + 1) << " " << rankTest << " " << this->getRank(8 * i + j + 1);
                        cin.ignore();
                    }
                }
            }
        }
};

}

#endif	/* RANK_HPP */

