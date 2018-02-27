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

#include "rolling_hasher.h"

bool RollingHasher::Next(unsigned long long* hash) {
  if (col_ + k_ > s_.size()) {
    return false;
  }

  if (col_ == 0) {
    hash_ = 0;
    for (int i = 0; i < k_ - 1; ++i) {
      hash_ = hash_ * alphabet_size_ + char_to_id_[s_[i]];
    }
  }

  hash_ = hash_ * alphabet_size_ + char_to_id_[s_[col_ + k_ - 1]];
  hash_ %= hash_mod_;
  *hash = hash_;
  ++col_;  // Not forgetting to update this!
  return true;
}
