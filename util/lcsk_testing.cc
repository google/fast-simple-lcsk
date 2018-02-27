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

#include <algorithm>
#include <map>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cassert>
#include <cmath>
#include <cstdlib>

#include "lcsk_testing.h"
using namespace std;

// This function determines the total number of
// distinct characters in input strings a and b.
// Outputs are: aid[character] = unique_character_id
//              alphabet_size = total number of distinct chars
static void prepare_alphabet(const string &a, const string &b,
                             vector<char> &aid, int &alphabet_size) {
  aid = vector<char>(256, -1);
  alphabet_size = 0;

  for (size_t i = 0; i < a.size(); ++i) {
    if (aid[a[i]] == -1) {
      aid[a[i]] = alphabet_size++;
    }
  }
  for (size_t i = 0; i < b.size(); ++i) {
    if (aid[b[i]] == -1) {
      aid[b[i]] = alphabet_size++;
    }
  }
}

// matches is filled with sorted pairs (i,j) meaning that for
// every such pair a[i...i+k-1] == b[j...j+k-1].
//
// In this implementation it is assumed that
// power(alphabet_size, k) < power(2, 64). In case this is
// not true in the particular application, this function
// would have to be reimplemented (for example by using
// non-perfect hashing or suffix arrays) and everything else
// should work again.
static void get_matches(const string &a, const string &b, const int k,
                        vector<pair<int, int> > *matches) {
  assert(matches != NULL);
  matches->clear();

  vector<char> aid;
  int alphabet_size;
  prepare_alphabet(a, b, aid, alphabet_size);

  // We assume: alphabet_size ** k < 2 ** 64,
  // in the case this does not hold, the entire
  // get_matches function probably has to be
  // reimplemented using non-perfect hashing or
  // suffix arrays.
  if (k * log(alphabet_size) >= 64 * log(2)) {
    fprintf(stderr,
            "We assume that alphabet_size ** k <\
2 ** 64.\nPlease see lcsk.cpp for more information.");
    exit(1);
  }

  typedef unordered_multimap<uint64_t, int> MatchIndexType;
  unique_ptr<MatchIndexType> match_index =
      unique_ptr<MatchIndexType>(new MatchIndexType());

  uint64_t hash_mod = 1;
  for (int i = 0; i < k; ++i) hash_mod *= alphabet_size;

  if (alphabet_size == 4) {
    assert(hash_mod == (1LL << (2 * k)));
  }

  uint64_t rolling_hash = 0;
  for (int i = 0; i < a.size(); ++i) {
    rolling_hash = rolling_hash * alphabet_size + aid[a[i]];
    rolling_hash %= hash_mod;

    if (i + 1 >= k) {
      match_index->insert(MatchIndexType::value_type(rolling_hash, i - k + 1));
    }
  }

  rolling_hash = 0;
  for (int i = 0; i < b.size(); ++i) {
    rolling_hash = rolling_hash * alphabet_size + aid[b[i]];
    rolling_hash %= hash_mod;

    if (i + 1 >= k) {
      auto positions_in_a = match_index->equal_range(rolling_hash);
      for (auto it = positions_in_a.first; it != positions_in_a.second; ++it) {
        matches->push_back(make_pair(it->second, i - k + 1));
      }
    }
  }

  sort(matches->begin(), matches->end());
}

static void fill_lcsk_reconstruction(const vector<pair<int, int> > &matches,
                                     const int k, const vector<int> &prev_idx,
                                     const int last_idx,
                                     vector<pair<int, int> > *lcsk_recon,
                                     const bool lcskpp) {
  assert(lcsk_recon != NULL);
  lcsk_recon->clear();

  for (int i = last_idx; i != -1; i = prev_idx[i]) {
    int r = matches[i].first + k - 1;
    int c = matches[i].second + k - 1;

    if (prev_idx[i] == -1 || 
        (matches[prev_idx[i]].first + k <= matches[i].first &&
         matches[prev_idx[i]].second + k <= matches[i].second)) {
      // Taking the entire match ...
      for (int j = 0; j < k; ++j, --r, --c) {
        lcsk_recon->push_back(make_pair(r, c));
      }
    } else {
      // ... otherwise it is a continuation (lcskpp only). Only the
      // last character is taken
      assert(lcskpp);
      
      int curr = i;
      int prev = prev_idx[i];

      assert(matches[prev].first + 1 == matches[curr].first &&
             matches[prev].second + 1 == matches[curr].second);
      
      lcsk_recon->push_back(make_pair(r, c));
    }
  }

  reverse(lcsk_recon->begin(), lcsk_recon->end());
}

// Assume matches is sorted by standard pair ordering.
static void LcskSparseSlow(const vector<pair<int, int> > &matches, const int k,
                           vector<pair<int, int> > *lcsk_reconstruction,
                           const bool lcskpp) {
  assert(is_sorted(matches.begin(), matches.end()));

  if (matches.empty()) {
    lcsk_reconstruction->clear();
  } else {
    int n = matches.size();
    vector<int> dp(n);
    vector<int> recon(n);
    int best_idx = 0;

    for (int i = 0; i < n; ++i) {
      dp[i] = k;
      recon[i] = -1;

      for (int j = i - 1; j >= 0; --j) {
        if (matches[j].first + k <= matches[i].first &&
            matches[j].second + k <= matches[i].second) {
          // 1) Taking the entire match interval and continuing
          // another match which 'ended'.
          if (dp[j] + k > dp[i]) {
            dp[i] = dp[j] + k;
            recon[i] = j;
          }
        }

        if (lcskpp && matches[j].first + 1 == matches[i].first &&
            matches[j].second + 1 == matches[i].second) {
          // 2) Continuation
          if (dp[j] + 1 > dp[i]) {
            dp[i] = dp[j] + 1;
            recon[i] = j;
          }
        }
      }

      if (dp[i] > dp[best_idx]) {
        best_idx = i;
      }
    }

    fill_lcsk_reconstruction(matches, k, recon, best_idx, lcsk_reconstruction, lcskpp);
  }
}

void LcskSparseSlow(const string &a, const string &b, const int k,
                    vector<pair<int, int> > *lcsk_reconstruction,
                    const bool lcskpp) {
  vector<pair<int, int> > matches;
  get_matches(a, b, k, &matches);
  LcskSparseSlow(matches, k, lcsk_reconstruction, lcskpp);
}

bool ValidLcsk(const string &a, const string &b, const int k,
               const vector<pair<int, int> > &lcsk_recon,
               const bool lcskpp) {
  // 1) Ensure chars corresponding to the indices match.
  for (auto match : lcsk_recon) {
    int i = match.first;
    int j = match.second;

    if (i < 0 || i >= a.size()) {
      return false;
    }
    if (j < 0 || j >= b.size()) {
      return false;
    }
    if (a[i] != b[j]) {
      return false;
    }
  }

  // 2) Ensure runs of indices have correct lengths
  int run_a = 1;
  int run_b = 1;
  for (size_t i = 1; i < lcsk_recon.size(); ++i) {
    if (lcsk_recon[i - 1].first >= lcsk_recon[i].first) {
      return false;
    }
    if (lcsk_recon[i - 1].second >= lcsk_recon[i].second) {
      return false;
    }

    if (lcsk_recon[i - 1].first + 1 == lcsk_recon[i].first) {
      ++run_a;
    }
    if (lcsk_recon[i - 1].second + 1 == lcsk_recon[i].second) {
      ++run_b;
    }

    if (i + 1 == lcsk_recon.size() ||
        lcsk_recon[i - 1].first + 1 != lcsk_recon[i].first) {
      if ((!lcskpp && run_a % k != 0) || (lcskpp && run_a < k)) {
        return false;
      }
      run_a = 1;
    }

    if (i + 1 == lcsk_recon.size() ||
        lcsk_recon[i - 1].second + 1 != lcsk_recon[i].second) {
      if ((!lcskpp && run_b % k != 0) || (lcskpp && run_b < k)) {
        return false;
      }
      run_b = 1;
    }
  }

  return true;
}

static int min3(int a, int b, int c) { return min(min(a, b), c); }

void LcskSlow(const string &a, const string &b, const int K, int *lcsk_length,
              const bool lcskpp) {
  vector<vector<int> > dp(a.size() + 1, vector<int>(b.size() + 1));

  for (int i = 0; i <= a.size(); ++i) dp[i][0] = 0;
  for (int j = 0; j <= b.size(); ++j) dp[0][j] = 0;

  for (int i = 1; i <= a.size(); ++i) {
    for (int j = 1; j <= b.size(); ++j) {
      dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
      
      // 2*K is good enough limit because everything bigger is
      // covered by some precedent matching interval.
      for (int k = 1; k <= min3(i, j, 2*K); ++k) {
        char aa = a[i - k];
        char bb = b[j - k];
        if (aa != bb) {
          break;
        }

        if ((!lcskpp && k == K) || (lcskpp && k >= K)) {
          dp[i][j] = max(dp[i][j], dp[i - k][j - k] + k);
        }
      }
    }
  }

  *lcsk_length = dp[a.size()][b.size()];
}


// Exposed functions
void LcskSparseSlow(const std::string &a, const std::string &b, const int k,
                    std::vector<std::pair<int, int> > *lcsk_reconstruction) {
  LcskSparseSlow(a, b, k, lcsk_reconstruction, false);
}

void LcskppSparseSlow(const std::string &a, const std::string &b, const int k,
                      std::vector<std::pair<int, int> > *lcsk_reconstruction) {
  LcskSparseSlow(a, b, k, lcsk_reconstruction, true);
}

bool ValidLcsk(const std::string &a, const std::string &b, const int k,
               const std::vector<std::pair<int, int> > &lcsk_recon) {
  return ValidLcsk(a, b, k, lcsk_recon, false);
}

bool ValidLcskpp(const std::string &a, const std::string &b, const int k,
                 const std::vector<std::pair<int, int> > &lcsk_recon) {
  return ValidLcsk(a, b, k, lcsk_recon, true);
}

void LcskSlow(const std::string &a, const std::string &b, const int K,
              int *lcsk_length) {
  LcskSlow(a, b, K, lcsk_length, false);
}

void LcskppSlow(const std::string &a, const std::string &b, const int K,
                int *lcsk_length) {
  LcskSlow(a, b, K, lcsk_length, true);
}

