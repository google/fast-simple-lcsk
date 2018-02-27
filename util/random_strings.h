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
#include <cstdlib>
#include <string>

const std::string kNuc = "ACTG";

// Selecting a character uniformily.
char get_random_base() { return kNuc[rand() % 4]; }

// This function returns a string which is a copy
// the input string, but character on every position
// is mutated to another one from the alphabet
std::string generate_similar(const std::string &a, const double &p_err) {
  std::string b = a;
  for (int i = 0; i < b.size(); ++i) {
    if (1.0 * rand() / RAND_MAX <= p_err) {
      b[i] = get_random_base();
    }
  }
  return b;
}

// This generates a random string of length <len>.
std::string generate_string(const int &len) {
  std::string ret;
  for (int i = 0; i < len; ++i) {
    ret += get_random_base();
  }
  assert(ret.size() == len);
  return ret;
}
