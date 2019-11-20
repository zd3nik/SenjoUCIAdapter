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

#ifndef SENJO_CHESS_ENGINE_H
#define SENJO_CHESS_ENGINE_H

#include "EngineOption.h"
#include "GoParams.h"
#include "SearchStats.h"

namespace senjo {

//-----------------------------------------------------------------------------
//! \brief Base class for Senjo compatible chess engines
//! Derive from this class to create a chess engine that may be used with
//! the Senjo UCIAdapter.
//-----------------------------------------------------------------------------
class ChessEngine {
public:
  virtual ~ChessEngine() {}

  //---------------------------------------------------------------------------
  //! \brief FEN string of standard chess start position
  //! See http://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  //---------------------------------------------------------------------------
  static const std::string STARTPOS;

  //---------------------------------------------------------------------------
  //! \brief Get the engine name
  //! \return The engine name
  //---------------------------------------------------------------------------
  virtual std::string getEngineName() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Get the engine version (e.g. "major.minor.build" e.g. "1.0.0")
  //! \return The engine version
  //---------------------------------------------------------------------------
  virtual std::string getEngineVersion() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Get the engine author name(s)
  //! \return The engine author name(s)
  //---------------------------------------------------------------------------
  virtual std::string getAuthorName() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Get email address(es) for use with this engine
  //! Return an empty string if you don't wish to report an email address.
  //! \return An email address(es) for use with this engine
  //---------------------------------------------------------------------------
  virtual std::string getEmailAddress() const;

  //---------------------------------------------------------------------------
  //! \brief Get the name of the country this engine originates from
  //! Return an empty string if you don't wish to report a country
  //! \return The name of the country this engine originates from
  //---------------------------------------------------------------------------
  virtual std::string getCountryName() const;

  //---------------------------------------------------------------------------
  //! \brief Get options supported by the engine, and their current values
  //! \return A list of the engine's options and their current values
  //---------------------------------------------------------------------------
  virtual std::list<EngineOption> getOptions() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Set a particular option to a given value
  //! Option value may be empty, particularly if the option type is Button
  //! \param[in] optionName The option name
  //! \param[in] optionValue The new option value
  //! \return false if the option name or value is invalid
  //---------------------------------------------------------------------------
  virtual bool setEngineOption(const std::string& optionName,
                               const std::string& optionValue) = 0;

  //---------------------------------------------------------------------------
  //! \brief Initialize the engine
  //---------------------------------------------------------------------------
  virtual void initialize() = 0;

  //---------------------------------------------------------------------------
  //! \brief Is the engine initialized?
  //! \return true if the engine is initialized
  //---------------------------------------------------------------------------
  virtual bool isInitialized() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Set the board position according to a given FEN string
  //! The engine should use Output() to report errors in the FEN string.
  //! Only use position info from the given FEN string, don't process any moves
  //! or other data present in the FEN string.
  //! \param[in] fen The FEN string on input
  //! \param[out] remain If not NULL populated with tail portion of \p fen
  //!                    string that was not used to set the position.
  //! \return false if the FEN string does not contain a valid position
  //---------------------------------------------------------------------------
  virtual bool setPosition(const std::string& fen,
                           std::string* remain = nullptr) = 0;

  //---------------------------------------------------------------------------
  //! \brief Execute a single move on the current position
  //! Determine whether the given string is a valid move
  //! and if it is apply the move to the current position.
  //! Moves should be in coordinate notation (e.g. "e2e4", "g8f6", "e7f8q").
  //! \param[in] move A string containing move coordinate notation
  //! \return false if the given string isn't a valid move
  //---------------------------------------------------------------------------
  virtual bool makeMove(const std::string& move) = 0;

  //---------------------------------------------------------------------------
  //! \brief Get a FEN string representation of the current board position
  //! \return A FEN string representation of the current board postiion
  //---------------------------------------------------------------------------
  virtual std::string getFEN() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Output a text representation of the current board position
  //---------------------------------------------------------------------------
  virtual void printBoard() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Is it white to move in the current position?
  //! \return true if it is white to move in the current position
  //---------------------------------------------------------------------------
  virtual bool whiteToMove() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Clear any engine data that can persist between searches
  //! Examples of search data are the transposition table and killer moves.
  //---------------------------------------------------------------------------
  virtual void clearSearchData() = 0;

  //---------------------------------------------------------------------------
  //! \brief The last ponder move was played
  //! The Go() method may return a ponder move which is the expected response
  //! to the bestmove returned by Go().  If pondering is enabled the UCI
  //! adapter may tell the engine to ponder this move, e.g. start searching
  //! for a reply to the ponder move.  If, while the engine is pondering, the
  //! ponder move is played this method will be called.  In general the engine
  //! should make what it has learned from its pondering available for the next
  //! Go() call.
  //---------------------------------------------------------------------------
  virtual void ponderHit() = 0;

  //---------------------------------------------------------------------------
  //! \brief Is the engine registered?
  //! \return true if the engine is registered
  //---------------------------------------------------------------------------
  virtual bool isRegistered() const;

  //---------------------------------------------------------------------------
  //! \brief Register the engine later
  //! The engine should still function, but may cripple itself in some fashion.
  //---------------------------------------------------------------------------
  virtual void registerLater();

  //---------------------------------------------------------------------------
  //! \brief Register the engine now
  //! If this fails the engine should still function, but may cripple itself
  //! in some fashion.
  //! \param[in] name The name to register the engine to
  //! \param[in] code The code to use for registration
  //! \return true if successful
  //---------------------------------------------------------------------------
  virtual bool doRegistration(const std::string& name,
                              const std::string& code);

  //---------------------------------------------------------------------------
  //! \brief Does this engine use copy protection?
  //! \return true if the engine uses copy protection
  //---------------------------------------------------------------------------
  virtual bool isCopyProtected() const;

  //---------------------------------------------------------------------------
  //! \brief Determine whether this is a legitimate copy of the engine
  //! This method will be called if IsCopyProtected() returns true.  This is
  //! where your engine should try to determine whether it is a legitimate
  //! copy or not.
  //! \return true if the engine is a legitimate copy
  //---------------------------------------------------------------------------
  virtual bool copyIsOK();

  //--------------------------------------------------------------------------
  //! \brief Set the engine's debug mode on or off
  //! \param[in] flag true to enable debug mode, false to disable debug mode
  //---------------------------------------------------------------------------
  virtual void setDebug(const bool flag) = 0;

  //---------------------------------------------------------------------------
  //! \brief Is debug mode enabled?
  //! \return true if debug mode is enabled
  //---------------------------------------------------------------------------
  virtual bool isDebugOn() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Is the engine currently executing the Go() method?
  //! It is not recommended to set this to true while Perft() is executing.
  //! \return true if the engine is searching
  //---------------------------------------------------------------------------
  virtual bool isSearching() = 0;

  //---------------------------------------------------------------------------
  //! \brief Tell the engine to stop searching
  //! Exit Perft()/Go() methods as quickly as possible.
  //---------------------------------------------------------------------------
  virtual void stopSearching() = 0;

  //--------------------------------------------------------------------------
  //! \brief Was stopSearching() called after the last go() or perft() call?
  //! \return true if stopSearching() called after the last go() or perft() call
  //--------------------------------------------------------------------------
  virtual bool stopRequested() const = 0;

  //---------------------------------------------------------------------------
  //! \brief Block execution on the calling thread until the engine is
  //! finished searching.  Return immediateky if no search in progress.
  //---------------------------------------------------------------------------
  virtual void waitForSearchFinish() = 0;

  //---------------------------------------------------------------------------
  //! \brief Do performance test on the current position
  //! \param[in] depth How many half-moves (plies) to search
  //! \return The number of leaf nodes visited at \p depth
  //---------------------------------------------------------------------------
  virtual uint64_t perft(const int depth) = 0;

  //---------------------------------------------------------------------------
  //! \brief Execute search on current position to find best move
  //! \param[in] params UCI "go" command parameters
  //! \param[out] ponder If not null set to the move engine should ponder next
  //! \return Best move in coordinate notation (e.g. "e2e4", "g8f6", "e7f8q")
  //---------------------------------------------------------------------------
  virtual std::string go(const GoParams& params,
                         std::string* ponder = nullptr) = 0;

  //--------------------------------------------------------------------------
  //! \brief Get statistics about the last (or current) search
  //! \param[in] count The maximum number of lines to get stats for
  //! \return a SearchStats struct updated with the latest search stats
  //--------------------------------------------------------------------------
  virtual SearchStats getSearchStats() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Reset custom engine statistical counter totals
  //! \remark Override if you desire custom engine stats to be output when
  //!         the "test" command is run.
  //--------------------------------------------------------------------------
  virtual void resetEngineStats();

  //--------------------------------------------------------------------------
  //! \brief Output engine stats collected since last resetEngineStats call
  //! \remark Override if you desire custom engine stats to be output when
  //!         the "test" command is run.
  //--------------------------------------------------------------------------
  virtual void showEngineStats() const;
};

} // namespace senjo

#endif // SENJO_CHESS_ENGINE_H
