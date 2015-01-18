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

#ifndef SENJO_BACKGROUND_COMMAND_H
#define SENJO_BACKGROUND_COMMAND_H

#include "ChessEngine.h"
#include "Output.h"

namespace senjo
{

//----------------------------------------------------------------------------
//! \brief Base class for a command that should run on a background thread
//----------------------------------------------------------------------------
class BackgroundCommand
{
public:
  //--------------------------------------------------------------------------
  //! \brief Constructor
  //! \param[in] engine The chess engine to use while executing
  //--------------------------------------------------------------------------
  BackgroundCommand(ChessEngine* engine) : engine(engine) { }

  //--------------------------------------------------------------------------
  //! \brief Parse command parameters and execute on the given thread
  //! \param[in] params The command parameters
  //! \param[in] thread The thread to execute the command on
  //! \return true if parameters are valid and execution started
  //--------------------------------------------------------------------------
  virtual bool ParseAndExecute(const char* params, Thread& thread);

  //--------------------------------------------------------------------------
  //! \brief Provide usage syntax for this command
  //! For example: "command_name [boolean_option_name] [option_name <value>]"
  //! \return Usage syntax string for this command
  //--------------------------------------------------------------------------
  virtual std::string Usage() const = 0;

  //--------------------------------------------------------------------------
  //! \brief Provide a brief description of this command
  //! \return A brief description of this command
  //--------------------------------------------------------------------------
  virtual std::string Description() const = 0;

protected:
  //--------------------------------------------------------------------------
  //! \brief Parse command parameters
  //! \param[in] params The command parameters
  //! \return true if the given parameters are valid
  //--------------------------------------------------------------------------
  virtual bool Parse(const char* params) = 0;

  //--------------------------------------------------------------------------
  //! \brief Main method that performs the command
  //! This will be run on a background thread
  //--------------------------------------------------------------------------
  virtual void Execute() = 0;

  ChessEngine* engine;

private:
  //--------------------------------------------------------------------------
  //! \brief Execute given background command object
  //! \param[in] data A pointer to the BackgroundCommand object to execute
  //--------------------------------------------------------------------------
  static void Run(void* data);
};

//----------------------------------------------------------------------------
//! \brief Wrapper for the UCI "register" command
//----------------------------------------------------------------------------
class RegisterCommandHandle : public BackgroundCommand
{
public:
  RegisterCommandHandle(ChessEngine* engine) : BackgroundCommand(engine) { }
  std::string Usage() const {
    return "register [later] [name <x>] [code <x>]";
  }
  std::string Description() const {
    return "Register the chess engine to enable full functionality.";
  }

protected:
  bool Parse(const char* params);
  void Execute();

private:
  bool        later;
  std::string code;
  std::string name;
};

//----------------------------------------------------------------------------
//! \brief Wrapper for the UCI "go" command
//----------------------------------------------------------------------------
class GoCommandHandle : public BackgroundCommand
{
public:
  GoCommandHandle(ChessEngine* engine) : BackgroundCommand(engine) { }
  std::string Usage() const {
    return "go [infinite] [ponder] [depth <x>] [nodes <x>] "
        "[wtime <x>] [btime <x>] [winc <x>] [binc <x>] "
        "[movetime <secs>] [movestogo <x>] [searchmoves <movelist>]";
  }
  std::string Description() const {
    return "Find the best move for the current position.";
  }

protected:
  bool Parse(const char* params);
  void Execute();

private:
  bool     infinite;
  bool     ponder;
  int      depth;
  int      movestogo;
  uint64_t binc;
  uint64_t btime;
  uint64_t movetime;
  uint64_t nodes;
  uint64_t winc;
  uint64_t wtime;
};

//----------------------------------------------------------------------------
//! \brief Wrapper for the "perft" command (not a UCI command)
//----------------------------------------------------------------------------
class PerftCommandHandle : public BackgroundCommand
{
public:
  PerftCommandHandle(ChessEngine* engine) : BackgroundCommand(engine) { }
  std::string Usage() const {
    return "perft [depth <x>] [count <x>] [skip <x>] [leafs <x>] "
        "[epd] [file <x> (default=" + _TEST_FILE + ")]";
  }
  std::string Description() const {
    return "Execute performance test.";
  }

protected:
  bool Parse(const char* params);
  void Execute();

private:
  bool Process(const char* params, uint64_t& leafs, uint64_t& nodes);

  static const std::string _TEST_FILE;

  int         count;
  int         skip;
  int         maxDepth;
  uint64_t    maxLeafs;
  std::string fileName;
};

//----------------------------------------------------------------------------
//! \brief Wrapper for the "test" command (not a UCI command)
//----------------------------------------------------------------------------
class TestCommandHandle : public BackgroundCommand
{
public:
  TestCommandHandle(ChessEngine* engine) : BackgroundCommand(engine) { }
  std::string Usage() const {
    return "test [print] [skip <x>] [count <x>] [depth <x>] [time <msecs>] "
        "[gain <x>] [file <x> (default=" + _TEST_FILE + ")]";
  }
  std::string Description() const {
    return "Find the best move for a suite of test positions.";
  }

protected:
  bool Parse(const char* params);
  void Execute();

private:
  static const std::string _TEST_FILE;

  bool        noClear;
  bool        printBoard;
  int         maxCount;
  int         maxDepth;
  int         minGain;
  int         skipCount;
  uint64_t    maxTime;
  std::string fileName;
};

} // namespace senjo

#endif // SENJO_BACKGROUND_COMMAND_H
