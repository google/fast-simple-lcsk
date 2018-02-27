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

#ifndef MATCH_PAIR
#define MATCH_PAIR

#include <memory>
#include "../util/object_counter.h"

struct MatchPair : ObjectCounter<MatchPair> {
  // Needed only for the reconstruction.
  int end_row;
  // Needed during computation and reconstruction.
  int end_col;
  // Needed only for the computation.
  int dp;
  // Pointer to the previous match, used for reconstruction.
  std::shared_ptr<MatchPair> prev;

  MatchPair() { }

  MatchPair(int end_row, int end_col, int dp, std::shared_ptr<MatchPair> prev)
      : end_row(end_row), end_col(end_col), dp(dp), prev(prev) { }
};

#endif
