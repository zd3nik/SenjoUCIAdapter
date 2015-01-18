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

#include "ChessEngine.h"
#include "Output.h"

namespace senjo
{

//----------------------------------------------------------------------------
// static variables
//----------------------------------------------------------------------------
bool        ChessEngine::_debug = false;
bool        ChessEngine::_searching = false;
int         ChessEngine::_stop = 0;
uint64_t    ChessEngine::_startTime = 0;
uint64_t    ChessEngine::_stopTime = 0;
const char* ChessEngine::_STARTPOS =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

//----------------------------------------------------------------------------
uint64_t ChessEngine::Perft(const int depth)
{
  _stop = 0;
  _searching = false;
  _startTime = Now();
  _stopTime = 0;
  return MyPerft(depth);
}

//----------------------------------------------------------------------------
std::string ChessEngine::Go(const int depth,
                            const int movestogo,
                            const uint64_t movetime,
                            const uint64_t wtime, const uint64_t winc,
                            const uint64_t btime, const uint64_t binc,
                            std::string* ponder)
{
  _stop = 0;
  _searching = true;
  _startTime = Now();

  // set _stopTime to something smarter in MyGo() if you wish
  _stopTime = (movetime ? (_startTime + movetime) : 0);
  const uint64_t timeRemaining = (WhiteToMove() ? wtime : btime);
  if (timeRemaining) {
    const int moves = (movestogo ? movestogo : 15);
    const uint64_t timePerMove = (timeRemaining / moves);
    const uint64_t endTime = (_startTime + timePerMove);
    if (!_stopTime || (endTime < _stopTime)) {
      _stopTime = endTime;
    }
  }

  if (UseTimer() && !timerThread.Active()) {
    if (!timerThread.Start(Timer, this)) {
      Output() << "Failed to start timer thread!";
    }
  }

  std::string bestmove =
      MyGo(depth, movestogo, movetime, wtime, winc, btime, binc, ponder);

  _searching = false;
  if (StopRequested() && UseTimer()) {
    timerThread.Join();
  }

  return bestmove;
}

//----------------------------------------------------------------------------
void ChessEngine::Timer(void* data)
{
  try {
    if (!data) {
      Output() << "ChessEngine::Timer() NULL data parameter";
      return;
    }

    ChessEngine* engine;
#ifdef NDEBUG
    engine = static_cast<ChessEngine*>(data);
#else
    engine = reinterpret_cast<ChessEngine*>(data);
    if (!engine) {
      Output() << "ChessEngine::Timer() failed to cast data parameter";
      return;
    }
#endif

    const unsigned int outputInterval = engine->TimerOutputInterval();
    const unsigned int sleepInterval = engine->TimerSleepInterval();

    char     move[8];
    int      depth = 0;
    int      movenum = 0;
    int      seldepth = 0;
    uint64_t msecs = 0;
    uint64_t nodes = 0;
    uint64_t qnodes = 0;

    while (!engine->StopRequested()) {
      if (engine->IsSearching() && !engine->TimeoutOccurred()) {
        const uint64_t now = Now();
        const uint64_t end = engine->GetStopTime();

        if (end && ((now + 100) >= end)) {
          engine->Stop(StopReason::Timeout);
        }
        else if (now >= (Output::LastOutput() + outputInterval)) {
          engine->GetStats(&depth, &seldepth, &nodes, &qnodes, &msecs,
                           &movenum, move, sizeof(move));

          Output out(Output::NoPrefix);
          out << "info depth " << depth
              << " seldepth " << seldepth
              << " nodes " << nodes
              << " time " << msecs
              << " nps " << static_cast<uint64_t>(Rate(nodes, msecs));

          if (movenum > 0) {
            out << " currmovenumber " << movenum
                << " currmove " << move;
          }
        }

        const uint64_t tmout = (end - now - 100);
        if (end && (tmout < outputInterval)) {
          if (!MillisecondSleep(static_cast<unsigned int>(tmout))) {
            break;
          }
          continue;
        }
      }

      if (!MillisecondSleep(sleepInterval)) {
        break;
      }
    }
  }
  catch (std::exception& e) {
    std::cerr << "info string ChessEngine::Timer() ERROR: "
              << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "info string ChessEngine::Timer() ERROR: unknown exception"
              << std::endl;
  }
}

} // namespace senjo
