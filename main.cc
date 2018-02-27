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

#include <iostream>
#include <fstream>

#include "fast_simple_lcsk/lcsk.h"
#include "fast_simple_lcsk/match_pair.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 5) {
    printf(
      "Compute LCSk++ of two plain texts.\n\n"
      "Usage: ./main k input1 input2 output\n\n"
      "Example: ./main 4 test/tests/test.1.A test/tests/test.1.B out\n"
      "finds LCS4++ of files `test/tests/test.1.A` and `test/tests/test.1.B`\n"
      "and writes it to `out`\n"
    );
    return 0;
  };


  int k = stoi(argv[1]);
  ifstream infile1(argv[2]);
  string A;
  getline(infile1, A);

  ifstream infile2(argv[3]);
  string B;
  getline(infile2, B);

  printf("Sequence 1 length: %d\n", (int)A.size());
  printf("Sequence 2 length: %d\n", (int)B.size());
  printf("Computing LCSk++..\n");

  vector<pair<int, int>> recon;
  LcsKppSparseFast(A, B, k, &recon);
  int length = recon.size();
  
  printf("LCSk++ length: %d\n", length);
  cout << "MatchPairs created: " << ObjectCounter<MatchPair>::objects_created << endl;
  cout << "Max Alive MatchPairs: " << ObjectCounter<MatchPair>::max_objects_alive << endl;

  auto r = freopen(argv[4], "w", stdout);
  for (auto& p: recon) {
    putchar(A[p.first]);
  }
  return 0;
}
