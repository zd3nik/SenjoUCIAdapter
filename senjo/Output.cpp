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

#include "Output.h"

namespace senjo {

//-----------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------
std::mutex Output::_mutex;
TimePoint  Output::_lastOutput = now();

//-----------------------------------------------------------------------------
TimePoint Output::lastOutput() {
  return _lastOutput;
}

//-----------------------------------------------------------------------------
Output::Output(const OutputPrefix prefix) {
  _mutex.lock();
  switch (prefix) {
  case OutputPrefix::InfoPrefix:
    std::cout << "info string ";
    break;
  case OutputPrefix::NoPrefix:
    break;
  }
}

//-----------------------------------------------------------------------------
Output::~Output() {
  std::cout << '\n';
  std::cout.flush();
  _lastOutput = now();
  _mutex.unlock();
}

} // namespace senjo
