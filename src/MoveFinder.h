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

#ifndef SENJO_MOVE_FINDER_H
#define SENJO_MOVE_FINDER_H

#include "ChessMove.h"

namespace senjo {

//----------------------------------------------------------------------------
//! \brief Psueod-legal position loader and move generator
//! Used for translating algebraic moves to coordinates.
//----------------------------------------------------------------------------
class MoveFinder
{
public:
  //--------------------------------------------------------------------------
  //! \brief Is the given character a FEN piece letter?
  //! Supported piece letters are PNBRQK for white pieces, pnbrqk for black.
  //! \param[in] ch The character to test
  //! \return true if \p ch is a supported piece character
  //--------------------------------------------------------------------------
  static bool IsPiece(const char ch);

  //--------------------------------------------------------------------------
  //! \brief Load board position from given FEN string
  //! \param[in] fen The FEN string
  //! \return true if \P fen is valid
  //--------------------------------------------------------------------------
  bool LoadFEN(const char* fen);

  //--------------------------------------------------------------------------
  //! \brief Convert given move string into coordinate notation
  //! Coordinate notation is always 4 or 5 characters long and consists of:
  //!   source square (2 bytes)
  //!   destination square (2 bytes)
  //!   optional promotion piece type (1 byte)
  //! Examples: "e2e4", "g8f6", "e7f8q"
  //! File names and promotion piece type are always lowercase.
  //! \param[in,out] moveStr The move string, set to first character after move
  //! \return An empty string if \p moveStr is not valid
  //--------------------------------------------------------------------------
  std::string ToCoordinates(char*& moveStr) const;

private:
  char Friend(const char piece) const;
  char Enemy(const char piece) const;

  std::string PawnMove(const char* str, Square from = Square::None,
                       Square to = Square::None) const;
  std::string KnightMove(const char* str, Square from= Square::None,
                         Square to = Square::None) const;
  std::string BishopMove(const char* str, Square from= Square::None,
                         Square to = Square::None) const;
  std::string RookMove(const char* str, Square from= Square::None,
                       Square to = Square::None) const;
  std::string QueenMove(const char* str, Square from= Square::None,
                        Square to = Square::None) const;
  std::string KingMove(const char* str, Square from= Square::None,
                       Square to = Square::None) const;
  std::string PieceMove(const char*& str, const char type,
                        Square& from, Square& to) const;

  void AddOrigins(const char type, int x, int y, std::set<Square>& origs) const;
  bool AddMove(ChessMove&, const Square& dest, const int x, const int y,
               const char cap, std::list<ChessMove>& moves) const;
  void AddBishopMoves(const std::set<Square>& origins, const int x, const int y,
                      const char cap, std::list<ChessMove>& moves) const;
  void AddKingMoves(const std::set<Square>& origins, const int x, const int y,
                    const char cap, std::list<ChessMove>& moves) const;
  void AddKnightMoves(const std::set<Square>& origins, const int x, const int y,
                      const char cap, std::list<ChessMove>& moves) const;
  void AddPawnMoves(const std::set<Square>& origins, const int x, const int y,
                    const char cap, std::list<ChessMove>& moves) const;
  void AddRookMoves(const std::set<Square>& origins, const int x, const int y,
                    const char cap, std::list<ChessMove>& moves) const;

  enum ColorToMove { White, Black } ctm;
  char      board[8][8];
  ChessMove castleLong[2];
  ChessMove castleShort[2];
  Square    ep;
};

} // namespace senjo

#endif // SENJO_MOVE_FINDER_H
