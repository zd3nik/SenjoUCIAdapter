//-----------------------------------------------------------------------------
// Copyright (c) 2019 Shawn Chidester <zd3nik@gmail.com>
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

#ifndef SENJO_GO_PARAMS_H
#define SENJO_GO_PARAMS_H

#include "Platform.h"

namespace senjo {

struct GoParams {
  bool     infinite   = false; // Search until the "stop" command
  bool     ponder     = false; // Start searching in pondering mode
  int      depth      = 0; // Maximum number of half-moves (plies) to search
  int      movestogo  = 0; // Number of moves remaining until next time control
  uint64_t binc       = 0; // Black increment per move in milliseconds
  uint64_t btime      = 0; // Milliseconds remaining on black's clock
  uint64_t movetime   = 0; // Maximum milliseconds to spend on this move
  uint64_t nodes      = 0; // Maximum number of nodes to search
  uint64_t winc       = 0; // White increment per move in milliseconds
  uint64_t wtime      = 0; // Milliseconds remaining on white's clock
};

} // namespace senjo

#endif // SENJO_GO_PARAMS_H
