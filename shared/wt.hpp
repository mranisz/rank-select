#ifndef WT_HPP
#define	WT_HPP

#include <tuple>
#include "rank.hpp"

namespace shared {
    
class WTNode {
private:
	void freeMemory() {
            if (this->nodes[0] != NULL) this->nodes[0]->freeMemory();
            if (this->nodes[1] != NULL) this->nodes[1]->freeMemory();
        }
        
	void initialize() {
            this->nodes[0] = NULL;
            this->nodes[1] = NULL;
            this->offset = 0;
            this->offsetRank = 0;
        }

public:
        unsigned int offset;
        unsigned int offsetRank;
	WTNode* nodes[2];

	WTNode() {
            this->initialize();
	};
        
        ~WTNode() {
            this->free();
	}

	unsigned int getWTNodeSize() {
            unsigned int size = 2 * sizeof(WTNode *) + sizeof(this->offset) + sizeof(this->offsetRank);
            if (this->nodes[0] != NULL) size += this->nodes[0]->getWTNodeSize();
            if (this->nodes[1] != NULL) size += this->nodes[1]->getWTNodeSize();
            return size;
        }
        
	void save(FILE *outFile) {
            bool nullNode = false;
            bool notNullNode = true;
            fwrite(&this->offset, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            fwrite(&this->offsetRank, (size_t)sizeof(unsigned int), (size_t)1, outFile);
            if (this->nodes[0] == NULL) fwrite(&nullNode, (size_t)sizeof(bool), (size_t)1, outFile);
            else {
                    fwrite(&notNullNode, (size_t)sizeof(bool), (size_t)1, outFile);
                    this->nodes[0]->save(outFile);
            }
            if (this->nodes[1] == NULL) fwrite(&nullNode, (size_t)sizeof(bool), (size_t)1, outFile);
            else {
                    fwrite(&notNullNode, (size_t)sizeof(bool), (size_t)1, outFile);
                    this->nodes[1]->save(outFile);
            }
        }
        
	void load(FILE *inFile) {
            this->free();
            bool isNotNullNode;
            size_t result = fread(&this->offset, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading wt node" << endl;
                    exit(1);
            }
            result = fread(&this->offsetRank, (size_t)sizeof(unsigned int), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading wt node" << endl;
                    exit(1);
            }
            result = fread(&isNotNullNode, (size_t)sizeof(bool), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading wt node" << endl;
                    exit(1);
            }
            if (isNotNullNode) {
                    this->nodes[0] = new WTNode();
                    this->nodes[0]->load(inFile);
            }
            result = fread(&isNotNullNode, (size_t)sizeof(bool), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading wt node" << endl;
                    exit(1);
            }
            if (isNotNullNode) {
                    this->nodes[1] = new WTNode();
                    this->nodes[1]->load(inFile);
            }
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
}; 
    
template<class RANK> class WT {
private:
	void freeMemory() {
            if (this->offsetNode != NULL) this->offsetNode->free();
            delete this->rank;
        }
        
	void initialize() {
            this->offsetNode = NULL;
            this->rank = new RANK();
        }

public:
        WTNode *offsetNode;
        RANK *rank;

	WT() {
            this->initialize();
	};
        
        ~WT() {
            this->free();
	}

	unsigned int getWTSize() {
            return sizeof(this->offsetNode) + this->rank->getSize();
        }
        
	void save(FILE *outFile) {
            this->offsetNode->save(outFile);
            this->rank->save(outFile);
        }
        
	void load(FILE *inFile) {
            this->free();
            this->offsetNode = new WTNode();
            this->offsetNode->load(inFile);
            this->rank->load(inFile);
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
        
        void createHWT(unsigned char *text, unsigned int textLen, unsigned long long *code, unsigned int *codeLen) {
            vector<unsigned char> textBits;
            unsigned int offset = 0;
            unsigned int offsetRank = 0; 
            this->offsetNode = WT::createHWTNodes(textBits, offset, offsetRank, text, textLen, 0, code, codeLen);
            this->rank->build(&textBits[0], offset);
        }
        
        static WTNode *createHWTNodes(vector<unsigned char>& textBits, unsigned int &offset, unsigned int &offsetRank, unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen) {
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

            WTNode *node = new WTNode();

            for (int i = 0; i < 2; ++i) {
                    node->nodes[i] = WT::createHWTNodes(textBits, offset, offsetRank, texts[i], textLengths[i], wtLevel + 1, code, codeLen);
                    delete[] texts[i];
            }
            
            node->offset = 8 * offset;
            node->offsetRank = offsetRank;
            unsigned int nodeBitLen = textLen / 8;
            if (textLen % 8 > 0) ++nodeBitLen;
            unsigned char *bitsInNode = new unsigned char[nodeBitLen];
            for (unsigned int i = 0; i < nodeBitLen; ++i) bitsInNode[i] = 0;
            for (unsigned int i = 0; i < textLen; ++i) {
                if (((code[text[i]] >> wtLevel) & 1) == 1) ++offsetRank;
                bitsInNode[i / 8] += (((code[text[i]] >> wtLevel) & 1) << (7 - (i % 8)));
            }
            textBits.insert(textBits.end(), bitsInNode, bitsInNode + nodeBitLen);
            offset += nodeBitLen;
            delete[] bitsInNode;
            return node;
        }
        
        static pair<unsigned int, unsigned int> getRankHWT(WT<RANK> *wt, unsigned long long code, unsigned int codeLen, unsigned int iFirst, unsigned int iLast) {
            WTNode *node = wt->offsetNode;
            for (unsigned int wtLevel = 0; wtLevel < codeLen; ++wtLevel) {
                int nextNode = (code >> wtLevel) & 1;
                if (nextNode == 0) {
                    iFirst -= (wt->rank->rank(node->offset + iFirst) - node->offsetRank);
                    iLast -= (wt->rank->rank(node->offset + iLast) - node->offsetRank);
                } else {
                    iFirst = wt->rank->rank(node->offset + iFirst) - node->offsetRank;
                    iLast = wt->rank->rank(node->offset + iLast) - node->offsetRank;
                }
                node = node->nodes[nextNode];
            } 
            return make_pair(iFirst, iLast);
        }
};

}

#endif	/* WT_HPP */

