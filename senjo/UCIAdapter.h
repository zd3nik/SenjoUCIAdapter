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

#ifndef SENJO_UCI_ADAPTER_H
#define SENJO_UCI_ADAPTER_H

#include "ChessEngine.h"
#include "Parameters.h"
#include "BackgroundCommand.h"

namespace senjo {

//-----------------------------------------------------------------------------
//! \brief Convenience class to handle UCI communication for a ChessEngine
//-----------------------------------------------------------------------------
class UCIAdapter {
public:
  UCIAdapter(ChessEngine& engine);

  //--------------------------------------------------------------------------
  //! \brief Execute the given one-line command
  //! \param[in] cmd The command to execute
  //! \return false when the program should exit, true to continue processing
  //--------------------------------------------------------------------------
  bool doCommand(const std::string& command);

private:
  void doHelpCommand(Parameters& params);
  void doFENCommand(Parameters& params);
  void doMoveCommand(Parameters& params);
  void doNewCommand(Parameters& params);
  void doOptsCommand(Parameters& params);
  void doPrintCommand(Parameters& params);
  bool doQuitCommand(Parameters& params);
  void doDebugCommand(Parameters& params);
  void doIsReadyCommand(Parameters& params);
  void doPonderHitCommand(Parameters& params);
  void doSetOptionCommand(Parameters& params);
  void doStopCommand(Parameters params = {});
  void doUCICommand(Parameters& params);
  void doUCINewGameCommand(Parameters params = {});
  void doPositionCommand(const std::string& line, Parameters& params);
  void execute(std::unique_ptr<BackgroundCommand> command, Parameters& params);

  ChessEngine& engine;
  std::string lastPosition;
  std::unique_ptr<BackgroundCommand> lastCommand;
};

} // namespace senjo

#endif // SENJO_UCI_ADAPTER_H
