/**
 * Copyright 2004-present, Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RingBuffer.h"

#include <fb/log.h>

#include <profilo/logger/lfrb/LockFreeRingBuffer.h>
#include <profilo/mmapbuf/Buffer.h>
#include <atomic>
#include <memory>
#include <stdexcept>

namespace facebook {
namespace profilo {

namespace {

mmapbuf::Buffer noop_buffer{1};
std::atomic<mmapbuf::Buffer*> buffer(&noop_buffer);

mmapbuf::Buffer& getBuffer() {
  return *buffer.load();
}

void initBuffer(mmapbuf::Buffer& newBuffer) {
  buffer.store(&newBuffer);
}

} // namespace

void RingBuffer::init(mmapbuf::Buffer& newBuffer) {
  initBuffer(newBuffer);
}

void RingBuffer::destroy() {
  if (buffer.load() == &noop_buffer) {
    return;
  }

  auto expected = buffer.load();
  if (buffer.compare_exchange_strong(expected, &noop_buffer)) {
    delete expected;
  }
}

mmapbuf::Buffer& RingBuffer::get() {
  return getBuffer();
}

} // namespace profilo
} // namespace facebook
