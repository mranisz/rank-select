#ifndef WT_HPP
#define	WT_HPP

#include <tuple>
#include "rank.hpp"

namespace shared {
    
template<class RANK> class WT {
private:
	void freeMemory() {
            if (this->nodes[0] != NULL) this->nodes[0]->freeMemory();
            if (this->nodes[1] != NULL) this->nodes[1]->freeMemory();
            delete this->rank;
        }
        
	void initialize() {
            this->rank = new RANK();
            this->nodes[0] = NULL;
            this->nodes[1] = NULL;
        }

public:
        RANK *rank;
	WT* nodes[2];

	WT() {
            this->initialize();
	};
        
        ~WT() {
            this->free();
	}

	unsigned int getWTSize() {
            unsigned int size = 2 * sizeof(WT *);
            size += this->rank->getSize();
            if (this->nodes[0] != NULL) size += this->nodes[0]->getWTSize();
            if (this->nodes[1] != NULL) size += this->nodes[1]->getWTSize();
            return size;
        }
	void save(FILE *outFile) {
            bool nullNode = false;
            bool notNullNode = true;
            this->rank->save(outFile);
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
            size_t result;
            this->rank->load(inFile);
            result = fread(&isNotNullNode, (size_t)sizeof(bool), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading wt" << endl;
                    exit(1);
            }
            if (isNotNullNode) {
                    this->nodes[0] = new WT();
                    this->nodes[0]->load(inFile);
            }
            result = fread(&isNotNullNode, (size_t)sizeof(bool), (size_t)1, inFile);
            if (result != 1) {
                    cout << "Error loading wt" << endl;
                    exit(1);
            }
            if (isNotNullNode) {
                    this->nodes[1] = new WT();
                    this->nodes[1]->load(inFile);
            }
        }
        
	void free() {
            this->freeMemory();
            this->initialize();
        }
        
        static WT *createHWT(unsigned char *text, unsigned int textLen, unsigned int wtLevel, unsigned long long *code, unsigned int *codeLen) {
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

            WT *node = new WT();

            for (int i = 0; i < 2; ++i) {
                    node->nodes[i] = createHWT(texts[i], textLengths[i], wtLevel + 1, code, codeLen);
                    delete[] texts[i];
            }

            unsigned int nodeBitLen = textLen / 8;
            if (textLen % 8 > 0) ++nodeBitLen;
            unsigned char *bitsInNode = new unsigned char[nodeBitLen];
            for (unsigned int i = 0; i < nodeBitLen; ++i) bitsInNode[i] = 0;
            for (unsigned int i = 0; i < textLen; ++i) bitsInNode[i / 8] += (((code[text[i]] >> wtLevel) & 1) << (7 - (i % 8)));
            node->rank->build(bitsInNode, nodeBitLen);
            delete[] bitsInNode;
            return node;
        }
        
        static pair<unsigned int, unsigned int> getRankHWT(WT<RANK> *wt, unsigned long long code, unsigned int codeLen, unsigned int iFirst, unsigned int iLast) {
            unsigned int rank;
            for (unsigned int wtLevel = 0; wtLevel < codeLen; ++wtLevel) {
                int nextNode = (code >> wtLevel) & 1;
                
                rank = wt->rank->rank(iFirst);
                if (nextNode == 0) rank = iFirst - rank;
                iFirst = rank;
                
                rank = wt->rank->rank(iLast);
                if (nextNode == 0) rank = iLast - rank;
                iLast = rank;

                wt = wt->nodes[nextNode];
            }
            
            return make_pair(iFirst, iLast);
        }
};

}

#endif	/* WT_HPP */

