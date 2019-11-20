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

#ifndef SENJO_SQUARE_H
#define SENJO_SQUARE_H

#include "Platform.h"

namespace senjo {

//-----------------------------------------------------------------------------
//! \brief 0x88 board direction deltas
//-----------------------------------------------------------------------------
enum Direction {
  KnightMove1 = -33,
  KnightMove2 = -31,
  KnightMove3 = -18,
  SouthWest   = -17,
  South       = -16,
  SouthEast   = -15,
  KnightMove4 = -14,
  West        =  -1,
  Unknown     =   0,
  East        =   1,
  KnightMove5 =  14,
  NorthWest   =  15,
  North       =  16,
  NorthEast   =  17,
  KnightMove6 =  18,
  KnightMove7 =  31,
  KnightMove8 =  33
};

//-----------------------------------------------------------------------------
//! \brief Simple chessboard square representation
//-----------------------------------------------------------------------------
class Square {
public:
  enum SquareName {
    A1 = 0x00, B1, C1, D1, E1, F1, G1, H1,
    A2 = 0x10, B2, C2, D2, E2, F2, G2, H2,
    A3 = 0x20, B3, C3, D3, E3, F3, G3, H3,
    A4 = 0x30, B4, C4, D4, E4, F4, G4, H4,
    A5 = 0x40, B5, C5, D5, E5, F5, G5, H5,
    A6 = 0x50, B6, C6, D6, E6, F6, G6, H6,
    A7 = 0x60, B7, C7, D7, E7, F7, G7, H7,
    A8 = 0x70, B8, C8, D8, E8, F8, G8, H8,
    None
  };

  Square(const int name = None) : sqr((name & ~0x77) ? None : name) { }
  Square(const Square& other) : sqr(other.sqr) { }
  Square(const int x, const int y) { assign(x, y); }
  Square& operator=(const int name) { return assign(name); }
  Square& operator=(const Square& other) { return assign(other); }
  Square& assign(const int x, const int y) {
    if ((x >= 0) && (x < 8) && (y >= 0) && (y < 8)) {
      sqr = ((y * 16) + x);
    }
    else {
      sqr = None;
    }
    return *this;
  }
  Square& assign(const int name) {
    sqr = (name & ~0x77) ? None : name;
    return *this;
  }
  Square& assign(const Square& other) {
    sqr = other.sqr;
    return *this;
  }
  Square& operator++() {
    if (sqr == H8) {
      sqr = None;
    }
    else if (isValid()) {
      if ((sqr % 16) == 7) {
        sqr += 9;
      }
      else {
        sqr++;
      }
    }
    return *this;
  }
  Square& operator--() {
    if (sqr == A1) {
      sqr = None;
    }
    else if (isValid()) {
      if ((sqr % 16) == 0) {
        sqr -= 9;
      }
      else {
        sqr--;
      }
    }
    return *this;
  }
  Square& operator+=(const Direction direction) {
    if (isValid()) {
      if ((sqr += direction) & ~0x77) sqr = None;
    }
    return *this;
  }
  Square& operator-=(const Direction direction) {
    if (isValid()) {
      if ((sqr -= direction) & ~0x77) sqr = None;
    }
    return *this;
  }
  Square operator+(const Direction direction) const {
    return isValid() ? Square(sqr + direction) : Square();
  }
  Square operator-(const Direction direction) const {
    return isValid() ? Square(sqr - direction) : Square();
  }
  Direction directionTo(const Square& destination) const {
    static const Direction DIRECTION[] = {
      SouthWest,Unknown,Unknown,Unknown,Unknown,Unknown,Unknown,South,
      Unknown,Unknown,Unknown,Unknown,Unknown,Unknown,SouthEast,Unknown,
      Unknown,SouthWest,Unknown,Unknown,Unknown,Unknown,Unknown,South,
      Unknown,Unknown,Unknown,Unknown,Unknown,SouthEast,Unknown,Unknown,
      Unknown,Unknown,SouthWest,Unknown,Unknown,Unknown,Unknown,South,
      Unknown,Unknown,Unknown,Unknown,SouthEast,Unknown,Unknown,Unknown,
      Unknown,Unknown,Unknown,SouthWest,Unknown,Unknown,Unknown,South,
      Unknown,Unknown,Unknown,SouthEast,Unknown,Unknown,Unknown,Unknown,
      Unknown,Unknown,Unknown,Unknown,SouthWest,Unknown,Unknown,South,
      Unknown,Unknown,SouthEast,Unknown,Unknown,Unknown,Unknown,Unknown,
      Unknown,Unknown,Unknown,Unknown,Unknown,SouthWest,KnightMove1,South,
      KnightMove2,SouthEast,Unknown,Unknown,Unknown,Unknown,Unknown,Unknown,
      Unknown,Unknown,Unknown,Unknown,Unknown,KnightMove3,SouthWest,South,
      SouthEast,KnightMove4,Unknown,Unknown,Unknown,Unknown,Unknown,Unknown,
      West,West,West,West,West,West,West,Unknown,
      East,East,East,East,East,East,East,Unknown,
      Unknown,Unknown,Unknown,Unknown,Unknown,KnightMove5,NorthWest,North,
      NorthEast,KnightMove6,Unknown,Unknown,Unknown,Unknown,Unknown,Unknown,
      Unknown,Unknown,Unknown,Unknown,Unknown,NorthWest,KnightMove7,North,
      KnightMove8,NorthEast,Unknown,Unknown,Unknown,Unknown,Unknown,Unknown,
      Unknown,Unknown,Unknown,Unknown,NorthWest,Unknown,Unknown,North,
      Unknown,Unknown,NorthEast,Unknown,Unknown,Unknown,Unknown,Unknown,
      Unknown,Unknown,Unknown,NorthWest,Unknown,Unknown,Unknown,North,
      Unknown,Unknown,Unknown,NorthEast,Unknown,Unknown,Unknown,Unknown,
      Unknown,Unknown,NorthWest,Unknown,Unknown,Unknown,Unknown,North,
      Unknown,Unknown,Unknown,Unknown,NorthEast,Unknown,Unknown,Unknown,
      Unknown,NorthWest,Unknown,Unknown,Unknown,Unknown,Unknown,North,
      Unknown,Unknown,Unknown,Unknown,Unknown,NorthEast,Unknown,Unknown,
      NorthWest,Unknown,Unknown,Unknown,Unknown,Unknown,Unknown,North,
      Unknown,Unknown,Unknown,Unknown,Unknown,Unknown,NorthEast,Unknown
    };
    if (isValid() && destination.isValid()) {
      return DIRECTION[0x77 + destination.sqr - sqr];
    }
    return Unknown;
  }
  int distanceTo(const Square& destination) const {
    static const int DISTANCE[] = {
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,7,6,6,6,6,6,6,6,6,6,6,6,6,6,7,8,
      7,6,5,5,5,5,5,5,5,5,5,5,5,6,7,8,7,6,5,4,4,4,4,4,4,4,4,4,5,6,7,8,
      7,6,5,4,3,3,3,3,3,3,3,4,5,6,7,8,7,6,5,4,3,2,2,2,2,2,3,4,5,6,7,8,
      7,6,5,4,3,2,1,1,1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,
      7,6,5,4,3,2,1,1,1,2,3,4,5,6,7,8,7,6,5,4,3,2,2,2,2,2,3,4,5,6,7,8,
      7,6,5,4,3,3,3,3,3,3,3,4,5,6,7,8,7,6,5,4,4,4,4,4,4,4,4,4,5,6,7,8,
      7,6,5,5,5,5,5,5,5,5,5,5,5,6,7,8,7,6,6,6,6,6,6,6,6,6,6,6,6,6,7,8,
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8
    };
    if (isValid() && destination.isValid()) {
      return DISTANCE[0x77 + destination.sqr - sqr];
    }
    return 8;
  }
  int value() const                            { return sqr; }
  bool isValid() const                         { return !(sqr & ~0x77); }
  bool operator!() const                       { return (sqr & ~0x77); }
  bool operator!=(const Square& other) const   { return (sqr != other.sqr); }
  bool operator<(const Square& other) const    { return (sqr < other.sqr); }
  bool operator==(const Square& other) const   { return (sqr == other.sqr); }
  bool operator==(const SquareName name) const { return (sqr == name); }
  bool operator>(const Square& other) const    { return (sqr > other.sqr); }
  int x() const { return isValid() ? (sqr & 0xF) : -1; }
  int y() const { return isValid() ? (sqr / 16) : -1; }
  std::string toString() const {
    std::string str;
    if (isValid()) {
      str += static_cast<char>('a' + (sqr % 16));
      str += static_cast<char>('1' + (sqr / 16));
    }
    return str;
  }

private:
  int sqr;
};

} // namespace senjo

#endif // SENJO_SQUARE_H
