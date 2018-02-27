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

#ifndef MATCH_MAKER
#define MATCH_MAKER

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>

#include "rolling_hasher.h"

enum MatchMakerType { NAIVE, PERFECT_HASH, };

// This interface provides a single GetNextMatches method.
// On i-th call of the of the method, it returns a vector filled
// with indices j such that a[i,i+k) == b[j,j+k).
class MatchMaker {
 public:
  MatchMaker() {}
  virtual ~MatchMaker() {}

  virtual bool GetNextMatches(std::vector<int>* matches) = 0;

  static std::unique_ptr<MatchMaker> Create(const std::string& a,
                                            const std::string& b, int k,
                                            MatchMakerType type);
};

// An implementation of the MatchMaker using brute force string
// matching for constructing the output vectors.
class NaiveMatchMaker : public MatchMaker {
 public:
  NaiveMatchMaker(const std::string& a, const std::string& b, int k)
      : a_(a), b_(b), k_(k), row_(0) {}

  bool GetNextMatches(std::vector<int>* matches) override;

 private:
  std::string a_;
  std::string b_;
  int k_;
  int row_;
};

// An implementation of the MatchMaker which assumes that alphabet_size^k fits
// into a 64-bit integer. A RollingHasher is used to efficiently find the
// matching points between strings a and b in complexity proportional to sum of
// the lengths of these strings.
class PerfectHashMatchMaker : public MatchMaker {
 public:
  PerfectHashMatchMaker(const std::string& a, const std::string& b, int k) {
    // TODO(fpavetic): Move the work to the Create method.
    a_ = a;
    b_ = b;
    k_ = k;
    row_ = 0;
    PrepareAlphabet(a, b, char_to_id_, alphabet_size_);
    ahasher_.reset(new RollingHasher(a_, k_, char_to_id_, alphabet_size_));
    InitBMap(b);
  }

  bool GetNextMatches(std::vector<int>* matches) override;

 private:
  // This function determines the total number of
  // distinct characters in input strings a and b.
  // Outputs are: aid[character] = unique_character_id
  // alphabet_size = total number of distinct chars
  static void PrepareAlphabet(const std::string& a, const std::string& b,
                              std::vector<char>& aid, int& alphabet_size);

  // This method creates a mapping from hashes of length k
  // substrings of b to indices of those substrings. This
  // information gets stored in bmap_ member.
  void InitBMap(const std::string& b);

  std::string a_;
  std::string b_;
  int k_;
  int row_;

  std::vector<char> char_to_id_;
  int alphabet_size_;
  std::unique_ptr<RollingHasher> ahasher_;
  std::unordered_map<unsigned long long, std::vector<int>> bmap_;
};

#endif
