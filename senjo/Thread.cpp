//-----------------------------------------------------------------------------
// Copyright (c) 2015-2019 Shawn Chidester <zd3nik@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "Thread.h"
#include "Output.h"

namespace senjo {

//-----------------------------------------------------------------------------
Thread::Thread(int id)
  : id(id)
{}

//-----------------------------------------------------------------------------
Thread::~Thread() {
  waitForFinish();
}

//-----------------------------------------------------------------------------
bool Thread::run() {
  std::lock_guard<std::mutex> lock(mutex);
  if (thread && thread->joinable()) {
    return false;
  }

  thread.reset(new std::thread(Thread::staticRun, this));
  return true;
}

//-----------------------------------------------------------------------------
bool Thread::isRunning() {
  std::lock_guard<std::mutex> lock(mutex);
  return thread && thread->joinable();
}

//-----------------------------------------------------------------------------
void Thread::waitForFinish() {
  std::lock_guard<std::mutex> lock(mutex);
  if (thread && thread->joinable()) {
    thread->join();
  }
}

//-----------------------------------------------------------------------------
void Thread::staticRun(Thread* thread) {
  if (thread) {
    try {
      thread->doWork();
    } catch (const std::exception& ex) {
      Output() << "ERROR: Thread(" << thread->getId() << ") " << ex.what();
    } catch (...) {
      Output() << "ERROR: Thread(" << thread->getId() << ") unhandled exception";
    }
  }
}

} // namespace senjo
