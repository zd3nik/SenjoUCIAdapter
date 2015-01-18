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

#ifndef SENJO_UCI_ADAPTER_H
#define SENJO_UCI_ADAPTER_H

#include "ChessEngine.h"
#include "Threading.h"

namespace senjo {

//----------------------------------------------------------------------------
//! \brief Convenience class to handle UCI communication for a ChessEngine
//----------------------------------------------------------------------------
class UCIAdapter
{
public:
  //--------------------------------------------------------------------------
  //! \brief Does the given string begin with a valid move?
  //! \return true if \p str begins with a coordinate notation chess move
  //--------------------------------------------------------------------------
  static bool IsMove(const char* str);

  //--------------------------------------------------------------------------
  //! \brief Constructor
  //--------------------------------------------------------------------------
  UCIAdapter();

  //--------------------------------------------------------------------------
  //! \brief Destructor
  //--------------------------------------------------------------------------
  virtual ~UCIAdapter();

  //--------------------------------------------------------------------------
  //! \brief Start/Initialize the UCI adapter with a new engine
  //! \param[in] engine The engine the UCIAdapter should use
  //! \return true if started successfully
  //--------------------------------------------------------------------------
  bool Start(ChessEngine& engine);

  //--------------------------------------------------------------------------
  //! \brief Execute the given one-line command
  //! \param[in] cmd The command to execute
  //! \return false when the program should exit, true to continue processing
  //--------------------------------------------------------------------------
  bool DoCommand(const char* command);

private:
  // custom commands
  bool ExitCommand(const char* params);
  void FENCommand(const char* params);
  void HelpCommand(const char* params);
  void MoveCommand(const char* params);
  void NewCommand(const char* params);
  void OptsCommand(const char* params);
  void PerftCommand(const char* params);
  void PonderHitCommand(const char* params);
  void PrintCommand(const char* params);
  void RegisterCommand(const char* params);
  void TestCommand(const char* params);

  // UCI commands
  bool QuitCommand(const char* params);
  void DebugCommand(const char* params);
  void GoCommand(const char* params);
  void IsReadyCommand(const char* params);
  void PositionCommand(const char* params);
  void SetOptionCommand(const char* params);
  void StopCommand(const char* params = "");
  void UCICommand(const char* params);
  void UCINewGameCommand(const char* params);

  ChessEngine* engine;
  Thread       thread;
  std::string  lastPosition;
};

} // namespace senjo

#endif // SENJO_UCI_ADAPTER_H
