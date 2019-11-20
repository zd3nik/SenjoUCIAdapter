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

#ifndef SENJO_OUTPUT_H
#define SENJO_OUTPUT_H

#include "Platform.h"
#include <iostream>
#include <mutex>

namespace senjo {

//-----------------------------------------------------------------------------
//! \brief Thread safe stdout stream
//! Instantiating this class will obtain a lock on a mtuex guarding stdout.
//! The stdout mutex is released when the instantiated object is destroyed.
//!
//! \e Important: '\n' is automatically appended when the object is destroyed.
//! \e Important: The UCI protocol requires that lines end with a single
//! new-line character: '\n'.  So you should *not* use std::endl, as this will
//! output '\r\n' on windows systems and '\r' on some other systems.
//!
//! Example:
//!
//!   Output() << "Hello, world!";
//!
//! Notice it is not necessary to add '\n' on the end.
//!
//! Example of multiple line outout:
//!
//!   Output() << "Line 1";
//!   Output() << "Line 2";
//!
//! Notice it is not necessary to add '\n' on the end of either line.
//! Notice it is possible that other threads could output something between
//! "Line 1" and "Line 2".
//!
//! Example of multiple line outout without allowing another thread to output
//! between each line:
//!
//!   Output() << "Line 1\n"
//!            << "info string Line 2";
//! or
//!   Output() << "Line 1\ninfo string Line 2";
//! or
//!   Output() << "Line " << 1 << '\n' << "info string " << "Line " << 2;
//! etc..
//!
//! Notice each case uses a single Output() instance.
//! Notice it is not necessary to add '\n' after the last line.
//! Notice it is necessary to explicitly prefix all but the first line with
//! "info string ".  If you know what you're doing concerning the UCI protocol
//! you can omit "info string " where appropriate.
//!
//! To maintain exclusive access to stdout so that processing can be done
//! between lines of output without allowing another thread to output between
//! lines:
//!
//!   {
//!     Output out;
//!     out << "line 1\n";
//!     ... do some processing ...
//!     out << "info string line 2";
//!   }
//!
//! Notice the code is enclosed in { } to clearly define the scope of 'out'.
//! Notice it is not necessary to add  '\n' after the last line.
//! Notice it is necessary to explicitly prefix all but the first line with
//! "info string ".  If you know what you're doing concerning the UCI protocol
//! you can omit "info string " where appropriate.
//-----------------------------------------------------------------------------
class Output {
public:
  enum OutputPrefix {
    NoPrefix,   ///< Don't prefix output with "info string "
    InfoPrefix  ///< Prefix output with "info string "
  };

  //--------------------------------------------------------------------------
  //! \brief Constructor
  //! If \p param is InfoPrefix then output is prefixed with "info string ".
  //! \param[in] prefix NoPrefix or InfoPrefix (InfoPrefix is the default)
  //--------------------------------------------------------------------------
  Output(const OutputPrefix prefix = InfoPrefix);

  //--------------------------------------------------------------------------
  //! \brief Destructor
  //--------------------------------------------------------------------------
  virtual ~Output();

  //--------------------------------------------------------------------------
  //! \brief Get timestamp of the last time an Output class was destroyed
  //! \return Timestamp of last Output class destruction, 0 if none
  //--------------------------------------------------------------------------
  static TimePoint lastOutput();

  //--------------------------------------------------------------------------
  //! \brief Insertion operator
  //! All data types supported by std::cout are supported here.
  //! \return Reference to self.
  //--------------------------------------------------------------------------
  template<typename T>
  Output& operator<<(const T& x) {
    std::cout << x;
    return *this;
  }

private:
  static std::mutex _mutex;
  static TimePoint _lastOutput;
};

} // namespace senjo

#endif // SENJO_OUTPUT_H
