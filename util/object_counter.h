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

#ifndef OBJECT_COUNTER
#define OBJECT_COUNTER

template <typename T>
struct ObjectCounter {
    ObjectCounter() {
        objects_created++;
        objects_alive++;
        max_objects_alive = std::max(max_objects_alive, objects_alive);
    }

    virtual ~ObjectCounter() {
        --objects_alive;
    }

    static uint64_t objects_created;
    static uint64_t objects_alive;
    static uint64_t max_objects_alive;
};

template <typename T> uint64_t ObjectCounter<T>::objects_created(0);
template <typename T> uint64_t ObjectCounter<T>::objects_alive(0);
template <typename T> uint64_t ObjectCounter<T>::max_objects_alive(0);


#endif
