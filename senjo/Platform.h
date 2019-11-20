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

#ifndef SENJO_PLATFORM_H
#define SENJO_PLATFORM_H

#ifdef WIN32
#include <Windows.h>
#endif

#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <list>
#include <set>
#include <cassert>
#include <cstring>

namespace senjo {

//-----------------------------------------------------------------------------
typedef std::chrono::system_clock::time_point TimePoint;

//-----------------------------------------------------------------------------
inline TimePoint now() {
  return std::chrono::system_clock::now();
}

//-----------------------------------------------------------------------------
inline TimePoint maxTime() {
  return TimePoint::max();
}

//-----------------------------------------------------------------------------
inline TimePoint addMsecs(const TimePoint& begin, const uint64_t msecs) {
  return (begin + std::chrono::milliseconds(msecs));
}

//-----------------------------------------------------------------------------
inline uint64_t getMsecs(const TimePoint& begin, const TimePoint& end = now()) {
  auto duration = (end - begin);
  auto msecs = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
  return uint64_t(msecs.count());
}

//-----------------------------------------------------------------------------
template<typename T>
inline double average(const T total, const T count) {
  return (count != 0.0) ? (double(total) / double(count)) : 0;
}

//-----------------------------------------------------------------------------
template<typename T>
inline double rate(const T count, const T msecs) {
  return (msecs != 0.0) ? ((double(count) / double(msecs)) * 1000) : 0;
}

//-----------------------------------------------------------------------------
template<typename T>
inline double percent(const T top, const T bottom) {
  return bottom ? (100 * (double(top) / double(bottom))) : 0;
}

//-----------------------------------------------------------------------------
template<typename T>
inline T toNumber(const std::string& str, const T defaultValue = 0) {
  T number;
  return (std::stringstream(str) >> number) ? number : defaultValue;
}

//-----------------------------------------------------------------------------
inline std::string trimLeft(const std::string& str,
                            const std::string& chars = " ")
{
  size_t i = str.find_first_not_of(chars);
  return (i == std::string::npos) ? str : str.substr(i);
}

//-----------------------------------------------------------------------------
inline std::string trimRight(const std::string& str,
                             const std::string& chars = " ")
{
  size_t i = str.size();
  while ((i > 0) && (chars.find_first_of(str[i - 1]) != std::string::npos)) {
    --i;
  }
  return str.substr(0, i);
}

//-----------------------------------------------------------------------------
inline std::string trim(const std::string& str,
                        const std::string& chars = " ")
{
  return trimRight(trimLeft(str, chars), chars);
}

//-----------------------------------------------------------------------------
inline bool iEqual(const std::string& a, const std::string& b) {
  return ((a.size() == b.size()) &&
          std::equal(a.begin(), a.end(), b.begin(), [](char c1, char c2) {
    return (c1 == c2) || (std::toupper(c1) == std::toupper(c2));
  }));
}

//-----------------------------------------------------------------------------
inline bool isMove(const std::string& str) {
  return (str.size() >= 4) &&
         (str[0] >= 'a') && (str[0] <= 'h') &&
         (str[1] >= '1') && (str[1] <= '8') &&
         (str[2] >= 'a') && (str[2] <= 'h') &&
         (str[3] >= '1') && (str[3] <= '8') &&
         ((str.size() == 4) ||
          (str[4] == 'n') || (str[4] == 'b') ||
          (str[4] == 'r') || (str[4] == 'q'));
}

} // namespace senjo

#endif // SENJO_PLATFORM_H
