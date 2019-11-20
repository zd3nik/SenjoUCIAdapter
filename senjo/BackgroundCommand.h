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

#ifndef SENJO_BACKGROUND_COMMAND_H
#define SENJO_BACKGROUND_COMMAND_H

#include "ChessEngine.h"
#include "Parameters.h"
#include "GoParams.h"
#include "Thread.h"

namespace senjo {

//-----------------------------------------------------------------------------
//! \brief Base class for a command that should run on a background thread
//-----------------------------------------------------------------------------
class BackgroundCommand : public Thread {
public:
  //--------------------------------------------------------------------------
  //! \brief Constructor
  //! \param[in] engine The chess engine to use while executing
  //--------------------------------------------------------------------------
  BackgroundCommand(ChessEngine& chessEngine)
    : engine(chessEngine) {}

  //--------------------------------------------------------------------------
  //! \brief Parse command parameters and execute on the given thread
  //! \param[in] params The command parameters
  //! \return true if parameters are valid and execution started
  //--------------------------------------------------------------------------
  virtual bool parseAndExecute(Parameters& params);

  //--------------------------------------------------------------------------
  //! \brief Provide usage syntax for this command
  //! For example: "command_name [boolean_option_name] [option_name <value>]"
  //! \return Usage syntax string for this command
  //--------------------------------------------------------------------------
  virtual std::string usage() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Provide a brief description of this command
  //! \return A brief description of this command
  //--------------------------------------------------------------------------
  virtual std::string description() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Parse command parameters
  //! \param[in] params The command parameters
  //! \return true if the given parameters are valid
  //--------------------------------------------------------------------------
  virtual bool parse(Parameters& params) = 0;

protected:
  ChessEngine& engine;
};

//-----------------------------------------------------------------------------
//! \brief Wrapper for the UCI "register" command
//-----------------------------------------------------------------------------
class RegisterCommandHandle : public BackgroundCommand {
public:
  RegisterCommandHandle(ChessEngine& eng) : BackgroundCommand(eng) { }
  std::string usage() const {
    return "register [later] [name <x>] [code <x>]";
  }
  std::string description() const {
    return "Register the chess engine to enable full functionality.";
  }
  void stop() {}

protected:
  bool parse(Parameters& params);
  void doWork();

private:
  bool        later;
  std::string code;
  std::string name;
};

//-----------------------------------------------------------------------------
//! \brief Wrapper for the UCI "go" command
//-----------------------------------------------------------------------------
class GoCommandHandle : public BackgroundCommand {
public:
  GoCommandHandle(ChessEngine& eng) : BackgroundCommand(eng) { }
  std::string usage() const {
    return "go [infinite] [ponder] [depth <x>] [nodes <x>] "
        "[wtime <x>] [btime <x>] [winc <x>] [binc <x>] "
        "[movetime <msecs>] [movestogo <x>] [searchmoves <movelist>]";
  }
  std::string description() const {
    return "Find the best move for the current position.";
  }
  void stop() {
    engine.stopSearching();
  }

protected:
  bool parse(Parameters& params);
  void doWork();

private:
  GoParams goParams;
};

//-----------------------------------------------------------------------------
//! \brief Wrapper for the "perft" command (not a UCI command)
//-----------------------------------------------------------------------------
class PerftCommandHandle : public BackgroundCommand {
public:
  PerftCommandHandle(ChessEngine& eng) : BackgroundCommand(eng) { }
  std::string usage() const {
    return ("perft [unsorted] [depth <x>] [count <x>] [skip <x>] [leafs <x>] "
            "[epd] [file <x> (default=" + _TEST_FILE + ")]");
  }
  std::string description() const {
    return "Execute performance test.";
  }
  void stop() {
    engine.stopSearching();
  }

protected:
  bool parse(Parameters& params);
  void doWork();

private:
  bool process(const int depth, const uint64_t expected_leaf_count,
               uint64_t& leaf_count);

  static const std::string _TEST_FILE;

  bool        unsorted;
  int         count;
  int         skip;
  int         maxDepth;
  uint64_t    maxLeafs;
  std::string fileName;
};

//-----------------------------------------------------------------------------
//! \brief Wrapper for the "test" command (not a UCI command)
//-----------------------------------------------------------------------------
class TestCommandHandle : public BackgroundCommand {
public:
  TestCommandHandle(ChessEngine& eng) : BackgroundCommand(eng) { }
  std::string usage() const {
    return "test [print] [skip <x>] [count <x>] [depth <x>] [time <msecs>] "
        "[fail <x>] [file <x> (default=" + _TEST_FILE + ")]";
  }
  std::string description() const {
    return "Find the best move for a suite of test positions.";
  }
  void stop() {
    engine.stopSearching();
  }

protected:
  bool parse(Parameters& params);
  void doWork();

private:
  static const std::string _TEST_FILE;

  bool        noClear;
  bool        printBoard;
  int         maxCount;
  int         maxDepth;
  int         maxFails;
  int         skipCount;
  uint64_t    maxTime;
  std::string fileName;
};

} // namespace senjo

#endif // SENJO_BACKGROUND_COMMAND_H
