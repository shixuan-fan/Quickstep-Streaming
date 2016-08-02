/**
 *   Copyright 2011-2015 Quickstep Technologies LLC.
 *   Copyright 2015 Pivotal Software, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 **/

#include "threading/Thread.hpp"

#include <cstddef>

namespace quickstep {

ThreadInterface::~ThreadInterface() {}

namespace threading_internal {
void executeRunMethodForThreadReturnNothing(void *thread_ptr) {
  static_cast<ThreadInterface*>(thread_ptr)->run();
}

void *executeRunMethodForThreadReturnNull(void *thread_ptr) {
  static_cast<ThreadInterface*>(thread_ptr)->run();
  return NULL;
}
}  // namespace threading_internal

}  // namespace quickstep
