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

#ifndef ROLLING_HASHER
#define ROLLING_HASHER

#include <string>
#include <vector>

class RollingHasher {
 public:
  RollingHasher(const std::string& s, int k,
                const std::vector<char>& char_to_id, int alphabet_size)
      : s_(s),
        k_(k),
        char_to_id_(char_to_id),
        alphabet_size_(alphabet_size),
        col_(0) {
    hash_mod_ = 1;
    for (int i = 0; i < k; ++i) {
      hash_mod_ *= alphabet_size;
    }
  }

  // TODO(fpavetic): Docs.
  bool Next(unsigned long long* hash);

 private:
  const std::string& s_;
  int k_;
  const std::vector<char>& char_to_id_;
  int alphabet_size_;

  unsigned long long hash_mod_;
  unsigned long long hash_;
  int col_;
};

#endif  // ROLLING_HASHER
