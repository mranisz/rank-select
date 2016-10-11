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

libshared.a: shared/common.hpp shared/patterns.hpp shared/sais.h shared/sais.c shared/xxhash.h shared/xxhash.c shared/timer.hpp shared/rank.hpp shared/select.hpp shared/wt.hpp shared/hash.hpp shared/huff.hpp shared/fm.hpp
	$(CXX) $(CFLAGS) -c shared/sais.c shared/xxhash.c
	ar rcs libshared.a sais.o xxhash.o shared/common.hpp shared/patterns.hpp shared/rank.hpp shared/select.hpp shared/wt.hpp shared/hash.hpp shared/fm.hpp shared/huff.hpp shared/timer.hpp
	make cleanObjects

cleanObjects:
	rm -f *o

clean:
	rm -f *o test/testRank test/testSelect test/countFMHWT libshared.a