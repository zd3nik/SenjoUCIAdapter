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

#include "MoveFinder.h"
#include "Parameters.h"
#include "Output.h"

namespace senjo {

//-----------------------------------------------------------------------------
static inline int toX(int ch) { return (ch - 'a'); }

//-----------------------------------------------------------------------------
static inline int toY(int ch) { return (ch - '1'); }

//-----------------------------------------------------------------------------
static inline bool isX(char ch) { return ((ch >= 'a') & (ch <= 'h')); }

//-----------------------------------------------------------------------------
static inline bool isY(char ch) { return ((ch >= '1') & (ch <= '8')); }

//-----------------------------------------------------------------------------
static inline const char* nextWord(const char*& p) {
  while (*p && isspace(*p)) ++p;
  return p;
}

//-----------------------------------------------------------------------------
static inline const char* nextSpace(const char*& p) {
  while (*p && !isspace(*p)) ++p;
  return p;
}

//-----------------------------------------------------------------------------
bool MoveFinder::isPiece(const char ch) {
  switch (toupper(ch)) {
  case 'B': case 'K': case 'N': case 'P': case 'Q': case 'R':
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
bool MoveFinder::loadFEN(const std::string& fen) {
  ctm = White;
  memset(board, 0, sizeof(board));
  castleShort[White].clear();
  castleShort[Black].clear();
  castleLong[White].clear();
  castleLong[Black].clear();
  ep = Square::None;

  const char* p = fen.c_str();
  nextWord(p);
  for (int y = 7; y >= 0; --y, ++p) {
    for (int x = 0; x < 8; ++x, ++p) {
      if (isdigit(*p)) {
        x += (*p - '1');
      }
      else if (isPiece(*p)) {
        board[x][y] = *p;
      }
      else {
        Output() << "Invalid character at " << p;
        return false;
      }
    }
    if ((y > 0) && (*p != '/')) {
      Output() << "Expected '/' at " << p;
      return false;
    }
  }

  switch (*nextWord(p)) {
  case 'b': ctm = Black; break;
  case 'w': ctm = White; break;
  default:
    Output() << "Expected 'w' or 'b' at " << p;
    return false;
  }

  nextSpace(p);
  nextWord(p);
  while (*p && !isspace(*p)) {
    switch (*p++) {
    case '-':
      break;
    case 'K':
      castleShort[White].from = Square::E1;
      castleShort[White].to = Square::G1;
      continue;
    case 'Q':
      castleLong[White].from = Square::E1;
      castleLong[White].to = Square::C1;
      continue;
    case 'k':
      castleShort[Black].from = Square::E8;
      castleShort[Black].to = Square::G8;
      continue;
    case 'q':
      castleLong[Black].from = Square::E8;
      castleLong[Black].to = Square::C8;
      continue;
    default:
      Output() << "Unexpected castle rights at " << p;
      return false;
    }
    break;
  }

  nextSpace(p);
  nextWord(p);
  if (isX(p[0]) && isY(p[1])) {
    const int x = toX(*p++);
    const int y = toY(*p++);
    ep.assign(x, y);
  }

  return true;
}

//-----------------------------------------------------------------------------
std::string MoveFinder::toCoordinates(const std::string& moveStr) const {
  std::string move;
  if (moveStr.size() < 2) {
    return move;
  }

  if ((moveStr == "0-0-0") ||
      (moveStr == "O-O-O") ||
      (moveStr == "o-o-o") ||
      (moveStr == castleLong[ctm].toString()))
  {
    move = castleLong[ctm].toString();
  }
  else if ((moveStr == "0-0") ||
           (moveStr == "O-O") ||
           (moveStr == "o-o") ||
           (moveStr == castleShort[ctm].toString()))
  {
    move = castleShort[ctm].toString();
  }
  else if ((moveStr == "ep") || (moveStr == "e.p.")) {
    if (ep.isValid()) {
      move = pawnMove("", Square(), ep);
    }
  }
  else {
    const char* p = moveStr.c_str();
    switch (*p) {
    case 'B': move = bishopMove(p + 1); break;
    case 'K': move = kingMove(p + 1);   break;
    case 'N': move = knightMove(p + 1); break;
    case 'Q': move = queenMove(p + 1);  break;
    case 'R': move = rookMove(p + 1);   break;
    case 'P': move = pawnMove(p + 1);   break;
    default:
      if (isX(p[0]) && isY(p[1])) {
        const int x = toX(*p++);
        const int y = toY(*p++);
        const char pc = board[x][y];
        if (!pc) {
          move = pawnMove(p, Square(), Square(x, y));
        }
        else if (pc == friendPiece(pc)) {
          switch (toupper(pc)) {
          case 'B': move = bishopMove(p, Square(x, y)); break;
          case 'K': move = kingMove(p, Square(x, y));   break;
          case 'N': move = knightMove(p, Square(x, y)); break;
          case 'P': move = pawnMove(p, Square(x, y));   break;
          case 'Q': move = queenMove(p, Square(x, y));  break;
          case 'R': move = rookMove(p, Square(x, y));   break;
          }
        }
      }
      else if (isX(p[0]) &&
               (isX(p[1]) || (p[1] == '-') || (p[1] == 'x') || (p[1] == ':')))
      {
        move = pawnMove(p);
      }
    }
  }

  return move;
}

//-----------------------------------------------------------------------------
char MoveFinder::friendPiece(const char piece) const {
  return static_cast<char>((ctm == White) ? toupper(piece) : tolower(piece));
}

//-----------------------------------------------------------------------------
char MoveFinder::enemyPiece(const char piece) const {
  return static_cast<char>((ctm == White) ? tolower(piece) : toupper(piece));
}

//-----------------------------------------------------------------------------
std::string MoveFinder::pawnMove(const char* p, Square from, Square to) const {
  std::string move = pieceMove(p, friendPiece('P'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    const char ch = static_cast<char>(tolower(*p));
    switch (ch) {
    case 'b': case 'n': case 'q': case 'r':
      move += ch;
      return move;
    }
    return std::string();
  }
  return move;
}

//-----------------------------------------------------------------------------
std::string MoveFinder::knightMove(const char* p, Square from, Square to) const {
  std::string move = pieceMove(p, friendPiece('N'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//-----------------------------------------------------------------------------
std::string MoveFinder::bishopMove(const char* p, Square from, Square to) const
{
  std::string move = pieceMove(p, friendPiece('B'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//-----------------------------------------------------------------------------
std::string MoveFinder::rookMove(const char* p, Square from, Square to) const
{
  std::string move = pieceMove(p, friendPiece('R'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//-----------------------------------------------------------------------------
std::string MoveFinder::queenMove(const char* p, Square from, Square to) const
{
  std::string move = pieceMove(p, friendPiece('Q'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//-----------------------------------------------------------------------------
std::string MoveFinder::kingMove(const char* p, Square from, Square to) const
{
  std::string move = pieceMove(p, friendPiece('K'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//-----------------------------------------------------------------------------
std::string MoveFinder::pieceMove(const char*& p, const char type,
                                  Square& from, Square& to) const
{
  assert(isPiece(type));
  assert(type == friendPiece(type));

  int fromX = from.x();
  int fromY = from.y();
  if (!from) {
    if (isX(p[0]) && isY(p[1])) {
      if (p[2]) {
        fromX = toX(*p++);
        fromY = toY(*p++);
        from = Square(fromX, fromY);
      }
    }
    else if (isX(p[0]) && p[1]) {
      fromX = toX(*p++);
    }
    else if (isY(p[0]) && p[1]) {
      fromY = toY(*p++);
    }
  }

  char cap = 0;
  if ((*p == '-') || (*p == 'x') || (*p == ':')) {
    if ((cap = *p++) == ':') {
      cap = 'x';
    }
  }

  switch (*p) {
  case 'B': case 'N': case 'P': case 'Q': case 'R':
    if (cap == '-') {
      return std::string();
    }
    cap = enemyPiece(*p++);
    break;
  }

  int destX = to.x();
  int destY = to.y();
  if (!to) {
    if (isX(p[0]) && isY(p[1])) {
      destX = toX(*p++);
      destY = toY(*p++);
      to = Square(destX, destY);
    }
    else if (isX(p[0]) && p[1]) {
      destX = toX(*p++);
    }
    else if (isY(p[0]) && p[1]) {
      destY = toY(*p++);
    }
    else {
      to = from;
      destX = fromX;
      destY = fromY;
      from = Square::None;
      fromX = -1;
      fromY = -1;
    }
  }

  std::set<Square> origins;
  addOrigins(type, fromX, fromY, origins);
  if (origins.empty()) {
    return std::string();
  }

  std::list<ChessMove> moves;
  switch (toupper(type)) {
  case 'B':
    addBishopMoves(origins, destX, destY, cap, moves);
    break;
  case 'N':
    addKnightMoves(origins, destX, destY, cap, moves);
    break;
  case 'Q':
    addBishopMoves(origins, destX, destY, cap, moves);
    addRookMoves(origins, destX, destY, cap, moves);
    break;
  case 'P':
    addPawnMoves(origins, destX, destY, cap, moves);
    break;
  case 'R':
    addRookMoves(origins, destX, destY, cap, moves);
    break;
  case 'K':
    addKingMoves(origins, destX, destY, cap, moves);
    break;
  }

  if (moves.size() != 1) {
    return std::string();
  }

  while (*p && ispunct(*p)) ++p;
  return moves.front().toString();
}

//-----------------------------------------------------------------------------
void MoveFinder::addOrigins(const char pc, int x, int y,
                            std::set<Square>& origins) const
{
  if ((x >= 0) && (x < 8)) {
    if ((y >= 0) && (y < 8)) {
      if (board[x][y] == pc) {
        origins.insert(Square(x, y));
      }
    }
    else {
      for (y = 0; y < 8; ++y) {
        if (board[x][y] == pc) {
          origins.insert(Square(x, y));
        }
      }
    }
  }
  else if ((y >= 0) && (y < 8)) {
    for (x = 0; x < 8; ++x) {
      if (board[x][y] == pc) {
        origins.insert(Square(x, y));
      }
    }
  }
  else {
    for (x = 0; x < 8; ++x) {
      for (y = 0; y < 8; ++y) {
        if (board[x][y] == pc) {
          origins.insert(Square(x, y));
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
//! \return true if move generation along this line should stop
//-----------------------------------------------------------------------------
bool MoveFinder::addMove(ChessMove& move, const Square& dest,
                         const int x, const int y, const char cap,
                         std::list<ChessMove>& moves) const
{
  const int destX = dest.x();
  const int destY = dest.y();
  if (((x < 0) || (x == destX)) && ((y < 0) || (y == destY))) {
    move.cap = board[destX][destY];
    if (move.cap && (move.cap == friendPiece(move.cap))) {
      return true;
    }
    if (move.cap ? (cap == '-') : (cap == 'x')) {
      return true;
    }
    switch (cap) {
    case 'B': case 'N': case 'Q': case 'R':
      if (toupper(move.cap) != cap) {
        return true;
      }
    }
    move.to = dest;
    moves.push_back(move);
    if (move.cap) {
      return true;
    }
  }
  return (dest.isValid() ? board[destX][destY] : false);
}

//-----------------------------------------------------------------------------
void MoveFinder::addBishopMoves(const std::set<Square>& origins,
                                const int x, const int y, const char cap,
                                std::list<ChessMove>& moves) const
{
  ChessMove move;
  Direction direction;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    direction = move.from.directionTo(Square(x, y));
    if ((direction == Direction::Unknown) || (direction == NorthEast)) {
      for (dest = (move.from + NorthEast); dest.isValid(); dest += NorthEast) {
        if (addMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == SouthEast)) {
      for (dest = (move.from + SouthEast); dest.isValid(); dest += SouthEast) {
        if (addMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == SouthWest)) {
      for (dest = (move.from + SouthWest); dest.isValid(); dest += SouthWest) {
        if (addMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == NorthWest)) {
      for (dest = (move.from + NorthWest); dest.isValid(); dest += NorthWest) {
        if (addMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
void MoveFinder::addKingMoves(const std::set<Square>& origins,
                              const int x, const int y, const char cap,
                              std::list<ChessMove>& moves) const
{
  ChessMove move;
  Direction direction;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    direction = move.from.directionTo(Square(x, y));
    if ((direction == Direction::Unknown) || (direction == North)) {
      if ((dest = (move.from + North)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == NorthEast)) {
      if ((dest = (move.from + NorthEast)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == East)) {
      if ((dest = (move.from + East)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == SouthEast)) {
      if ((dest = (move.from + SouthEast)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == South)) {
      if ((dest = (move.from + South)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == SouthWest)) {
      if ((dest = (move.from + SouthWest)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == West)) {
      if ((dest = (move.from + West)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == NorthWest)) {
      if ((dest = (move.from + NorthWest)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void MoveFinder::addKnightMoves(const std::set<Square>& origins,
                                const int x, const int y, const char cap,
                                std::list<ChessMove>& moves) const
{
  ChessMove move;
  Direction direction;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    direction = move.from.directionTo(Square(x, y));
    if ((direction == Direction::Unknown) || (direction == KnightMove1)) {
      if ((dest = (move.from + KnightMove1)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove2)) {
      if ((dest = (move.from + KnightMove2)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove3)) {
      if ((dest = (move.from + KnightMove3)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove4)) {
      if ((dest = (move.from + KnightMove4)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove5)) {
      if ((dest = (move.from + KnightMove5)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove6)) {
      if ((dest = (move.from + KnightMove6)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove7)) {
      if ((dest = (move.from + KnightMove7)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove8)) {
      if ((dest = (move.from + KnightMove8)).isValid()) {
        addMove(move, dest, x, y, cap, moves);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void MoveFinder::addPawnMoves(const std::set<Square>& origins,
                              const int x, const int y, const char cap,
                              std::list<ChessMove>& moves) const
{
  ChessMove move;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    if (ctm == White) {
      if ((dest = (move.from + North)).isValid() &&
          !board[dest.x()][dest.y()])
      {
        addMove(move, dest, x, y, cap, moves);
        if ((move.from.y() == 1) && (dest += North).isValid() &&
            !board[dest.x()][dest.y()])
        {
          addMove(move, dest, x, y, cap, moves);
        }
      }
      if ((dest = (move.from + NorthEast)).isValid() &&
          (board[dest.x()][dest.y()] || (dest == ep)))
      {
        addMove(move, dest, x, y, cap, moves);
      }
      if ((dest = (move.from + NorthWest)).isValid() &&
          (board[dest.x()][dest.y()] || (dest == ep)))
      {
        addMove(move, dest, x, y, cap, moves);
      }
    }
    else {
      if ((dest = (move.from + South)).isValid() &&
          !board[dest.x()][dest.y()])
      {
        addMove(move, dest, x, y, cap, moves);
        if ((move.from.y() == 6) && (dest += South).isValid() &&
            !board[dest.x()][dest.y()])
        {
          addMove(move, dest, x, y, cap, moves);
        }
      }
      if ((dest = (move.from + SouthEast)).isValid() &&
          (board[dest.x()][dest.y()] || (dest == ep)))
      {
        addMove(move, dest, x, y, cap, moves);
      }
      if ((dest = (move.from + SouthWest)).isValid() &&
          (board[dest.x()][dest.y()] || (dest == ep)))
      {
        addMove(move, dest, x, y, cap, moves);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void MoveFinder::addRookMoves(const std::set<Square>& origins,
                              const int x, const int y, const char cap,
                              std::list<ChessMove>& moves) const
{
  ChessMove move;
  Direction direction;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    direction = move.from.directionTo(Square(x, y));
    if ((direction == Direction::Unknown) || (direction == North)) {
      for (dest = (move.from + North); dest.isValid(); dest += North) {
        if (addMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == East)) {
      for (dest = (move.from + East); dest.isValid(); dest += East) {
        if (addMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == South)) {
      for (dest = (move.from + South); dest.isValid(); dest += South) {
        if (addMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == West)) {
      for (dest = (move.from + West); dest.isValid(); dest += West) {
        if (addMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
  }
}

} // namespace senjo
