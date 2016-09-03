UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
ASMLIB = libaelf64.a
else
ASMLIB = libacof64.lib
endif

CXX=g++
CFLAGS=-Wall -std=c++11 -O3 -mpopcnt
	
all: testRank testSelect countFMHWT
	
testRank: test/testRank.cpp libshared.a libs/$(ASMLIB)
	$(CXX) $(CFLAGS) test/testRank.cpp libshared.a libs/$(ASMLIB) -o test/testRank
	
testSelect: test/testSelect.cpp libshared.a libs/$(ASMLIB)
	$(CXX) $(CFLAGS) test/testSelect.cpp libshared.a libs/$(ASMLIB) -o test/testSelect
	
countFMHWT: test/countFMHWT.cpp libshared.a libs/$(ASMLIB)
	$(CXX) $(CFLAGS) test/countFMHWT.cpp libshared.a libs/$(ASMLIB) -o test/countFMHWT

libshared.a: shared/common.h shared/common.cpp shared/patterns.h shared/patterns.cpp shared/sais.h shared/sais.c shared/timer.h shared/timer.cpp shared/xxhash.h shared/xxhash.c shared/rank.hpp shared/select.hpp shared/wt.hpp shared/hash.hpp shared/huff.h shared/huff.cpp shared/fm.hpp
	$(CXX) $(CFLAGS) -c shared/common.cpp shared/patterns.cpp shared/sais.c shared/timer.cpp shared/xxhash.c shared/huff.cpp
	ar rcs libshared.a common.o patterns.o sais.o xxhash.o timer.o huff.o shared/rank.hpp shared/select.hpp shared/wt.hpp shared/hash.hpp shared/fm.hpp
	make cleanObjects

cleanObjects:
	rm -f *o

clean:
	rm -f *o test/testRank test/testSelect test/countFMHWT libshared.a