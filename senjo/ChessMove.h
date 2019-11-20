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

#ifndef SENJO_MOVE_H
#define SENJO_MOVE_H

#include "Square.h"

namespace senjo {

//-----------------------------------------------------------------------------
//! \brief Simple chess move representation
//-----------------------------------------------------------------------------
struct ChessMove {
  ChessMove() {
    clear();
  }

  void clear() {
    cap = 0;
    promo = 0;
    from = Square::None;
    to = Square::None;
  }

  std::string toString() const {
    std::string str;
    if (from.isValid() && to.isValid() && (from != to)) {
      str += from.toString();
      str += to.toString();
      if (promo) {
        str += static_cast<char>(tolower(promo));
      }
    }
    return str;
  }

  char   cap;
  char   promo;
  Square from;
  Square to;
};

} // namespace senjo

#endif // SENJO_MOVE_H
