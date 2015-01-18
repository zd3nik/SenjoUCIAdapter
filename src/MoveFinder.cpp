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

#include "MoveFinder.h"
#include "Output.h"

namespace senjo {

//----------------------------------------------------------------------------
bool MoveFinder::IsPiece(const char ch)
{
  switch (toupper(ch)) {
  case 'B': case 'K': case 'N': case 'P': case 'Q': case 'R':
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool MoveFinder::LoadFEN(const char* fen)
{
  ctm = White;
  memset(board, 0, sizeof(board));
  castleShort[White].Clear();
  castleShort[Black].Clear();
  castleLong[White].Clear();
  castleLong[Black].Clear();
  ep = Square::None;

  const char* p = fen;
  for (int y = 7; y >= 0; --y, ++p) {
    for (int x = 0; x < 8; ++x, ++p) {
      if (isdigit(*p)) {
        x += (*p - '1');
      }
      else if (IsPiece(*p)) {
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

  switch (*NextWord(p)) {
  case 'b': ctm = Black; break;
  case 'w': ctm = White; break;
  default:
    Output() << "Expected 'w' or 'b' at " << p;
    return false;
  }

  NextSpace(p);
  NextWord(p);
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

  NextSpace(p);
  NextWord(p);
  if (IS_X(p[0]) && IS_Y(p[1])) {
    const int x = TO_X(*p++);
    const int y = TO_Y(*p++);
    ep.Assign(x, y);
  }

  return true;
}

//----------------------------------------------------------------------------
std::string MoveFinder::ToCoordinates(char*& moveStr) const
{
  std::string move;
  if (!moveStr || !moveStr[0] || !moveStr[1]) {
    return move;
  }

  char* end = moveStr;
  NextSpace(end);

  if (!strncmp(moveStr, "0-0-0", 5) ||
      !strncmp(moveStr, "O-O-O", 5) ||
      !strncmp(moveStr, "o-o-o", 5) ||
      !strncmp(moveStr, castleLong[ctm].ToString().c_str(), 4))
  {
    move = castleLong[ctm].ToString();
  }
  else if (!strncmp(moveStr, "0-0", 3) ||
           !strncmp(moveStr, "O-O", 3) ||
           !strncmp(moveStr, "o-o", 3) ||
           !strncmp(moveStr, castleShort[ctm].ToString().c_str(), 4))
  {
    move = castleShort[ctm].ToString();
  }
  else if (!strcmp(moveStr, "ep") || !strcmp(moveStr, "e.p.")) {
    if (ep.IsValid()) {
      move = PawnMove("", Square(), ep);
    }
  }
  else {
    const char* p = moveStr;
    switch (*p) {
    case 'B': move = BishopMove(p + 1); break;
    case 'K': move = KingMove(p + 1);   break;
    case 'N': move = KnightMove(p + 1); break;
    case 'Q': move = QueenMove(p + 1);  break;
    case 'R': move = RookMove(p + 1);   break;
    case 'P': move = PawnMove(p + 1);   break;
    default:
      if (IS_X(p[0]) && IS_Y(p[1])) {
        const int x = TO_X(*p++);
        const int y = TO_Y(*p++);
        const int pc = board[x][y];
        if (!pc) {
          move = PawnMove(p, Square(), Square(x, y));
        }
        else if (pc == Friend(pc)) {
          switch (toupper(pc)) {
          case 'B': move = BishopMove(p, Square(x, y)); break;
          case 'K': move = KingMove(p, Square(x, y));   break;
          case 'N': move = KnightMove(p, Square(x, y)); break;
          case 'P': move = PawnMove(p, Square(x, y));   break;
          case 'Q': move = QueenMove(p, Square(x, y));  break;
          case 'R': move = RookMove(p, Square(x, y));   break;
          }
        }
      }
      else if (IS_X(p[0]) &&
               (IS_X(p[1]) || (p[1] == '-') || (p[1] == 'x') || (p[1] == ':')))
      {
        move = PawnMove(p);
      }
    }
  }

  if (move.size()) {
    moveStr = end;
  }

  return move;
}

//----------------------------------------------------------------------------
char MoveFinder::Friend(const char piece) const
{
  return (ctm == White) ? toupper(piece) : tolower(piece);
}

//----------------------------------------------------------------------------
char MoveFinder::Enemy(const char piece) const
{
  return (ctm == White) ? tolower(piece) : toupper(piece);
}

//----------------------------------------------------------------------------
std::string MoveFinder::PawnMove(const char* p, Square from, Square to) const
{
  std::string move = PieceMove(p, Friend('P'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    const char ch = tolower(*p);
    switch (ch) {
    case 'b': case 'n': case 'q': case 'r':
      move += ch;
      return move;
    }
    return std::string();
  }
  return move;
}

//----------------------------------------------------------------------------
std::string MoveFinder::KnightMove(const char* p, Square from, Square to) const
{
  std::string move = PieceMove(p, Friend('N'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//----------------------------------------------------------------------------
std::string MoveFinder::BishopMove(const char* p, Square from, Square to) const
{
  std::string move = PieceMove(p, Friend('B'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//----------------------------------------------------------------------------
std::string MoveFinder::RookMove(const char* p, Square from, Square to) const
{
  std::string move = PieceMove(p, Friend('R'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//----------------------------------------------------------------------------
std::string MoveFinder::QueenMove(const char* p, Square from, Square to) const
{
  std::string move = PieceMove(p, Friend('Q'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//----------------------------------------------------------------------------
std::string MoveFinder::KingMove(const char* p, Square from, Square to) const
{
  std::string move = PieceMove(p, Friend('K'), from, to);
  if (move.size() && *p && !isspace(*p)) {
    return std::string();
  }
  return move;
}

//----------------------------------------------------------------------------
std::string MoveFinder::PieceMove(const char*& p, const char type,
                                  Square& from, Square& to) const
{
  assert(IsPiece(type));
  assert(type == Friend(type));

  int fromX = from.X();
  int fromY = from.Y();
  if (!from) {
    if (IS_X(p[0]) && IS_Y(p[1])) {
      if (p[2]) {
        fromX = TO_X(*p++);
        fromY = TO_Y(*p++);
        from = Square(fromX, fromY);
      }
    }
    else if (IS_X(p[0]) && p[1]) {
      fromX = TO_X(*p++);
    }
    else if (IS_Y(p[0]) && p[1]) {
      fromY = TO_Y(*p++);
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
    cap = Enemy(*p++);
    break;
  }

  int toX = to.X();
  int toY = to.Y();
  if (!to) {
    if (IS_X(p[0]) && IS_Y(p[1])) {
      toX = TO_X(*p++);
      toY = TO_Y(*p++);
      to = Square(toX, toY);
    }
    else if (IS_X(p[0]) && p[1]) {
      toX = TO_X(*p++);
    }
    else if (IS_Y(p[0]) && p[1]) {
      toY = TO_Y(*p++);
    }
    else {
      to = from;
      toX = fromX;
      toY = fromY;
      from = Square::None;
      fromX = -1;
      fromY = -1;
    }
  }

  std::set<Square> origins;
  AddOrigins(type, fromX, fromY, origins);
  if (origins.empty()) {
    return std::string();
  }

  std::list<ChessMove> moves;
  switch (toupper(type)) {
  case 'B':
    AddBishopMoves(origins, toX, toY, cap, moves);
    break;
  case 'N':
    AddKnightMoves(origins, toX, toY, cap, moves);
    break;
  case 'Q':
    AddBishopMoves(origins, toX, toY, cap, moves);
    AddRookMoves(origins, toX, toY, cap, moves);
    break;
  case 'P':
    AddPawnMoves(origins, toX, toY, cap, moves);
    break;
  case 'R':
    AddRookMoves(origins, toX, toY, cap, moves);
    break;
  case 'K':
    AddKingMoves(origins, toX, toY, cap, moves);
    break;
  }

  if (moves.size() != 1) {
    return std::string();
  }

  while (*p && ispunct(*p)) ++p;
  return moves.front().ToString();
}

//----------------------------------------------------------------------------
void MoveFinder::AddOrigins(const char pc, int x, int y,
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

//----------------------------------------------------------------------------
//! \return true if move generation along this line should stop
//----------------------------------------------------------------------------
bool MoveFinder::AddMove(ChessMove& move, const Square& dest,
                         const int x, const int y, const char cap,
                         std::list<ChessMove>& moves) const
{
  const int destX = dest.X();
  const int destY = dest.Y();
  if (((x < 0) || (x == destX)) && ((y < 0) || (y == destY))) {
    move.cap = board[destX][destY];
    if (move.cap && (move.cap == Friend(move.cap))) {
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
  return (dest.IsValid() ? board[destX][destY] : false);
}

//----------------------------------------------------------------------------
void MoveFinder::AddBishopMoves(const std::set<Square>& origins,
                                const int x, const int y, const char cap,
                                std::list<ChessMove>& moves) const
{
  ChessMove move;
  Direction direction;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    direction = move.from.DirectionTo(Square(x, y));
    if ((direction == Direction::Unknown) || (direction == NorthEast)) {
      for (dest = (move.from + NorthEast); dest.IsValid(); dest += NorthEast) {
        if (AddMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == SouthEast)) {
      for (dest = (move.from + SouthEast); dest.IsValid(); dest += SouthEast) {
        if (AddMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == SouthWest)) {
      for (dest = (move.from + SouthWest); dest.IsValid(); dest += SouthWest) {
        if (AddMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == NorthWest)) {
      for (dest = (move.from + NorthWest); dest.IsValid(); dest += NorthWest) {
        if (AddMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
void MoveFinder::AddKingMoves(const std::set<Square>& origins,
                              const int x, const int y, const char cap,
                              std::list<ChessMove>& moves) const
{
  ChessMove move;
  Direction direction;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    direction = move.from.DirectionTo(Square(x, y));
    if ((direction == Direction::Unknown) || (direction == North)) {
      if ((dest = (move.from + North)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == NorthEast)) {
      if ((dest = (move.from + NorthEast)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == East)) {
      if ((dest = (move.from + East)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == SouthEast)) {
      if ((dest = (move.from + SouthEast)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == South)) {
      if ((dest = (move.from + South)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == SouthWest)) {
      if ((dest = (move.from + SouthWest)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == West)) {
      if ((dest = (move.from + West)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == NorthWest)) {
      if ((dest = (move.from + NorthWest)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
  }
}

//----------------------------------------------------------------------------
void MoveFinder::AddKnightMoves(const std::set<Square>& origins,
                                const int x, const int y, const char cap,
                                std::list<ChessMove>& moves) const
{
  ChessMove move;
  Direction direction;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    direction = move.from.DirectionTo(Square(x, y));
    if ((direction == Direction::Unknown) || (direction == KnightMove1)) {
      if ((dest = (move.from + KnightMove1)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove2)) {
      if ((dest = (move.from + KnightMove2)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove3)) {
      if ((dest = (move.from + KnightMove3)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove4)) {
      if ((dest = (move.from + KnightMove4)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove5)) {
      if ((dest = (move.from + KnightMove5)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove6)) {
      if ((dest = (move.from + KnightMove6)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove7)) {
      if ((dest = (move.from + KnightMove7)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    if ((direction == Direction::Unknown) || (direction == KnightMove8)) {
      if ((dest = (move.from + KnightMove8)).IsValid()) {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
  }
}

//----------------------------------------------------------------------------
void MoveFinder::AddPawnMoves(const std::set<Square>& origins,
                              const int x, const int y, const char cap,
                              std::list<ChessMove>& moves) const
{
  ChessMove move;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    if (ctm == White) {
      if ((dest = (move.from + North)).IsValid() &&
          !board[dest.X()][dest.Y()])
      {
        AddMove(move, dest, x, y, cap, moves);
        if ((move.from.Y() == 1) && (dest += North).IsValid() &&
            !board[dest.X()][dest.Y()])
        {
          AddMove(move, dest, x, y, cap, moves);
        }
      }
      if ((dest = (move.from + NorthEast)).IsValid() &&
          (board[dest.X()][dest.Y()] || (dest == ep)))
      {
        AddMove(move, dest, x, y, cap, moves);
      }
      if ((dest = (move.from + NorthWest)).IsValid() &&
          (board[dest.X()][dest.Y()] || (dest == ep)))
      {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
    else {
      if ((dest = (move.from + South)).IsValid() &&
          !board[dest.X()][dest.Y()])
      {
        AddMove(move, dest, x, y, cap, moves);
        if ((move.from.Y() == 6) && (dest += South).IsValid() &&
            !board[dest.X()][dest.Y()])
        {
          AddMove(move, dest, x, y, cap, moves);
        }
      }
      if ((dest = (move.from + SouthEast)).IsValid() &&
          (board[dest.X()][dest.Y()] || (dest == ep)))
      {
        AddMove(move, dest, x, y, cap, moves);
      }
      if ((dest = (move.from + SouthWest)).IsValid() &&
          (board[dest.X()][dest.Y()] || (dest == ep)))
      {
        AddMove(move, dest, x, y, cap, moves);
      }
    }
  }
}

//----------------------------------------------------------------------------
void MoveFinder::AddRookMoves(const std::set<Square>& origins,
                              const int x, const int y, const char cap,
                              std::list<ChessMove>& moves) const
{
  ChessMove move;
  Direction direction;
  Square    dest;
  std::set<Square>::const_iterator it;
  for (it = origins.begin(); it != origins.end(); ++it) {
    move.from = *it;
    direction = move.from.DirectionTo(Square(x, y));
    if ((direction == Direction::Unknown) || (direction == North)) {
      for (dest = (move.from + North); dest.IsValid(); dest += North) {
        if (AddMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == East)) {
      for (dest = (move.from + East); dest.IsValid(); dest += East) {
        if (AddMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == South)) {
      for (dest = (move.from + South); dest.IsValid(); dest += South) {
        if (AddMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
    if ((direction == Direction::Unknown) || (direction == West)) {
      for (dest = (move.from + West); dest.IsValid(); dest += West) {
        if (AddMove(move, dest, x, y, cap, moves)) {
          break;
        }
      }
    }
  }
}

} // namespace senjo
