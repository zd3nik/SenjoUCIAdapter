//----------------------------------------------------------------------------
// Copyright (c) 2015 Shawn Chidester <zd3nik@gmail.com>
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
//----------------------------------------------------------------------------

#ifndef SENJO_PLATFORM_H
#define SENJO_PLATFORM_H

#define MAKE_XSTR(x) MAKE_STR(x)
#define MAKE_STR(x) #x
#ifndef GIT_REV
#define GIT_REV norev
#endif

#ifdef WIN32
#include <Windows.h>
#include <mmintrin.h>
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
#pragma warning(disable:4146)
#pragma warning(disable:4800)
#pragma warning(disable:4806)
#pragma warning(disable:4996)
#define PRId64 "lld"
#define PRIu64 "llu"
#define snprintf _snprintf
#else // ifdef WIN32
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#endif

#include <algorithm>
#include <assert.h>
#include <cstdint>
#include <errno.h>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <time.h>

namespace senjo
{

//----------------------------------------------------------------------------
//! \brief Get current millisecond timestamp
//! \return Number of milliseconds since epoch
//----------------------------------------------------------------------------
static inline uint64_t Now()
{
#ifdef WIN32
  return static_cast<uint64_t>(GetTickCount());
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<uint64_t>((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
#endif
}

static inline bool MillisecondSleep(const unsigned int msecs)
{
#ifdef WIN32
  SetLastError(0);
  Sleep(msecs);
  return GetLastError() ? false : true;
#else
  return usleep(1000 * msecs) ? false : true;
#endif
}

//----------------------------------------------------------------------------
//! \brief Convert unsigned 64-bit integer to double
//! Disables compiler warning whe using windows compiler
//! \param[in] value Unsigned 64-bit integer
//! \return \p value converted to double
//----------------------------------------------------------------------------
static inline double ToDouble(const uint64_t value)
{
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4244)
#endif
  return static_cast<double>(value);
#ifdef WIN32
#pragma warning(pop)
#endif
}

//----------------------------------------------------------------------------
//! \brief Convert millisecond timestamp to seconds as double
//! \param[in] msecs Millisecond timestamp
//! \return \p msecs converted to seconds as double
//----------------------------------------------------------------------------
static inline double ToSeconds(const uint64_t msecs)
{
  return (ToDouble(msecs) / 1000.0);
}

//----------------------------------------------------------------------------
//! \brief Calculate average from two doubles
//! \param[in] total The numerator
//! \param[in] count The denominator
//! \return \p total divided by \p count as double
//----------------------------------------------------------------------------
static inline double Average(const double total, const double count)
{
  return (count ? (total / count) : 0);
}

//----------------------------------------------------------------------------
//! \brief Calculate average from two unsigned 64-bit integers
//! \param[in] total The numerator
//! \param[in] count The denominator
//! \return \p total divided by \p count as double
//----------------------------------------------------------------------------
static inline double Average(const uint64_t total, const uint64_t count)
{
  return (count ? (ToDouble(total) / ToDouble(count)) : 0);
}

//----------------------------------------------------------------------------
//! \brief Calculate average from two native integers
//! \param[in] total The numerator
//! \param[in] count The denominator
//! \return \p total divided by \p count as double
//----------------------------------------------------------------------------
static inline double Average(const int total, const int count)
{
  return (count ? (ToDouble(total) / ToDouble(count)) : 0);
}

//----------------------------------------------------------------------------
//! \brief Calculate K-rate from two doubles
//! \param[in] count The number of items
//! \param[in] msecs The number of milliseconds spent
//! \return \p count per seconds as double
//----------------------------------------------------------------------------
static inline double Rate(const double count, const double msecs)
{
  return (msecs ? ((count / msecs) * 1000) : 0);
}

//----------------------------------------------------------------------------
//! \brief Calculate K-rate from two unsigned 64-bit integers
//! \param[in] count The number of items
//! \param[in] msecs The number of milliseconds spent
//! \return \p count per seconds as double
//----------------------------------------------------------------------------
static inline double Rate(const uint64_t count, const uint64_t msecs)
{
  return Rate(ToDouble(count), ToDouble(msecs));
}

//----------------------------------------------------------------------------
//! \brief Calculate K-rate from two native integers
//! \param[in] count The number of items
//! \param[in] msecs The number of milliseconds spent
//! \return \p count per seconds as double
//----------------------------------------------------------------------------
static inline double Rate(const int count, const int msecs)
{
  return Rate(static_cast<double>(count), static_cast<double>(msecs));
}

//----------------------------------------------------------------------------
//! \brief Calculate percentage from two doubles
//! \param[in] top The numerator
//! \param[in] bottom The denominator
//! \return \p top divided by \p bottom multiplied by 100 as double
//----------------------------------------------------------------------------
static inline double Percent(const double top, const double bottom)
{
  return (bottom ? (100 * (top / bottom)) : 0);
}

//----------------------------------------------------------------------------
//! \brief Calculate percentage from two unsigned 64-bit integers
//! \param[in] top The numerator
//! \param[in] bottom The denominator
//! \return \p top divided by \p bottom multiplied by 100 as double
//----------------------------------------------------------------------------
static inline double Percent(const uint64_t top, const uint64_t bottom)
{
  return (bottom ? (100 * (ToDouble(top) / ToDouble(bottom))) : 0);
}

//----------------------------------------------------------------------------
//! \brief Calculate percentage from two native integers
//! \param[in] top The numerator
//! \param[in] bottom The denominator
//! \return \p top divided by \p bottom multiplied by 100 as double
//----------------------------------------------------------------------------
static inline double Percent(const int top, const int bottom)
{
  return (bottom ? (100 * (static_cast<double>(top) / bottom)) : 0);
}

//----------------------------------------------------------------------------
//! \brief Move \p position to beginning of next word
//! \param[in,out] position Pointer to position within a NULL terminated string
//! \return Updated \p position value
//----------------------------------------------------------------------------
static inline const char* NextWord(const char*& position)
{
  while (position && *position && isspace(*position)) {
    ++position;
  }
  return position;
}

//----------------------------------------------------------------------------
//! \brief Move \p position to beginning of next word
//! \param[in,out] position Pointer to position within a NULL terminated string
//! \return Updated \p position value
//----------------------------------------------------------------------------
static inline char* NextWord(char*& position)
{
  while (position && *position && isspace(*position)) {
    ++position;
  }
  return position;
}

//----------------------------------------------------------------------------
//! \brief Move \p position to beginning of next whitespace segment
//! \param[in,out] position Pointer to position within a NULL terminated string
//! \return Updated \p position value
//----------------------------------------------------------------------------
static inline const char* NextSpace(const char*& position)
{
  while (position && *position && !isspace(*position)) {
    ++position;
  }
  return position;
}

//----------------------------------------------------------------------------
//! \brief Move \p position to beginning of next whitespace segment
//! \param[in,out] position Pointer to position within a NULL terminated string
//! \return Updated \p position value
//----------------------------------------------------------------------------
static inline char* NextSpace(char*& position)
{
  while (position && *position && !isspace(*position)) {
    ++position;
  }
  return position;
}

//----------------------------------------------------------------------------
//! \brief Move \p position to last character of current word
//! \param[in,out] position Pointer to position within a NULL terminated string
//! \return Updated \p position value
//----------------------------------------------------------------------------
static inline char* WordEnd(char*& position)
{
  while (position && *position && position[1] && !isspace(position[1])) {
    ++position;
  }
  return position;
}

//----------------------------------------------------------------------------
//! \brief Prepare a string for parsing
//! All whitespace in \p str is converted to spaces.
//! The first new-line or carriage-return found is converted to NULL terminator.
//! \param[in,out] str The string, updated to start at first word
//! \return Updated \p str value
//----------------------------------------------------------------------------
static inline char* NormalizeString(char*& str)
{
  for (char* n = str; n && *n; ++n) {
    if ((*n == '\n') || (*n == '\r')) {
      *n = 0;
      break;
    }
    if ((*n != ' ') && isspace(*n)) {
      *n = ' ';
    }
  }
  return NextWord(str);
}

//----------------------------------------------------------------------------
//! \brief Does the given parameter name match the current word in a string?
//! \param[in] name The parameter name to check for
//! \param[in,out] params Current word, updated to next word if matches
//! \return true if current word in \p params matches \p name
//----------------------------------------------------------------------------
static inline bool ParamMatch(const std::string& name, const char*& params)
{
  if (name.empty() || !params) {
    return false;
  }
  if (strncmp(params, name.c_str(), name.size()) ||
      (params[name.size()] && !isspace(params[name.size()])))
  {
    return false;
  }
  NextWord(params += name.size());
  return true;
}

//----------------------------------------------------------------------------
//! \brief Does the given parameter name exist at the current word in a string?
//! \param[in] name The parameter name to check for
//! \param[out] exists Set to true if current word matches \p name
//! \param[in,out] params The current word in string of parameters
//! \return true if current word in \p params matches \p name
//----------------------------------------------------------------------------
static inline bool HasParam(const std::string& name, bool& exists,
                            const char*& params)
{
  if (!ParamMatch(name, params)) {
    return false;
  }
  exists = true;
  return true;
}

//----------------------------------------------------------------------------
//! \brief Does 'name value' pattern exist at current position in a string?
//! \param[in] name The parameter name to check for
//! \param[out] value Set to word after \p name if current word matches \p name
//! \param[in,out] params The current word in string of parameters
//! \param[out] invalid Set to true if \p name is not followed by a value
//! \return true if current word in \p params matches \p name and has value
//----------------------------------------------------------------------------
static inline bool StringParam(const std::string& name, std::string& value,
                               const char*& params, bool& invalid)
{
  if (!ParamMatch(name, params)) {
    return false;
  }
  if (!params || !*params) {
    invalid = true;
  }
  else {
    const char* begin = params;
    NextSpace(params);
    value.assign(begin, (params - begin));
    NextWord(params);
  }
  return true;
}

//----------------------------------------------------------------------------
//! \brief Does 'name number' pattern exist at current position in a string?
//! \param[in] name The parameter name to check for
//! \param[out] value Set to number after \p name if current word matches \p name
//! \param[in,out] params The current word in string of parameters
//! \param[out] invalid Set to true if \p name is not followed by a number
//! \return true if current word in \p params matches \p name and has value
//----------------------------------------------------------------------------
template<typename Number>
static inline bool NumberParam(const std::string& name, Number& value,
                               const char*& params, bool& invalid)
{
  if (!ParamMatch(name, params)) {
    return false;
  }
  if (!params || !*params || !isdigit(*params)) {
    invalid = true;
  }
  else {
    value = (*params++ - '0');
    while (*params && isdigit(*params)) {
      value = ((10 * value) + (*params++ - '0'));
    }
    if (*params && !isspace(*params)) {
      invalid = true;
    }
    else {
      NextWord(params);
    }
  }
  return true;
}

static inline bool ParamValue(std::string& value, const char*& params,
                              const std::string& next)
{
  if (params && *NextWord(params)) {
    const char* end = next.size() ? strstr(params, next.c_str()) : NULL;
    while (end && end[next.size()] && !isspace(end[next.size()])) {
      NextSpace(end += next.size());
      end = strstr(end, next.c_str());
    }
    if (!end) {
      end = (params + strlen(params));
    }
    while ((end > params) && isspace(end[-1])) {
      --end;
    }
    value.assign(params, (end - params));
    if (value.size()) {
      NextWord(params += value.size());
      return true;
    }
  }
  return false;
}

} // namespace senjo

#endif // SENJO_PLATFORM_H
