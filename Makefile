all: test_lcsk main

test_lcsk:
	g++ -o test_lcsk test_lcsk.cc util/lcsk_testing.cc fast_simple_lcsk/match_maker.cc fast_simple_lcsk/rolling_hasher.cc fast_simple_lcsk/lcsk.cc -O2 -std=c++11

main:
	g++ -o main main.cc fast_simple_lcsk/match_maker.cc fast_simple_lcsk/rolling_hasher.cc fast_simple_lcsk/lcsk.cc -O2 -std=c++11

test:
	./test_lcsk

clean:
	rm -f test_lcsk main stats stats_fasta
