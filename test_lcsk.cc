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
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "fast_simple_lcsk/lcsk.h"
#include "util/lcsk_testing.h"
#include "util/random_strings.h"
using namespace std;

const int only_run_fast_version = 0;

// Length of the strings.
const int kStringLen = 100;

// Number of performed simulations.
const int kSimulationRuns = 10000;

// Default value of the k parameter.
const int kK = 3;

// If kPerr is set to -1 then rand-to-rand strings are aligned, otherwise
// rand-to-modified-copy simulations are performed.
// const double kPerr = -1.0;
const double kPerr = 0.1;

int test_lcsk(const string &a, const string &b, const int K) {
  vector<pair<int, int> > lcsk_sparse_slow_recon;
  vector<pair<int, int> > lcskpp_sparse_slow_recon;
  if (!only_run_fast_version) {
    LcskSparseSlow(a, b, K, &lcsk_sparse_slow_recon);
    LcskppSparseSlow(a, b, K, &lcskpp_sparse_slow_recon);
  }

  vector<pair<int, int> > lcsk_sparse_fast_recon;
  vector<pair<int, int> > lcskpp_sparse_fast_recon;
  LcsKSparseFast(a, b, K, &lcsk_sparse_fast_recon);
  LcsKppSparseFast(a, b, K, &lcskpp_sparse_fast_recon);

  printf("lcsk_sparse_slow_len=%d lcsk_sparse_fast_len=%d\n",
         (int)lcsk_sparse_slow_recon.size(),
         (int)lcsk_sparse_fast_recon.size());
  printf("lcskpp_sparse_slow_len=%d lcskpp_sparse_fast_len=%d\n",
         (int)lcskpp_sparse_slow_recon.size(),
         (int)lcskpp_sparse_fast_recon.size());

  if (!only_run_fast_version) {
    assert(lcsk_sparse_slow_recon.size() == lcsk_sparse_fast_recon.size());
    assert(ValidLcsk(a, b, K, lcsk_sparse_slow_recon));

    assert(lcskpp_sparse_slow_recon.size() == lcskpp_sparse_fast_recon.size());
    assert(ValidLcskpp(a, b, K, lcskpp_sparse_slow_recon));
  }
  assert(ValidLcsk(a, b, K, lcsk_sparse_fast_recon));
  assert(ValidLcskpp(a, b, K, lcskpp_sparse_fast_recon));

  return lcsk_sparse_fast_recon.size();
}

int run_one_simulation() {
  pair<string, string> ab;
  ab.first = generate_string(kStringLen);
  ab.second = kPerr < 0 ? generate_string(kStringLen)
                        : generate_similar(ab.first, kPerr);
  const string &a = ab.first;
  const string &b = ab.second;
  return test_lcsk(a, b, kK);
}

void calculate_distribution(map<int, double> &distr) {
  distr.clear();
  for (int i = 0; i < kSimulationRuns; ++i) {
    distr[run_one_simulation()] += 1.0 / kSimulationRuns;
  }
}

int main(int argc, char *argv[]) {
  printf("Running tests on %d random pairs ", kSimulationRuns);
  printf("with the following parameters:\n");
  printf("  string length=%d\n", kStringLen);
  printf("  k=%d\n", kK);
  printf("  pErr=%0.2lf\n", kPerr);

  srand(1603);
  
  map<int, double> distr;
  calculate_distribution(distr);

  double sum_prob = 0;
  double e_lcs = 0;

  for (int i = 0; i <= kStringLen; ++i) {
    double p = distr[i];
    sum_prob += p;
    e_lcs += p * i;
  }

  assert(0.99999 <= sum_prob <= 1.00001);
  printf("Expected LCSk++=%0.3lf\n", e_lcs);
  printf("Test PASSED!\n");
  return 0;
}
