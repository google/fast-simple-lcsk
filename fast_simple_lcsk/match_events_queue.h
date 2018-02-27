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

#ifndef MATCH_EVENTS_QUEUE
#define MATCH_EVENTS_QUEUE

#include <queue>
#include <tuple>
#include <utility>
#include "match_pair.h"

struct MatchEventsQueue {
  std::queue<std::tuple<int, int, std::shared_ptr<MatchPair>>> begin;
  std::queue<std::tuple<int, int, std::shared_ptr<MatchPair>>> end;

  void AddBegin(const std::tuple<int, int, std::shared_ptr<MatchPair>>& event) {
    begin.push(event);
  }
  void AddEnd(const std::tuple<int, int, std::shared_ptr<MatchPair>>& event) {
    end.push(event);
  }

  bool PopBegin(int row, std::tuple<int, int, std::shared_ptr<MatchPair>>* event) {
    if (!begin.empty() && std::get<0>(begin.front()) == row) {
      *event = begin.front();
      begin.pop();
      return true;
    }
    return false;
  }

  bool PopEnd(int row, std::tuple<int, int, std::shared_ptr<MatchPair>>* event) {
    if (!end.empty() && std::get<0>(end.front()) == row) {
      *event = end.front();
      end.pop();
      return true;
    }
    return false;
  }
};

#endif
