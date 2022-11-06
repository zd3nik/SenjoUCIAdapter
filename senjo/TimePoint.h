//-----------------------------------------------------------------------------
// Copyright (c) 2022 Shawn Chidester <zd3nik@gmail.com>
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

#ifndef TIMEPOINT_H
#define TIMEPOINT_H

#include <chrono>

namespace senjo {

//-----------------------------------------------------------------------------
//! @brief A representation of an instant in time
//! This is a wrapper around the standard C++ chrono system_clock time_point.
//-----------------------------------------------------------------------------
class TimePoint {
public:
    //-------------------------------------------------------------------------
    //! @brief Constructor
    //! @param tp the chrono system_clock time_point value
    //! @remark The current time is constructed if no time point given
    //-------------------------------------------------------------------------
    TimePoint(std::chrono::system_clock::time_point instant = std::chrono::system_clock::now())
        : instant(instant)
    {}

    //-------------------------------------------------------------------------
    //! @brief Get the maximum possible TimePoint value
    //! @return the maximum possible TimePoint value
    //-------------------------------------------------------------------------
    static TimePoint maxTime() {
      return std::chrono::system_clock::time_point::max();
    }

    //-------------------------------------------------------------------------
    //! @brief Cast this TimePoint to std::chrono::system_clock::time_point
    //-------------------------------------------------------------------------
    operator std::chrono::system_clock::time_point() const {
        return instant;
    }

    //-------------------------------------------------------------------------
    //! @brief Get a new TimePoint which equals this TimePoint plus some milliseconds
    //! @param msecs the number of milliseconds to add to this TimePoint
    //! @return a new TimePoint which equals this TimePoint plus some milliseconds
    //-------------------------------------------------------------------------
    TimePoint addMsecs(const uint64_t msecs) const {
      return (instant + std::chrono::milliseconds(msecs));
    }

    //-------------------------------------------------------------------------
    //! @brief Get the number of milliseconds between this TimePoint and another TimePoint
    //! @param other a TimePoint to compare with this TimePoint
    //! @return the number of milliseconds between this TimePoint and another TimePoint
    //-------------------------------------------------------------------------
    uint64_t msecsUntil(const TimePoint& other) const {
      auto duration = (other.instant - instant);
      auto msecs = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
      return uint64_t(msecs.count());
    }

private:
    std::chrono::system_clock::time_point instant;
};

} // namespace senjo

#endif // TIMEPOINT_H
