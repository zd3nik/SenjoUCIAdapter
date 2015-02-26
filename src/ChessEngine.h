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

#ifndef SENJO_CHESS_ENGINE_H
#define SENJO_CHESS_ENGINE_H

#include "EngineOption.h"
#include "Threading.h"

namespace senjo {

//----------------------------------------------------------------------------
//! \brief Base class for Senjo compatible chess engines
//! Derive from this class to create a chess engine that may be used with
//! the Senjo UCIAdapter.
//----------------------------------------------------------------------------
class ChessEngine
{
public:
  enum StopReason {
    FullStop = 0x1, ///< Stop everything
    Timeout  = 0x2  ///< Stop current search only
  };

  //--------------------------------------------------------------------------
  //! \brief FEN string of standard chess start position
  //! See http://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  //--------------------------------------------------------------------------
  static const char* _STARTPOS;

  //--------------------------------------------------------------------------
  //! \brief Get the engine name
  //! \return The engine name
  //--------------------------------------------------------------------------
  virtual std::string GetEngineName() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Get the engine version (e.g. "major.minor.build" e.g. "1.0.0")
  //! \return The engine version
  //--------------------------------------------------------------------------
  virtual std::string GetEngineVersion() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Get the engine quthor name (e.g. "John Doe <jdoe@nowhere.com>")
  //! \return The engine author name
  //--------------------------------------------------------------------------
  virtual std::string GetAuthorName() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Get options supported by the engine, and their current values
  //! \return A list of the engine's options and their current values
  //--------------------------------------------------------------------------
  virtual std::list<EngineOption> GetOptions() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Set a particular option to a given value
  //! Option value may be empty, particularly if the option type is Button
  //! \param[in] optionName The option name
  //! \param[in] optionValue The new option value
  //! \return false if the option name or value is invalid
  //--------------------------------------------------------------------------
  virtual bool SetEngineOption(const std::string& optionName,
                               const std::string& optionValue) = 0;

  //--------------------------------------------------------------------------
  //! \brief Initialize the engine
  //--------------------------------------------------------------------------
  virtual void Initialize() = 0;

  //--------------------------------------------------------------------------
  //! \brief Is the engine initialized?
  //! \return true if the engine is initialized
  //--------------------------------------------------------------------------
  virtual bool IsInitialized() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Set the board position according to a given FEN string
  //! The engine should use Output() to report errors in the FEN string.
  //! Only use position info from the given FEN string, don't process any moves
  //! or other data present in the FEN string.
  //! \param[in] fen The FEN string
  //! \return Position in \p fen after last char needed to load the position,
  //!         NULL if the FEN string does not contain a valid position
  //--------------------------------------------------------------------------
  virtual const char* SetPosition(const char* fen) = 0;

  //--------------------------------------------------------------------------
  //! \brief Execute a single move on the current position
  //! Determine whether the first word in the given string is a valid move
  //! and if it is apply the move to the current position.  Moves should be
  //! in coordinate notation (e.g. "e2e4", "g8f6", "e7f8q").
  //! \param[in] str A string containing one or more moves or anything else
  //! \return Position in \p str after the first move,
  //!         NULL if the first word isn't a valid move
  //--------------------------------------------------------------------------
  virtual const char* MakeMove(const char* str) = 0;

  //--------------------------------------------------------------------------
  //! \brief Get a FEN string representation of the current board position
  //! \return A FEN string representation of the current board postiion
  //--------------------------------------------------------------------------
  virtual std::string GetFEN() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Output a text representation of the current board position
  //--------------------------------------------------------------------------
  virtual void PrintBoard() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Is it white to move in the current position?
  //! \return true if it is white to move in the current position
  //--------------------------------------------------------------------------
  virtual bool WhiteToMove() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Clear any engine data that can persist between searches
  //! Examples of search data are the transposition table and killer moves.
  //--------------------------------------------------------------------------
  virtual void ClearSearchData() = 0;

  //--------------------------------------------------------------------------
  //! \brief The last ponder move was played
  //! The Go() method may return a ponder move which is the expected response
  //! to the bestmove returned by Go().  If pondering is enabled the UCI
  //! adapter may tell the engine to ponder this move, e.g. start searching
  //! for a reply to the ponder move.  If, while the engine is pondering, the
  //! ponder move is played this method will be called.  In general the engine
  //! should make what it has learned from its pondering available for the next
  //! Go() call.
  //--------------------------------------------------------------------------
  virtual void PonderHit() = 0;

  //--------------------------------------------------------------------------
  //! \brief Get statistics from the last (or current) search
  //! \param[out] depth The maximum depth achieved
  //! \param[out] seldepth The maximum selective depth achieved
  //! \param[out] nodes The number of nodes searched
  //! \param[out] qnodes The number of \p nodes that were in quiescence search
  //! \param[out] msecs The number of milliseconds spent searching
  //! \param[out] movenum The last/current move number searched
  //! \param[out] move The last/current move searched (e.g. "e2e4")
  //! \param[in] movelen The size of the \p move parameter
  //--------------------------------------------------------------------------
  virtual void GetStats(int* depth,
                        int* seldepth = NULL,
                        uint64_t* nodes = NULL,
                        uint64_t* qnodes = NULL,
                        uint64_t* msecs = NULL,
                        int* movenum = NULL,
                        char* move = NULL,
                        const size_t movelen = 0) const = 0;

  //--------------------------------------------------------------------------
  //! \brief Get a guess of how many moves remaining until game end
  //! This is used in the Go() method when the movestogo value is not given
  //! by the UCI caller.  It is generally not a good idea to return anything
  //! less than 10 or over 40 if the time control or move count is unknown.
  //! \return A guess of how many moves remaining until game end
  //--------------------------------------------------------------------------
  virtual int MovesToGo() const { return 20; }

  //--------------------------------------------------------------------------
  //! \brief Use the built-in timer thread for timeouts and periodic updates?
  //! \return true To use the built-in timer thread
  //--------------------------------------------------------------------------
  virtual bool UseTimer() const { return true; }

  //--------------------------------------------------------------------------
  //! \brief How long should the timer thread wait between outputs?
  //! \return Number of milliseconds to wait between outputs
  //--------------------------------------------------------------------------
  virtual unsigned int TimerOutputInterval() const { return 1000; }

  //--------------------------------------------------------------------------
  //! \brief Is the engine registered?
  //! \return true if the engine is registered
  //--------------------------------------------------------------------------
  virtual bool IsRegistered() const { return true; }

  //--------------------------------------------------------------------------
  //! \brief Register the engine later
  //! The engine should still function, but may cripple itself in some fashion.
  //--------------------------------------------------------------------------
  virtual void RegisterLater() { }

  //--------------------------------------------------------------------------
  //! \brief Register the engine now
  //! If this fails the engine should still function, but may cripple itself
  //! in some fashion.
  //! \param[in] name The name to register the engine to
  //! \param[in] code The code to use for registration
  //! \return true if successful
  //--------------------------------------------------------------------------
  virtual bool Register(const std::string& /*name*/,
                        const std::string& /*code*/)
  {
    return true;
  }

  //--------------------------------------------------------------------------
  //! \brief Does this engine use copy protection?
  //! \return true if the engine uses copy protection
  //--------------------------------------------------------------------------
  virtual bool IsCopyProtected() const { return false; }

  //--------------------------------------------------------------------------
  //! \brief Determine whether this is a legitimate copy of the engine
  //! This method will be called if IsCopyProtected() returns true.  This is
  //! where your engine should try to determine whether it is a legitimate
  //! copy or not.
  //! \return true if the engine is a legitimate copy
  //--------------------------------------------------------------------------
  virtual bool CopyIsOK() { return true; }

  //--------------------------------------------------------------------------
  //! \brief Do performance test on the current position
  //! This will call the MyPerft() method, which you must implement.
  //! \param[in] depth How many half-moves (plies) to search
  //! \return The number of leaf nodes visited at \p depth
  //--------------------------------------------------------------------------
  uint64_t Perft(const int depth);

  //--------------------------------------------------------------------------
  //! \brief Execute search on current position to find best move
  //! This will call the MyGo() method, which you must implement.
  //! \param[in] depth Maximum number of half-moves (plies) to search
  //! \param[in] movestogo Number of moves remaining until next time control
  //! \param[in] movetime Maximum milliseconds to spend on this move
  //! \param[in] wtime Milliseconds remaining on white's clock
  //! \param[in] winc White increment per move in milliseconds
  //! \param[in] btime Milliseconds remaining on black's clock
  //! \param[in] binc Black increment per move in milliseconds
  //! \param[out] ponder If not NULL set to the move engine should ponder next
  //! \return Best move in coordinate notation (e.g. "e2e4", "g8f6", "e7f8q")
  //--------------------------------------------------------------------------
  std::string Go(const int depth,
                 const int movestogo = 0,
                 const uint64_t movetime = 0,
                 const uint64_t wtime = 0, const uint64_t winc = 0,
                 const uint64_t btime = 0, const uint64_t binc = 0,
                 std::string* ponder = NULL);

  //--------------------------------------------------------------------------
  //! \brief Set the engine's debug mode on or off
  //! \param[in] flag true to enable debug mode, false to disable debug mode
  //--------------------------------------------------------------------------
  void SetDebug(const bool flag) { _debug = flag; }

  //--------------------------------------------------------------------------
  //! \brief Is debug mode enabled?
  //! \return true if debug mode is enabled
  //--------------------------------------------------------------------------
  bool IsDebugOn() const { return _debug; }

  //--------------------------------------------------------------------------
  //! \brief Is the engine currently executing the Go() method?
  //! It is not recommended to set this to true while Perft() is executing.
  //! \return true if the engine is searching
  //--------------------------------------------------------------------------
  bool IsSearching() const { return _searching; }

  //--------------------------------------------------------------------------
  //! \brief Get the millisecond timestamp of when Go() was started
  //! \return 0 if not searching
  //--------------------------------------------------------------------------
  uint64_t GetStartTime() const { return _startTime; }

  //--------------------------------------------------------------------------
  //! \brief Get millisecond timestamp when the current search should timeout
  //! \return 0 if not searching or the current search should not timeout
  //--------------------------------------------------------------------------
  uint64_t GetStopTime() const { return _stopTime; }

  //--------------------------------------------------------------------------
  //! \brief Clear all stop flags
  //--------------------------------------------------------------------------
  void ClearStopFlags() { _stop = 0; }

  //--------------------------------------------------------------------------
  //! \brief Tell the engine to stop searching
  //! Exit Perft()/Go() methods as quickly as possible.
  //! \param[in] reason The reason the search is being stopped
  //--------------------------------------------------------------------------
  void Stop(const StopReason reason) { _stop |= reason; }

  //--------------------------------------------------------------------------
  //! \brief Was the last search stopped by user request?
  //! \return true if Stop() was called with reason set to UserRequest
  //--------------------------------------------------------------------------
  bool StopRequested() const { return (_stop & StopReason::FullStop); }

  //--------------------------------------------------------------------------
  //! \brief Was the last search stopped because of timeout?
  //! \return true if Stop() was called with reason set to Timeout
  //--------------------------------------------------------------------------
  uint64_t TimeoutOccurred() const { return (_stop & StopReason::Timeout); }

  //--------------------------------------------------------------------------
  //! \brief Stop searching and perform engine exit
  //--------------------------------------------------------------------------
  virtual void Quit() {
    _quit = true;
    _stop = FullStop;
    if (UseTimer()) {
      timerThread.Join();
    }
  }

protected:
  //--------------------------------------------------------------------------
  //! \brief Do performance test on the current position
  //! Perft is useful for determining the speed of you move generator.
  //! Perft is vital for validating whether your move generator is
  //! producing the correct moves.  In any given position, a perft search to
  //! a specific depth should always visit the same number of leaf nodes.
  //! For example, Perft(6) from STARTPOS should yield 119060324.
  //! \param[in] depth How many half-moves (plies) to search
  //! \return The number of leaf nodes visited at \p depth
  //--------------------------------------------------------------------------
  virtual uint64_t MyPerft(const int depth) = 0;

  //--------------------------------------------------------------------------
  //! \brief Execute search on current position to find best move
  //! \param[in] depth Maximum number of half-moves (plies) to search
  //! \param[in] movestogo Number of moves remaining until next time control
  //! \param[in] movetime Maximum milliseconds to spend on this move
  //! \param[in] wtime Milliseconds remaining on white's clock
  //! \param[in] winc White increment per move in milliseconds
  //! \param[in] btime Milliseconds remaining on black's clock
  //! \param[in] binc Black increment per move in milliseconds
  //! \param[out] ponder If not NULL set to the move engine should ponder next
  //! \return Best move in coordinate notation (e.g. "e2e4", "g8f6", "e7f8q")
  //--------------------------------------------------------------------------
  virtual std::string MyGo(const int depth,
                           const int movestogo = 0,
                           const uint64_t movetime = 0,
                           const uint64_t wtime = 0, const uint64_t winc = 0,
                           const uint64_t btime = 0, const uint64_t binc = 0,
                           std::string* ponder = NULL) = 0;

  static void Timer(void* data);
  Thread timerThread;

  static bool     _debug;
  static bool     _searching;
  static bool     _quit;
  static int      _stop;
  static uint64_t _startTime;
  static uint64_t _stopTime;
};

} // namespace senjo

#endif // SENJO_CHESS_ENGINE_H
