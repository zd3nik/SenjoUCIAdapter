//-----------------------------------------------------------------------------
// Copyright (c) 2015-2022 Shawn Chidester <zd3nik@gmail.com>
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

#include "UCIAdapter.h"
#include "Output.h"

namespace senjo {

//-----------------------------------------------------------------------------
struct Token {
  static const std::string Debug;
  static const std::string Exit;
  static const std::string Fen;
  static const std::string Go;
  static const std::string Help;
  static const std::string IsReady;
  static const std::string Moves;
  static const std::string Name;
  static const std::string New;
  static const std::string Opts;
  static const std::string Perft;
  static const std::string PonderHit;
  static const std::string Position;
  static const std::string Print;
  static const std::string Quit;
  static const std::string Register;
  static const std::string SetOption;
  static const std::string StartPos;
  static const std::string Stop;
  static const std::string Uci;
  static const std::string UciNewGame;
  static const std::string Value;
};

//-----------------------------------------------------------------------------
const std::string Token::Debug("debug");
const std::string Token::Exit("exit");
const std::string Token::Fen("fen");
const std::string Token::Go("go");
const std::string Token::Help("help");
const std::string Token::IsReady("isready");
const std::string Token::Moves("moves");
const std::string Token::Name("name");
const std::string Token::New("new");
const std::string Token::Opts("opts");
const std::string Token::Perft("perft");
const std::string Token::PonderHit("ponderhit");
const std::string Token::Position("position");
const std::string Token::Print("print");
const std::string Token::Quit("quit");
const std::string Token::Register("register");
const std::string Token::SetOption("setoption");
const std::string Token::StartPos("startpos");
const std::string Token::Stop("stop");
const std::string Token::Uci("uci");
const std::string Token::UciNewGame("ucinewgame");
const std::string Token::Value("value");

//-----------------------------------------------------------------------------
UCIAdapter::UCIAdapter(ChessEngine& chessEngine)
  : engine(chessEngine) {}

//-----------------------------------------------------------------------------
bool UCIAdapter::doCommand(const std::string& line) {
  Parameters params(line);
  if (params.empty()) {
    return true; // ignore empty lines
  }

  if (engine.isDebugOn()) {
    Output() << "received command: " << line;
  }

  std::string command = params.popString();
  if (iEqual(Token::Go, command)) {
    doStopCommand();
    execute(std::unique_ptr<BackgroundCommand>(new GoCommandHandle(engine)), params);
  }
  else if (iEqual(Token::Position, command)) {
    doStopCommand();
    doPositionCommand(line, params);
  }
  else if (iEqual(Token::Stop, command)) {
    doStopCommand(params);
  }
  else if (iEqual(Token::SetOption, command)) {
    doSetOptionCommand(params);
  }
  else if (iEqual(Token::IsReady, command)) {
    doIsReadyCommand(params);
  }
  else if (iEqual(Token::Uci, command)) {
    doUCICommand(params);
  }
  else if (iEqual(Token::UciNewGame, command)) {
    doStopCommand();
    doUCINewGameCommand(params);
  }
  else if (iEqual(Token::New, command)) {
    doStopCommand();
    doNewCommand(params);
  }
  else if (iEqual(Token::Debug, command)) {
    doDebugCommand(params);
  }
  else if (iEqual(Token::Register, command)) {
    doStopCommand(params);
    execute(std::unique_ptr<BackgroundCommand>(new RegisterCommandHandle(engine)), params);
  }
  else if (iEqual(Token::PonderHit, command)) {
    doPonderHitCommand(params);
  }
  else if (iEqual(Token::Fen, command)) {
    doFENCommand(params);
  }
  else if (iEqual(Token::Print, command)) {
    doPrintCommand(params);
  }
  else if (iEqual(Token::Perft, command)) {
    doStopCommand();
    execute(std::unique_ptr<BackgroundCommand>(new PerftCommandHandle(engine)), params);
  }
  else if (iEqual(Token::Opts, command)) {
    doOptsCommand(params);
  }
  else if (iEqual(Token::Help, command)) {
    doHelpCommand(params);
  }
  else if (iEqual(Token::Exit, command) ||
           iEqual(Token::Quit, command))
  {
    if (doQuitCommand(params)) {
      return false;
    }
  }
  else if (isMove(command)) {
    doStopCommand();
    params.push_front(command);
    doMoveCommand(params);
  }
  else {
    Output() << "Unknown command: '" << command << "'";
    Output() << "Enter 'help' for a list of commands";
  }
  return true;
}

//-----------------------------------------------------------------------------
//! \brief Output list of available commands (not a UCI command)
//-----------------------------------------------------------------------------
void UCIAdapter::doHelpCommand(Parameters& /*params*/) {
  Output() << engine.getEngineName() << ' ' << engine.getEngineVersion()
           << " by " << engine.getAuthorName();
  Output() << "UCI commands:";
  Output() << "  " << Token::Debug;
  Output() << "  " << Token::Go;
  Output() << "  " << Token::IsReady;
  Output() << "  " << Token::Position;
  Output() << "  " << Token::Quit;
  Output() << "  " << Token::SetOption;
  Output() << "  " << Token::Stop;
  Output() << "  " << Token::Uci;
  Output() << "  " << Token::UciNewGame;
  Output() << "Additional commands:";
  Output() << "  " << Token::Exit;
  Output() << "  " << Token::Fen;
  Output() << "  " << Token::Help;
  Output() << "  " << Token::New;
  Output() << "  " << Token::Perft;
  Output() << "  " << Token::Print;
  Output() << "Also try '<command> help' for help on a specific command";
  Output() << "Or enter move(s) in coordinate notation, e.g. d2d4 g8f6";
}

//-----------------------------------------------------------------------------
//! \brief Execute the given background command thread
//-----------------------------------------------------------------------------
void UCIAdapter::execute(std::unique_ptr<BackgroundCommand> command,
                         Parameters& params)
{
  if (!command) {
    return;
  }

  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << command->usage();
    Output() << command->description();
    return;
  }

  if (lastCommand) {
    lastCommand->stop();
    lastCommand->waitForFinish();
  }

  if (command->parseAndExecute(params)) {
    lastCommand.swap(command);
  }
}

//-----------------------------------------------------------------------------
//! \brief Do the "fen" command (not a UCI command)
//! Print the FEN string for the current board position
//-----------------------------------------------------------------------------
void UCIAdapter::doFENCommand(Parameters& params) {
  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::Fen;
    Output() << "Output FEN string of the current position.";
    return;
  }

  if (lastCommand) {
    lastCommand->stop();
    lastCommand->waitForFinish();
  }

  if (!engine.isInitialized()) {
    engine.initialize();
  }

  Output() << engine.getFEN();
}

//-----------------------------------------------------------------------------
//! \brief Do the "print" command (not a UCI command)
//! Output an ascii representation of the current board position
//-----------------------------------------------------------------------------
void UCIAdapter::doPrintCommand(Parameters& params) {
  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::Print;
    Output() << "Output text representation of the current position.";
    return;
  }

  if (!engine.isInitialized()) {
    engine.initialize();
  }

  engine.printBoard();
}

//-----------------------------------------------------------------------------
//! \brief Do the "new" command (not a UCI command)
//! Clear search data, set position, and apply moves (if any given).
//-----------------------------------------------------------------------------
void UCIAdapter::doNewCommand(Parameters& params) {
  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::New << " [" << Token::StartPos << "|"
             << Token::Fen << " <fen_string>] [" << Token::Moves
             << "] <movelist>";
    Output() << "Clear search data, set position, and apply <movelist>.";
    Output() << "If no position is specified " << Token::StartPos
             << " is assumed.";
    return;
  }

  doUCINewGameCommand();

  if (params.empty() ||
      params.popParam(Token::StartPos) ||
      params.popParam(Token::Moves))
  {
    if (!engine.setPosition(ChessEngine::STARTPOS)) {
      return;
    }
  }
  else {
    // consume "fen" token if present
    params.popParam(Token::Fen);
    std::string remain;
    if (!engine.setPosition(params.toString(), &remain)) {
      return;
    }
    params.parse(remain);
  }

  // consume "moves" token if present
  params.popParam(Token::Moves);

  // apply moves (if any)
  while (params.size() && isMove(params.front())) {
    std::string move = params.popString();
    if (!engine.makeMove(move)) {
      Output() << "Invalid move: " << move;
      break;
    }
  }

  if (engine.isDebugOn()) {
    engine.printBoard();
  }
}

//-----------------------------------------------------------------------------
//! \brief Do the "opts" command (not a UCI command)
//! Output current engine option values
//-----------------------------------------------------------------------------
void UCIAdapter::doOptsCommand(Parameters& /*params*/) {
  for (auto&& opt : engine.getOptions()) {
    switch (opt.getType()) {
    case EngineOption::Checkbox:
    case EngineOption::Spin:
    case EngineOption::String:
      Output() << opt.getTypeName() << ':' << opt.getName() << ' '
               << opt.getValue();
      break;
    case EngineOption::ComboBox: {
      const std::set<std::string>& values = opt.getComboValues();
      Output out;
      out << opt.getTypeName() << ':' << opt.getName();
      for (auto&& value : values) {
        out << ' ' << value;
      }
      break;
    }
    default:
      break;
    }
  }
}

//-----------------------------------------------------------------------------
//! \brief Execute the given move(s) on the current position
//-----------------------------------------------------------------------------
void UCIAdapter::doMoveCommand(Parameters& params) {
  if (!engine.isInitialized()) {
    engine.initialize();
  }

  if (lastCommand) {
    lastCommand->stop();
    lastCommand->waitForFinish();
  }

  lastPosition.clear();
  while (params.size()) {
    std::string move = params.popString();
    if (!isMove(move) || !engine.makeMove(move)) {
      Output() << "Invalid move: " << move;
      return;
    }

    if (engine.isDebugOn()) {
      engine.printBoard();
    }
  }
}

//-----------------------------------------------------------------------------
//! \brief Do the UCI "quit" command
//! UCI specification:
//!   Quit the program as soon as possible.
//! \return true if quit requested, otherwise false
//-----------------------------------------------------------------------------
bool UCIAdapter::doQuitCommand(Parameters& params) {
  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::Quit;
    Output() << "Stop engine and terminate program.";
    return false;
  }

  engine.stopSearching();

  if (lastCommand) {
    lastCommand->stop();
    lastCommand->waitForFinish();
  }

  return true;
}

//-----------------------------------------------------------------------------
//! \brief Do the UCI "debug" command
//! UCI specification:
//!   Switch the debug mode of the engine on and off.  In debug mode the engine
//!   should send additional infos to the GUI, e.g. with the "info string"
//!   command, to help debugging, e.g. the commands that the engine has
//!   received etc.  This mode should be switched off by default and this
//!   command can be sent any time, also when the engine is thinking.
//-----------------------------------------------------------------------------
void UCIAdapter::doDebugCommand(Parameters& params) {
  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::Debug;
    Output() << "Toggle debug mode.";
    return;
  }

  engine.setDebug(!engine.isDebugOn());
  Output() << "debug " << (engine.isDebugOn() ? "on" : "off");
}

//-----------------------------------------------------------------------------
//! \brief Do the UCI "isready" command
//! UCI specification:
//!   This is used to synchronize the engine with the GUI.  When the GUI has
//!   sent a command or multiple commands that can take some time to complete,
//!   this command can be used to wait for the engine to be ready again or
//!   to ping the engine to find out if it is still alive.  E.g. this should be
//!   sent after setting the path to the tablebases as this can take some time.
//!   This command is also required once before the engine is asked to do any
//!   search to wait for the engine to finish initializing.  This command must
//!   always be answered with "readyok" and can be sent also when the engine
//!   is calculating in which case the engine should also immediately answer
//!   with "readyok" without stopping the search.
//-----------------------------------------------------------------------------
void UCIAdapter::doIsReadyCommand(Parameters& params) {
  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::IsReady;
    Output() << "Output readyok when engine is ready to receive input.";
    return;
  }

  if (!engine.isInitialized()) {
    engine.initialize();
  }

  if (lastCommand) {
    // don't set stop flag
    lastCommand->waitForFinish();
  }

  Output(Output::NoPrefix) << "readyok";
}

//-----------------------------------------------------------------------------
//! \brief Do the UCI "stop" command
//! UCI specification:
//!   Stop calculating as soon as possible, don't forget the "bestmove" and
//!   possibly the "ponder" token when finishing the search.
//-----------------------------------------------------------------------------
void UCIAdapter::doStopCommand(Parameters params) {
  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::Stop;
    Output() << "Stop engine if it is calculating.";
    return;
  }

  engine.stopSearching();
}

//-----------------------------------------------------------------------------
//! \brief Do the UCI "uci" command
//! UCI specification:
//!   Tell engine to use the uci (universal chess interface), this will be sent
//!   once as a first command after program boot to tell the engine to switch
//!   to uci mode.  After receiving the uci command the engine must identify
//!   itself with the "id" command and send the "option" commands to tell the
//!   GUI which engine settings the engine supports if any.  After that the
//!   engine should send "uciok" to acknowledge the uci mode.  If no uciok is
//!   sent within a certain time period, the engine task will be killed by the
//!   GUI.
//-----------------------------------------------------------------------------
void UCIAdapter::doUCICommand(Parameters& params) {
  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::Uci;
    Output() << "Output engine info and options followed by uciok.";
    return;
  }

  Output(Output::NoPrefix) << "id name " << engine.getEngineName()
                           << ' ' << engine.getEngineVersion();

  const std::string author = engine.getAuthorName();
  if (author.size()) {
    Output(Output::NoPrefix) << "id author " << author;
  }

  const std::string email = engine.getEmailAddress();
  if (email.size()) {
    Output(Output::NoPrefix) << "id email " << email;
  }

  const std::string country = engine.getCountryName();
  if (country.size()) {
    Output(Output::NoPrefix) << "id country " << country;
  }

  for (auto&& opt : engine.getOptions()) {
    Output out(Output::NoPrefix);
    out << "option name " << opt.getName() << " type " << opt.getTypeName();
    if (opt.getDefaultValue().size()) {
      out << " default " << opt.getDefaultValue();
    }
    if (opt.getMinValue() > INT64_MIN) {
      out << " min " << opt.getMinValue();
    }
    if (opt.getMaxValue() < INT64_MAX) {
      out << " max " << opt.getMaxValue();
    }
    for (auto&& val : opt.getComboValues()) {
      out << " var " << val;
    }
  }

  Output(Output::NoPrefix) << "uciok";

  if (engine.isCopyProtected()) {
    Output(Output::NoPrefix) << "copyprotection checking";
    if (engine.copyIsOK()) {
      Output(Output::NoPrefix) << "copyprotection ok";
    }
    else {
      Output(Output::NoPrefix) << "copyprotection error";
    }
  }

  if (!engine.isRegistered()) {
    Output(Output::NoPrefix) << "registration error";
  }
}

//-----------------------------------------------------------------------------
//! \brief Do the UCI "ucinewgame" command
//! UCI specification:
//!   This is sent to the engine when the next search (started with "position"
//!   and "go") will be from a different game.  This can be a new game the
//!   engine should play or a new game it should analyse but also the next
//!   position from a testsuite with positions only.  If the GUI hasn't sent a
//!   "ucinewgame" before the first "position" command, the engine shouldn't
//!   expect any further ucinewgame commands as the GUI is probably not
//!   supporting the ucinewgame command.  So the engine should not rely on this
//!   command even though all new GUIs should support it.  As the engine's
//!   reaction to "ucinewgame" can take some time the GUI should always send
//!   "isready" after "ucinewgame" to wait for the engine to finish its
//!   operation.
//-----------------------------------------------------------------------------
void UCIAdapter::doUCINewGameCommand(Parameters params) {
  if (params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::UciNewGame;
    Output() << "Clear all search data.";
    return;
  }

  if (!engine.isInitialized()) {
    engine.initialize();
  }

  if (lastCommand) {
    lastCommand->stop();
    lastCommand->waitForFinish();
  }

  lastPosition.clear();
  engine.clearSearchData();
}

//-----------------------------------------------------------------------------
//! \brief Do the UCI "position" command
//! UCI specification:
//!   Set up the position described in fenstring on the internal board and
//!   play the moves on the internal chess board.  If the game was played from
//!   the start position the string "startpos" will be sent.
//!   Note: no "new" command is needed.  However, if this position is from a
//!   different game than the last position sent to the engine, the GUI should
//!   have sent a "ucinewgame" inbetween.
//-----------------------------------------------------------------------------
void UCIAdapter::doPositionCommand(const std::string& fenstring,
                                   Parameters& params)
{
  if (params.empty() || params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::Position << " {" << Token::StartPos << "|"
             << Token::Fen << " <fen_string>} [<movelist>]";
    Output() << "Set a new position and apply <movelist> (if given).";
    return;
  }

  if (!engine.isInitialized()) {
    engine.initialize();
    lastPosition.clear();
  }

  if (lastCommand) {
    lastCommand->stop();
    lastCommand->waitForFinish();
  }

  if (lastPosition.size() &&
      lastPosition == fenstring.substr(0, lastPosition.size()))
  {
    // continue from current position
    params.parse(fenstring.substr(lastPosition.size() + 1));
  }
  else {
    if (params.firstParamIs(Token::StartPos)) {
      if (!engine.setPosition(ChessEngine::STARTPOS)) {
        return;
      }
    }
    else {
      // consume "fen" token if present
      params.popParam(Token::Fen);
      std::string remain;
      if (!engine.setPosition(params.toString(), &remain)) {
        return;
      }
      params.parse(remain);
    }
  }

  // remember this position command for next time
  lastPosition = fenstring;

  // consume "moves" token if present
  params.popParam(Token::Moves);

  // apply moves (if any)
  while (params.size() && isMove(params.front())) {
    std::string move = params.popString();
    if (!engine.makeMove(move)) {
      Output() << "Invalid move: " << move;
      lastPosition.clear();
      break;
    }
  }

  if (engine.isDebugOn()) {
    engine.printBoard();
  }
}

//-----------------------------------------------------------------------------
//! \brief Do the UCI "setoption" command
//! UCI specification:
//!   This is sent to the engine when the user wants to change the internal
//!   parameters of the engine.  For the "button" type no value is needed.
//!   One string will be sent for each parameter and this will only be sent
//!   when the engine is waiting.  The name and value of the option in <id>
//!   should not be case sensitive and can inlude spaces.  The substrings
//!   "value" and "name" should be avoided in <id> and <x> to allow unambiguous
//!   parsing, for example do not use <name> = "draw value".
//!   Here are some strings for the example below:
//!      "setoption name Nullmove value true\n"
//!      "setoption name Selectivity value 3\n"
//!      "setoption name Style value Risky\n"
//!      "setoption name Clear Hash\n"
//!      "setoption name NalimovPath value c:\chess\tb\4;c:\chess\tb\5\n"
//-----------------------------------------------------------------------------
void UCIAdapter::doSetOptionCommand(Parameters& params) {
  if (params.empty() || params.firstParamIs(Token::Help)) {
    Output() << "usage: " << Token::SetOption << ' ' << Token::Name
             << " <option_name> [" << Token::Value << " <option_value>]";
    Output() << "Set the value of the specified option name.";
    Output() << "If no value specified the option's default value is used,";
    Output() << "or the option will be triggered if it's a button option.";
    return;
  }

  std::string name;
  std::string value;

  if (!params.firstParamIs(Token::Name)) {
    Output() << "Missing name token";
    return;
  }

  if (!params.popString(Token::Name, name, Token::Value)) {
    Output() << "Missing name value";
    return;
  }

  if (params.firstParamIs(Token::Value)) {
    if (!params.popString(Token::Value, value)) {
      Output() << "Missing value";
      return;
    }
  }

  if (params.size()) {
    Output() << "Unexpected token: " << params.front();
    return;
  }

  if (!engine.setEngineOption(name, value)) {
    Output() << "Unknown option name '" << name
             << "' or invalid option value '" << value << "'";
  }
}

//-----------------------------------------------------------------------------
//! \brief Do the UCI "ponderhit" command
//! UCI specification:
//! 	The user has played the expected move. This will be sent if the engine
//!   was told to ponder on the same move the user has played. The engine
//!   should continue searching but switch from pondering to normal search.
//-----------------------------------------------------------------------------
void UCIAdapter::doPonderHitCommand(Parameters& /*params*/) {
  engine.ponderHit();
}

} // namespace senjo
