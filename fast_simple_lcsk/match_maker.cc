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

#include "match_maker.h"

using namespace std;

// static
std::unique_ptr<MatchMaker> MatchMaker::Create(const string& a, const string& b,
                                               int k, MatchMakerType type) {
  std::unique_ptr<MatchMaker> match_maker;
  switch (type) {
    case MatchMakerType::NAIVE:
      match_maker.reset(new NaiveMatchMaker(a, b, k));
    case MatchMakerType::PERFECT_HASH:
      match_maker.reset(new PerfectHashMatchMaker(a, b, k));
  }
  return match_maker;
}

bool NaiveMatchMaker::GetNextMatches(std::vector<int>* matches) {
  matches->clear();
  // Are there more matches to generate?
  if (row_ + k_ > a_.size()) return false;

  for (int b_index = 0; b_index <= (int)b_.size() - k_; ++b_index) {
    if (a_.substr(row_, k_) == b_.substr(b_index, k_)) {
      matches->push_back(b_index);
    }
  }

  ++row_;  // Not forgetting to update this!
  return true;
}

bool PerfectHashMatchMaker::GetNextMatches(std::vector<int>* matches) {
  matches->clear();
  unsigned long long hash = 0;

  // Are there more matches to generate?
  if (row_ + k_ > a_.size()) {
    assert(!ahasher_->Next(&hash));
    return false;
  }

  assert(ahasher_->Next(&hash));
  for (int x : bmap_[hash]) {
    matches->push_back(x);
  }

  ++row_;  // Not forgetting to update this!
  return true;
}

// static
void PerfectHashMatchMaker::PrepareAlphabet(const std::string& a,
                                            const std::string& b,
                                            std::vector<char>& aid,
                                            int& alphabet_size) {
  aid = std::vector<char>(256, -1);
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

void PerfectHashMatchMaker::InitBMap(const std::string& b) {
  bmap_.clear();
  RollingHasher bhasher_(b_, k_, char_to_id_, alphabet_size_);
  unsigned long long hash = 0;
  for (int i = 0; i + k_ <= b.size(); ++i) {
    assert(bhasher_.Next(&hash));
    bmap_[hash].push_back(i);
  }
  assert(!bhasher_.Next(&hash));
}
