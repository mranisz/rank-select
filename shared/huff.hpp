#ifndef HUFF_HPP
#define	HUFF_HPP

#include <vector>
#include <queue>
#include <cmath>

using namespace std;

namespace shared {
    
const int ALPHABET = 256;

class Symbol {
public:
	unsigned int symbol;
	unsigned int code;
	unsigned int codeLen;

	Symbol(unsigned int symbol) {
		this->symbol = symbol;
		this->code = 0;
		this->codeLen = 0;
	}
};

template<typename DATATYPE> class HuffNode {
public:
	DATATYPE freq;
	vector<Symbol> symbols;

	HuffNode() {
		this->freq = 0;
	};

	void add(HuffNode *hNode, unsigned int d, unsigned int order) {
            this->freq += hNode->freq;
            for(vector<Symbol>::iterator it = hNode->symbols.begin(); it != hNode->symbols.end(); ++it) {
                    Symbol *s = new Symbol((*it).symbol);
                    //s->code = (*it).code + order * (unsigned int)pow((double)d, (double)(*it).codeLen);
                    s->code = (*it).code * d + order;
                    s->codeLen = (*it).codeLen + 1;
                    this->symbols.push_back(*s);
            }
        }

	HuffNode(DATATYPE freq, unsigned int symbol) {
		this->freq = freq;
		Symbol *sym = new Symbol(symbol);
		this->symbols.push_back(*sym);
	};
        
	bool operator()(const HuffNode* lh, const HuffNode* rh) const { return lh->freq > rh->freq; }
};

template<typename DATATYPE> void encodeHuffFromFreq(unsigned int d, DATATYPE *freq, unsigned long long *huffCode, unsigned int *huffCodeLen) { 
	unsigned int n = 0;
	priority_queue<HuffNode<DATATYPE> *, vector<HuffNode<DATATYPE> *>, HuffNode<DATATYPE>> heap;
	for (int i = 0; i < ALPHABET; ++i) if (freq[i] > 0) {
		++n;
		heap.push(new HuffNode<DATATYPE>(freq[i], (unsigned int)i));
	}
	unsigned int q = (unsigned int)(ceil((double)(n - d) / (d - 1)));
	unsigned int N = d + q * (d - 1);
	for (unsigned int i = 0; i < (N - n); ++i) heap.push(new HuffNode<DATATYPE>(0, (unsigned int)(ALPHABET + i)));
	while (heap.size() > 1) {
		HuffNode<DATATYPE> *newHuffNode = new HuffNode<DATATYPE>();
		for (unsigned int i = 0; i < d; ++i) {
			HuffNode<DATATYPE> *hf = heap.top();
			heap.pop();
			newHuffNode->add(hf, d, i);
			delete hf;
		}
		heap.push(newHuffNode);
	}
	HuffNode<DATATYPE> *hf = heap.top();
	heap.pop();
	for(vector<Symbol>::iterator it = hf->symbols.begin(); it != hf->symbols.end(); ++it) {
		unsigned int s = (*it).symbol;
		if (s > (ALPHABET - 1)) continue;
		huffCode[s] = (*it).code;
		huffCodeLen[s] = (*it).codeLen;
	}
}

template void encodeHuffFromFreq<unsigned int>(unsigned int d, unsigned int *freq, unsigned long long *huffCode, unsigned int *huffCodeLen);
template void encodeHuffFromFreq<unsigned long long>(unsigned int d, unsigned long long *freq, unsigned long long *huffCode, unsigned int *huffCodeLen);

template<typename DATATYPE> void encodeHuffFromText(unsigned int d, unsigned char *text, DATATYPE textLen, unsigned long long *huffCode, unsigned int *huffCodeLen) {
	DATATYPE freq[ALPHABET];
	for (int i = 0; i < ALPHABET; ++i) {
		freq[i] = 0;
		huffCode[i] = 0;
		huffCodeLen[i] = 0;
	}
	for (DATATYPE i = 0; i < textLen; ++i) freq[(unsigned int)text[i]]++;
	encodeHuffFromFreq(d, freq, huffCode, huffCodeLen);
}

template void encodeHuffFromText<unsigned int>(unsigned int d, unsigned char *text, unsigned int textLen, unsigned long long *huffCode, unsigned int *huffCodeLen);
template void encodeHuffFromText<unsigned long long>(unsigned int d, unsigned char *text, unsigned long long textLen, unsigned long long *huffCode, unsigned int *huffCodeLen);

}

#endif	/* HUFF_HPP */

