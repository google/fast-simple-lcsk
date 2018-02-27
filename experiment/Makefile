all: stats_fasta

stats_fasta:
	g++ -o stats_fasta stats_fasta.cc ../fast_simple_lcsk/match_maker.cc ../fast_simple_lcsk/rolling_hasher.cc ../fast_simple_lcsk/lcsk.cc -O2 -std=c++11

clean:
	rm -f stats_fasta
