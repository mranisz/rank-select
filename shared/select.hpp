#ifndef SELECT_HPP
#define	SELECT_HPP

#include <iostream>

using namespace std;
    
namespace shared {
    
enum SelectBasicType {
        STANDARD = 1,
        WITH_COMPRESSED_HEADERS = 2
};
    
template<SelectBasicType T, unsigned int L, unsigned int THRESHOLD> class SelectBasic {
private:
	void freeMemory() {
            if (this->selects != NULL) delete[] this->selects;
            if (this->bits != NULL) delete[] this->bits;
        }
        
	void initialize() {
            this->selects = NULL;
            this->alignedSelects = NULL;
            this->selectsLen = 0;
            this->bits = NULL;
            this->alignedBits = NULL;
            this->bitsLen = 0;
            this->textLen = 0;
            this->pointer = NULL;
            this->pointer2 = NULL;
        }
        
        void build_std(unsigned char *text, unsigned int textLen) {
            this->free();
            this->textLen = textLen;

            unsigned numberOfOnes = 0;
            for (unsigned int i = 0; i < textLen; ++i) numberOfOnes += __builtin_popcount(text[i]);

            this->selectsLen = 2 * (numberOfOnes / L) + 2;
            if ((numberOfOnes % L) > 0) this->selectsLen += 2;
            this->selects = new unsigned int[this->selectsLen + 32];
            this->alignedSelects = this->selects;
            while ((unsigned long long)this->alignedSelects % 128) ++this->alignedSelects;

            unsigned int select = 0;
            unsigned int selectCounter = 0;
            unsigned int lastSelectPos = 0;
            //unsigned int prevSelectPos = 0;
            unsigned int blockLenInBits;
            this->bitsLen = 0;
            this->alignedSelects[selectCounter++] = 0;
            this->alignedSelects[selectCounter++] = 0;
            for (unsigned int i = 0; i < textLen; ++i) {
                for (unsigned int j = 0; j < 8; ++j) {
                    if (((text[i] >> (7 - j)) & 1) == 1) {
                        ++select;
                        //prevSelectPos = lastSelectPos;
                        lastSelectPos = 8 * i + j;
                        if (select % L == 0) {
                            this->alignedSelects[selectCounter++] = lastSelectPos;
                            this->alignedSelects[selectCounter++] = 0;
                            blockLenInBits = this->alignedSelects[selectCounter - 2] - this->alignedSelects[selectCounter - 4];
                            if (blockLenInBits > THRESHOLD) this->bitsLen += sizeof(unsigned int) * (L - 1);
                            else if (blockLenInBits > L) {
                                //blockLenInBits -= (lastSelectPos - prevSelectPos - 1);
                                if (selectCounter == 4) { //for select[0] = 0
                                    this->bitsLen += (blockLenInBits / 8);
                                    if (blockLenInBits % 8 > 0) ++this->bitsLen;
                                } else {
                                    this->bitsLen += ((blockLenInBits - 1) / 8);
                                    if ((blockLenInBits - 1) % 8 > 0) ++this->bitsLen;
                                }
                            }
                        }
                    }
                }
            }
            if ((numberOfOnes % L) > 0) {
                this->alignedSelects[selectCounter++] = lastSelectPos;
                this->alignedSelects[selectCounter++] = 0;
                blockLenInBits = this->alignedSelects[selectCounter - 2] - this->alignedSelects[selectCounter - 4];
                if (blockLenInBits > THRESHOLD) this->bitsLen += sizeof(unsigned int) * (select % L);
                else {
                    this->bitsLen += (blockLenInBits / 8);
                    if (blockLenInBits % 8 > 0) ++this->bitsLen;
                }
            }

            this->bits = new unsigned char[this->bitsLen + 128];
            this->alignedBits = this->bits;
            while ((unsigned long long)this->alignedBits % 128) ++this->alignedBits;

            selectCounter = 0;
            unsigned int bitsCounter = 0;
            unsigned char bitsToWrite = 0;
            unsigned int bitsToWritePos = 7;
            //unsigned int currOnes = 0;
            unsigned int upToBitPos = this->alignedSelects[2];
            blockLenInBits = this->alignedSelects[2] - this->alignedSelects[0];
            bool sparseBlock = false, monoBlock = false;
            if (blockLenInBits > THRESHOLD) sparseBlock = true;
            else if (blockLenInBits == L) monoBlock = true;
            for (unsigned int i = 0; i < textLen; ++i) {
                for (unsigned int j = 0; j < 8; ++j) {
                    if ((8 * i + j) == upToBitPos) {
                        selectCounter += 2;
                        if (selectCounter == this->selectsLen - 2) {
                            if ((numberOfOnes % L) > 0) {
                                if (sparseBlock && (((text[i] >> (7 - j)) & 1) == 1)) {
                                    unsigned int position = 8 * i + j;
                                    this->alignedBits[bitsCounter++] = position & 0xFF;
                                    this->alignedBits[bitsCounter++] = (position >> 8) & 0xFF;
                                    this->alignedBits[bitsCounter++] = (position >> 16) & 0xFF;
                                    this->alignedBits[bitsCounter++] = (position >> 24);
                                }
                                if (!sparseBlock && !monoBlock) {
                                    if (((text[i] >> (7 - j)) & 1) == 1) {
                                        bitsToWrite += (1 << bitsToWritePos);
                                    }
                                    this->alignedBits[bitsCounter++] = bitsToWrite;
                                }
                            } else if (!monoBlock && !sparseBlock && bitsToWritePos != 7) {
                                this->alignedBits[bitsCounter++] = bitsToWrite;
                            }
                            i = textLen;
                            break;
                        }
                        if (!monoBlock && !sparseBlock && bitsToWritePos != 7) {
                            this->alignedBits[bitsCounter++] = bitsToWrite;
                        }
                        this->alignedSelects[selectCounter + 1] = bitsCounter;
                        upToBitPos = this->alignedSelects[selectCounter + 2];
                        blockLenInBits = this->alignedSelects[selectCounter + 2] - this->alignedSelects[selectCounter];
                        sparseBlock = false;
                        monoBlock = false;
                        if (blockLenInBits > THRESHOLD) sparseBlock = true;
                        else if (blockLenInBits == L) monoBlock = true;
                        bitsToWrite = 0;
                        bitsToWritePos = 7;
                        //currOnes = 0;
                        continue;
                    }
                    if (monoBlock) continue;
                    if (sparseBlock) {
                        if (((text[i] >> (7 - j)) & 1) == 1) {
                            unsigned int position = 8 * i + j;
                            this->alignedBits[bitsCounter++] = position & 0xFF;
                            this->alignedBits[bitsCounter++] = (position >> 8) & 0xFF;
                            this->alignedBits[bitsCounter++] = (position >> 16) & 0xFF;
                            this->alignedBits[bitsCounter++] = (position >> 24);
                        }
                    } else {
                        //if (currOnes >= l - 1) continue;
                        if (((text[i] >> (7 - j)) & 1) == 1) {
                            //++currOnes;
                            bitsToWrite += (1 << bitsToWritePos);
                        }
                        if (bitsToWritePos == 0) {
                            this->alignedBits[bitsCounter++] = bitsToWrite;
                            bitsToWrite = 0;
                            bitsToWritePos = 7;
                        } else {
                            --bitsToWritePos;
                        }
                    }
                }
            }

            for (unsigned int i = 2; i < this->selectsLen; i += 2) ++this->alignedSelects[i];
        }
        
        void build_bch(unsigned char *text, unsigned int textLen) {
            this->free();
            this->textLen = textLen;

            unsigned numberOfOnes = 0;
            for (unsigned int i = 0; i < textLen; ++i) numberOfOnes += __builtin_popcount(text[i]);

            unsigned int selectLenTemp = 2 * (numberOfOnes / L) + 2;
            if ((numberOfOnes % L) > 0) selectLenTemp += 2;
            unsigned int selectLenTempNotExtended = selectLenTemp;
            if (selectLenTemp % (2 * SUPERBLOCKLEN) > 0) selectLenTemp += ((2 * SUPERBLOCKLEN) - selectLenTemp % (2 * SUPERBLOCKLEN));
            unsigned int *selectsTemp = new unsigned int[selectLenTemp];

            unsigned int select = 0;
            unsigned int selectCounter = 0;
            unsigned int lastSelectPos = 0;
            //unsigned int prevSelectPos = 0;
            unsigned int blockLenInBits;
            this->bitsLen = 0;
            selectsTemp[selectCounter++] = 0;
            selectsTemp[selectCounter++] = 0;
            for (unsigned int i = 0; i < textLen; ++i) {
                for (unsigned int j = 0; j < 8; ++j) {
                    if (((text[i] >> (7 - j)) & 1) == 1) {
                        ++select;
                        //prevSelectPos = lastSelectPos;
                        lastSelectPos = 8 * i + j;
                        if (select % L == 0) {
                            selectsTemp[selectCounter++] = lastSelectPos;
                            selectsTemp[selectCounter++] = 0;
                            blockLenInBits = selectsTemp[selectCounter - 2] - selectsTemp[selectCounter - 4];
                            if (blockLenInBits > THRESHOLD) this->bitsLen += sizeof(unsigned int) * (L - 1);
                            else if (blockLenInBits > L) {
                                //blockLenInBits -= (lastSelectPos - prevSelectPos - 1);
                                if (selectCounter == 4) { //for select[0] = 0
                                    this->bitsLen += (blockLenInBits / 8);
                                    if (blockLenInBits % 8 > 0) ++this->bitsLen;
                                } else {
                                    this->bitsLen += ((blockLenInBits - 1) / 8);
                                    if ((blockLenInBits - 1) % 8 > 0) ++this->bitsLen;
                                }
                            }
                        }
                    }
                }
            }
            if ((numberOfOnes % L) > 0) {
                selectsTemp[selectCounter++] = lastSelectPos;
                selectsTemp[selectCounter++] = 0;
                blockLenInBits = selectsTemp[selectCounter - 2] - selectsTemp[selectCounter - 4];
                if (blockLenInBits > THRESHOLD) this->bitsLen += sizeof(unsigned int) * (select % L);
                else {
                    this->bitsLen += (blockLenInBits / 8);
                    if (blockLenInBits % 8 > 0) ++this->bitsLen;
                }
            }
            for (unsigned int i = selectCounter; i < selectLenTemp; ++i) {
                selectsTemp[i] = 0;
            }

            this->bits = new unsigned char[this->bitsLen + 128];
            this->alignedBits = this->bits;
            while ((unsigned long long)this->alignedBits % 128) ++this->alignedBits;

            selectCounter = 0;
            unsigned int bitsCounter = 0;
            unsigned char bitsToWrite = 0;
            unsigned int bitsToWritePos = 7;
            //unsigned int currOnes = 0;
            unsigned int upToBitPos = selectsTemp[2];
            blockLenInBits = selectsTemp[2] - selectsTemp[0];
            bool sparseBlock = false, monoBlock = false;
            if (blockLenInBits > THRESHOLD) sparseBlock = true;
            else if (blockLenInBits == L) monoBlock = true;
            for (unsigned int i = 0; i < textLen; ++i) {
                for (unsigned int j = 0; j < 8; ++j) {
                    if ((8 * i + j) == upToBitPos) {
                        selectCounter += 2;
                        if (selectCounter == selectLenTempNotExtended - 2) {
                            if ((numberOfOnes % L) > 0) {
                                if (sparseBlock && (((text[i] >> (7 - j)) & 1) == 1)) {
                                    unsigned int position = 8 * i + j;
                                    this->alignedBits[bitsCounter++] = position & 0xFF;
                                    this->alignedBits[bitsCounter++] = (position >> 8) & 0xFF;
                                    this->alignedBits[bitsCounter++] = (position >> 16) & 0xFF;
                                    this->alignedBits[bitsCounter++] = (position >> 24);
                                }
                                if (!sparseBlock && !monoBlock) {
                                    if (((text[i] >> (7 - j)) & 1) == 1) {
                                        bitsToWrite += (1 << bitsToWritePos);
                                    }
                                    this->alignedBits[bitsCounter++] = bitsToWrite;
                                }
                            } else if (!monoBlock && !sparseBlock && bitsToWritePos != 7) {
                                this->alignedBits[bitsCounter++] = bitsToWrite;
                            }
                            i = textLen;
                            break;
                        }
                        if (!monoBlock && !sparseBlock && bitsToWritePos != 7) {
                            this->alignedBits[bitsCounter++] = bitsToWrite;
                        }
                        selectsTemp[selectCounter + 1] = bitsCounter;
                        upToBitPos = selectsTemp[selectCounter + 2];
                        blockLenInBits = selectsTemp[selectCounter + 2] - selectsTemp[selectCounter];
                        sparseBlock = false;
                        monoBlock = false;
                        if (blockLenInBits > THRESHOLD) sparseBlock = true;
                        else if (blockLenInBits == L) monoBlock = true;
                        bitsToWrite = 0;
                        bitsToWritePos = 7;
                        //currOnes = 0;
                        continue;
                    }
                    if (monoBlock) continue;
                    if (sparseBlock) {
                        if (((text[i] >> (7 - j)) & 1) == 1) {
                            unsigned int position = 8 * i + j;
                            this->alignedBits[bitsCounter++] = position & 0xFF;
                            this->alignedBits[bitsCounter++] = (position >> 8) & 0xFF;
                            this->alignedBits[bitsCounter++] = (position >> 16) & 0xFF;
                            this->alignedBits[bitsCounter++] = (position >> 24);
                        }
                    } else {
                        //if (currOnes >= l - 1) continue;
                        if (((text[i] >> (7 - j)) & 1) == 1) {
                            //++currOnes;
                            bitsToWrite += (1 << bitsToWritePos);
                        }
                        if (bitsToWritePos == 0) {
                            this->alignedBits[bitsCounter++] = bitsToWrite;
                            bitsToWrite = 0;
                            bitsToWritePos = 7;
                        } else {
                            --bitsToWritePos;
                        }
                    }
                }
            }

            for (unsigned int i = 2; i < selectLenTempNotExtended; i += 2) ++selectsTemp[i];

            this->selectsLen = selectLenTemp / 2;
            this->selectsLen = (this->selectsLen / SUPERBLOCKLEN) * INTSINSUPERBLOCK;
            selectLenTemp /= 2;

            this->selects = new unsigned int[this->selectsLen + 32];
            this->alignedSelects = this->selects;
            while ((unsigned long long)this->alignedSelects % 128) ++this->alignedSelects;

            unsigned int bucket[INTSINSUPERBLOCK];
            selectCounter = 0;
            for (unsigned int j = 0; j < INTSINSUPERBLOCK; ++j) bucket[j] = 0;

            for (unsigned int i = 0; i < selectLenTemp; i += SUPERBLOCKLEN) {
                for (unsigned int j = 0; j < SUPERBLOCKLEN; ++j) {
                    bucket[j] = selectsTemp[2 * (i + j)];
                }
                bucket[SUPERBLOCKLEN] = selectsTemp[2 * i + 1];
                for (unsigned int j = 0; j < SUPERBLOCKLEN - 1; ++j) {
                    bucket[(SUPERBLOCKLEN + 1) + j / 2] += ((selectsTemp[2 * (i + j + 1) + 1] - bucket[SUPERBLOCKLEN]) << (16 * (j % 2)));
                }
                for (unsigned int j = 0; j < INTSINSUPERBLOCK; ++j) {
                    this->alignedSelects[selectCounter++] = bucket[j];
                    bucket[j] = 0;
                }
            }

            delete[] selectsTemp;
        }
        
        unsigned int getSelect_std(unsigned int i) {
            unsigned int start = i / L;
            unsigned int select = this->alignedSelects[2 * start];
            i %= L;
            if (i == 0) return select - 1;
            unsigned int selectDiff = this->alignedSelects[2 * start + 2] - select;
            if (selectDiff == L) return select - 1 + i;
            if (selectDiff > THRESHOLD) return *((unsigned int *)(this->alignedBits + this->alignedSelects[2 * start + 1] + 4 * (i - 1)));
            unsigned int popcnt;
            this->pointer = this->alignedBits + this->alignedSelects[2 * start + 1];
            for (unsigned int j = 64; j < selectDiff; j += 64) {
                popcnt = __builtin_popcountll(*((unsigned long long*)this->pointer));
                if (popcnt >= i) break;
                i -= popcnt;
                select += 64;
                this->pointer += 8;
            }
            while(true) {
                popcnt = popcntLUT[*this->pointer];
                if (popcnt >= i) break;
                i -= popcnt;
                select += 8;
                ++this->pointer;
            }
            for (unsigned int j = 0; j < 8; ++j) {
                if ((this->pointer[0] & masks[j]) > 0) {
                    if (i == 1) return select + j;
                    --i;
                }
            }
            return 0;
        }

        unsigned int getSelect_bch(unsigned int i) {
            unsigned int start = i / L;
            this->pointer2 = this->alignedSelects + INTSINSUPERBLOCK * (start / SUPERBLOCKLEN);
            unsigned int selectFromStart = start % SUPERBLOCKLEN;
            unsigned int select = this->pointer2[selectFromStart];
            i %= L;
            if (i == 0) return select - 1;
            unsigned int selectDiff;
            if (selectFromStart == (SUPERBLOCKLEN - 1)) {
                selectDiff = this->pointer2[INTSINSUPERBLOCK] - select;
            } else {
                selectDiff = this->pointer2[selectFromStart + 1] - select;
            }
            if (selectDiff == L) return select - 1 + i;
            unsigned int offset = this->pointer2[SUPERBLOCKLEN];
            if (selectFromStart > 0) {
                if (selectFromStart % 2 == 1) offset += (this->pointer2[SUPERBLOCKLEN + (selectFromStart + 1) / 2] & 0xFFFF);
                else offset += (this->pointer2[SUPERBLOCKLEN + (selectFromStart + 1) / 2] >> 16);
            }
            if (selectDiff > THRESHOLD) return *((unsigned int *)(this->alignedBits + offset + 4 * (i - 1)));
            unsigned int popcnt;
            this->pointer = this->alignedBits + offset;
            for (unsigned int j = 64; j < selectDiff; j += 64) {
                popcnt = __builtin_popcountll(*((unsigned long long*)this->pointer));
                if (popcnt >= i) break;
                i -= popcnt;
                select += 64;
                this->pointer += 8;
            }
            while(true) {
                popcnt = popcntLUT[*this->pointer];
                if (popcnt >= i) break;
                i -= popcnt;
                select += 8;
                ++this->pointer;
            }
            for (unsigned int j = 0; j < 8; ++j) {
                if ((this->pointer[0] & masks[j]) > 0) {
                    if (i == 1) return select + j;
                    --i;
                }
            }
            return 0;
        }

public:
	unsigned int *selects;
        unsigned int *alignedSelects;
        unsigned int selectsLen;
        unsigned char *bits;
        unsigned char *alignedBits;
        unsigned int bitsLen;
        unsigned int textLen;
        unsigned char *pointer;
        unsigned int *pointer2;
        
        const static unsigned int SUPERBLOCKLEN = 16;
        const static unsigned int INTSINSUPERBLOCK = 25;
        /*
        INTSINSUPERBLOCK = SUPERBLOCKLEN + 1 + (SUPERBLOCKLEN - 1) / 2;
        if ((SUPERBLOCKLEN - 1) % 2 == 1) ++INTSINSUPERBLOCK;
        */

        const unsigned int popcntLUT[256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};
        const unsigned char masks[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

	SelectBasic() {
            if (L >= THRESHOLD) {
                cout << "Error: not valid L and THRESHOLD values (THRESHOLD should be greater than L)" << endl;
                exit(1);
            }
            if (T == SelectBasicType::WITH_COMPRESSED_HEADERS) {
                if ((L - 1) * 4 > (65535 / (SUPERBLOCKLEN - 1)) || (THRESHOLD - 1) > (8 * (65535 / (SUPERBLOCKLEN - 1)))) {
                    cout << "Error: not valid L and THRESHOLD values" << endl;
                    exit(1);
                }
            }
            this->initialize();
	}

	~SelectBasic() {
            this->freeMemory();
	}

        void build(unsigned char *text, unsigned int textLen) {
            switch(T) {
                case SelectBasicType::WITH_COMPRESSED_HEADERS:
                    this->build_bch(text, textLen);
                    break;
                default:
                    this->build_std(text, textLen);
                    break;      
            }
        }
        
        unsigned int getSelect(unsigned int i) {
            switch(T) {
            case SelectBasicType::WITH_COMPRESSED_HEADERS:
                return this->getSelect_bch(i);
                break;
            default:
                return this->getSelect_std(i);
                break;
            }
        } 
           
	void save(FILE *outFile) {
            fwrite(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(&this->selectsLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            if (this->selectsLen > 0) fwrite(this->alignedSelects, (size_t)sizeof(unsigned int), (size_t)this->selectsLen, outFile);
            fwrite(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            if (this->bitsLen > 0) fwrite(this->alignedBits, (size_t)sizeof(unsigned char), (size_t)this->bitsLen, outFile);
        }
	
        void load(FILE *inFile) {
            this->free();
            size_t result;
            result = fread(&this->textLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading select" << endl;
                    exit(1);
            }
            result = fread(&this->selectsLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading select" << endl;
                    exit(1);
            }
            if (this->selectsLen > 0) {
                    this->selects = new unsigned int[this->selectsLen + 32];
                    this->alignedSelects = this->selects;
                    while ((unsigned long long)(this->alignedSelects) % 128) ++(this->alignedSelects);
                    result = fread(this->alignedSelects, (size_t)sizeof(unsigned int), (size_t)this->selectsLen, inFile);
                    if (result != this->selectsLen) {
                            cout << "Error loading select" << endl;
                            exit(1);
                    }
            }
            result = fread(&this->bitsLen, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading select" << endl;
                    exit(1);
            }
            if (this->bitsLen > 0) {
                    this->bits = new unsigned char[this->bitsLen + 128];
                    this->alignedBits = this->bits;
                    while ((unsigned long long)(this->alignedBits) % 128) ++(this->alignedBits);
                    result = fread(this->alignedBits, (size_t)sizeof(unsigned char), (size_t)this->bitsLen, inFile);
                    if (result != this->bitsLen) {
                            cout << "Error loading select" << endl;
                            exit(1);
                    }
            }
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
	
        unsigned int getSelectSize() {
            unsigned int size = sizeof(this->selectsLen) + sizeof(this->bitsLen) + sizeof(this->textLen) + sizeof(this->pointer) + sizeof(this->pointer2);
            if (this->selectsLen > 0) size += (this->selectsLen + 32) * sizeof(unsigned int);
            if (this->bitsLen > 0) size += (this->bitsLen + 128) * sizeof(unsigned char);
            return size;
        }
        
        unsigned int getTextSize() {
            return this->textLen * sizeof(unsigned char);
        }
        
        void testSelect(unsigned char* text, unsigned int textLen) {
            unsigned int selectTest = 0;

            for (unsigned int i = 0; i < textLen; ++i) {
                for (unsigned int j = 0; j < 8; ++j) {
                    if (((text[i] >> (7 - j)) & 1) == 1) {
                        ++selectTest;
                        if ((8 * i + j) != this->getSelect(selectTest)) {
                            cout << selectTest << " " << (8 * i + j) << " " << this->getSelect(selectTest);
                            cin.ignore();
                        }
                    }
                }
            }
        }
};

}

#endif	/* SELECT_HPP */

