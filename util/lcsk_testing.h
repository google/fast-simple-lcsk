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

#ifndef LCSK_TESTING
#define LCSK_TESTING

#include <string>
#include <utility>
#include <vector>

// This is a slower sparse LCSk/LCSkpp calculation.
//
// If lcsk_reconstruction equals NULL, only the value of the metric is
// computed, without reconstructing it.
void LcskSparseSlow(const std::string &a, const std::string &b, const int k,
                    std::vector<std::pair<int, int> > *lcsk_reconstruction);
void LcskppSparseSlow(const std::string &a, const std::string &b, const int k,
                      std::vector<std::pair<int, int> > *lcsk_reconstruction);

// This function tests whether LCSk/LCSkpp has been reconstructed successfully.
bool ValidLcsk(const std::string &a, const std::string &b, const int k,
               const std::vector<std::pair<int, int> > &lcsk_recon);
bool ValidLcskpp(const std::string &a, const std::string &b, const int k,
                 const std::vector<std::pair<int, int> > &lcsk_recon);

// Slow calculation of LCSk, for testing purposes.
void LcskSlow(const std::string &a, const std::string &b, const int K,
              int *lcsk_length);
void LcskppSlow(const std::string &a, const std::string &b, const int K,
                int *lcsk_length);

#endif
