// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cassert>

#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>

#include "../fast_simple_lcsk/lcsk.h"
#include "../fast_simple_lcsk/match_pair.h"
#include "../fast_simple_lcsk/rolling_hasher.h"

using namespace std;


#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

long long CountMatchPairs(const string& s, const int k) {
  vector<char> char_to_id(256);
  char_to_id['A'] = 0;
  char_to_id['C'] = 1;
  char_to_id['T'] = 2;
  char_to_id['G'] = 3;
  RollingHasher rolling_hasher(s, k, char_to_id,
                               /*alphabet_size=*/4);
  unordered_map<unsigned long long, long long> kmer_counts;
  for (unsigned long long hash = -1;
       rolling_hasher.Next(&hash);) {
    ++kmer_counts[hash];
  }
  long long num_match_pairs = 0;
  for (const auto& kmer_count : kmer_counts) {
    num_match_pairs += kmer_count.second * kmer_count.second;
  }
  return num_match_pairs;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf(
      "Example: ./stats_fasta 4 input.fa\n"
      "outputs lcskpplen, number of matchpair objects created, max number of matchpair objects alive\n"
    );
    return 0;
  };

  int k = stoi(argv[1]);
  ifstream infile(argv[2]);

  vector<string> sequences;
  string line;
  string current_seq;
  bool first = true;
  while (getline(infile, line)) {
    if (!line.size()) continue;

    if (line[0] == '>') {
      if (!first) {
        sequences.push_back(current_seq);
      }
      current_seq = "";
      first = false;
    } else if (line[0] == ',') {
      continue;
    } else {
      current_seq += line;
    }
  }
  sequences.push_back(current_seq);

  string input = "";
  for (string& sequence : sequences) {
    for (char c : sequence) {
      if (c == 'A' || c == 'G' || c == 'T' || c == 'C') input.push_back(c);
    }
  }

  const int n = input.size();
  cerr << "input.size()=" << n << endl;

  const long long num_match_pairs = CountMatchPairs(input, k);
  vector<pair<int, int>> recon;
  LcsKSparseFast(input, input, k, &recon);

  // '+1' comes from a single dummy MatchPair object in the first row of the
  // compressed table.
  assert(num_match_pairs + 1 == ObjectCounter<MatchPair>::objects_created);

  const int length = recon.size();
  cout << n << " "
       << length << " "
       << num_match_pairs << " "
       << ObjectCounter<MatchPair>::max_objects_alive << endl;
  return 0;
}
