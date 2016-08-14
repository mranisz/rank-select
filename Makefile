UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
ASMLIB = libaelf64.a
else
ASMLIB = libacof64.lib
endif

CXX=g++
CFLAGS=-Wall -std=c++11 -O3 -mpopcnt
	
all: testRank testSelect
	
testRank: test/testRank.cpp libshared.a libs/$(ASMLIB)
	$(CXX) $(CFLAGS) test/testRank.cpp libshared.a libs/$(ASMLIB) -o test/testRank
	
testSelect: test/testSelect.cpp libshared.a libs/$(ASMLIB)
	$(CXX) $(CFLAGS) test/testSelect.cpp libshared.a libs/$(ASMLIB) -o test/testSelect

libshared.a: shared/common.h shared/common.cpp shared/patterns.h shared/patterns.cpp shared/sais.h shared/sais.c shared/timer.h shared/timer.cpp shared/xxhash.h shared/xxhash.c shared/hash.h shared/hash.cpp shared/wt.h shared/wt.cpp shared/huff.h shared/huff.cpp shared/rank.hpp shared/select.hpp
	$(CXX) $(CFLAGS) -c shared/common.cpp shared/patterns.cpp shared/sais.c shared/timer.cpp shared/xxhash.c shared/hash.cpp shared/wt.cpp shared/huff.cpp
	ar rcs libshared.a common.o patterns.o sais.o timer.o xxhash.o hash.o wt.o huff.o
	make cleanObjects

cleanObjects:
	rm -f *o

clean:
	rm -f *o test/testRank test/testSelect libshared.a