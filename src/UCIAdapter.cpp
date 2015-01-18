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

#include "UCIAdapter.h"
#include "BackgroundCommand.h"
#include "Output.h"

namespace senjo {

//----------------------------------------------------------------------------
namespace token
{
  static const std::string Debug("debug");
  static const std::string Exit("exit");
  static const std::string Fen("fen");
  static const std::string Go("go");
  static const std::string Help("help");
  static const std::string IsReady("isready");
  static const std::string Moves("moves");
  static const std::string Name("name");
  static const std::string New("new");
  static const std::string Opts("opts");
  static const std::string Perft("perft");
  static const std::string PonderHit("ponderhit");
  static const std::string Position("position");
  static const std::string Print("print");
  static const std::string Quit("quit");
  static const std::string Register("register");
  static const std::string SetOption("setoption");
  static const std::string StartPos("startpos");
  static const std::string Stop("stop");
  static const std::string Test("test");
  static const std::string Uci("uci");
  static const std::string UciNewGame("ucinewgame");
  static const std::string Value("value");
}

//----------------------------------------------------------------------------
bool UCIAdapter::IsMove(const char* str)
{
  return ((str[0] >= 'a') && (str[0] <= 'h') &&
          (str[1] >= '1') && (str[1] <= '8') &&
          (str[2] >= 'a') && (str[2] <= 'h') &&
          (str[3] >= '1') && (str[3] <= '8') &&
          (!str[4] || isspace(str[4]) ||
           (str[4] == 'n') || (str[4] == 'b') ||
           (str[4] == 'r') || (str[4] == 'q')));
}

//----------------------------------------------------------------------------
UCIAdapter::UCIAdapter()
  : engine(NULL)
{
}

//----------------------------------------------------------------------------
UCIAdapter::~UCIAdapter()
{
  engine = NULL;
}

//----------------------------------------------------------------------------
bool UCIAdapter::Start(ChessEngine& chessEngine)
{
  if (engine) {
    Output() << "UCIAdapter already started";
    return false;
  }

  engine = &chessEngine;
  return true;
}

//----------------------------------------------------------------------------
bool UCIAdapter::DoCommand(const char* command)
{
  if (!engine || !command) {
    StopCommand();
    return false;
  }

  if (!*command) {
    return true;
  }

  if (engine->IsDebugOn()) {
    Output() << "received command: " << command;
  }

  if (ParamMatch(token::Go, command)) {
    StopCommand();
    GoCommand(command);
  }
  else if (ParamMatch(token::Position, command)) {
    StopCommand();
    PositionCommand(command);
  }
  else if (ParamMatch(token::Stop, command)) {
    StopCommand(command);
  }
  else if (ParamMatch(token::SetOption, command)) {
    SetOptionCommand(command);
  }
  else if (ParamMatch(token::IsReady, command)) {
    IsReadyCommand(command);
  }
  else if (ParamMatch(token::Uci, command)) {
    UCICommand(command);
  }
  else if (ParamMatch(token::UciNewGame, command)) {
    StopCommand();
    UCINewGameCommand(command);
  }
  else if (ParamMatch(token::New, command)) {
    StopCommand();
    NewCommand(command);
  }
  else if (ParamMatch(token::Debug, command)) {
    DebugCommand(command);
  }
  else if (ParamMatch(token::Register, command)) {
    StopCommand(command);
    RegisterCommand(command);
  }
  else if (ParamMatch(token::PonderHit, command)) {
    PonderHitCommand(command);
  }
  else if (ParamMatch(token::Fen, command)) {
    FENCommand(command);
  }
  else if (ParamMatch(token::Print, command)) {
    PrintCommand(command);
  }
  else if (ParamMatch(token::Perft, command)) {
    StopCommand();
    PerftCommand(command);
  }
  else if (ParamMatch(token::Test, command)) {
    StopCommand();
    TestCommand(command);
  }
  else if (ParamMatch(token::Opts, command)) {
    OptsCommand(command);
  }
  else if (ParamMatch(token::Help, command)) {
    HelpCommand(command);
  }
  else if (ParamMatch(token::Exit, command) ||
           ParamMatch(token::Quit, command))
  {
    StopCommand();
    if (QuitCommand(command)) {
      return false;
    }
  }
  else if (IsMove(command)) {
    StopCommand();
    MoveCommand(command);
  }
  else {
    Output() << "Unknown command: '" << command << "'";
    Output() << "Enter 'help' for a list of commands";
  }
  return true;
}

//----------------------------------------------------------------------------
//! \brief Output list of available commands (not a UCI command)
//----------------------------------------------------------------------------
void UCIAdapter::HelpCommand(const char* /*params*/)
{
  Output() << engine->GetEngineName() << ' ' << engine->GetEngineVersion()
           << " by " << engine->GetAuthorName();
  Output() << "UCI commands:";
  Output() << "  " << token::Debug;
  Output() << "  " << token::Go;
  Output() << "  " << token::IsReady;
  Output() << "  " << token::Position;
  Output() << "  " << token::Quit;
  Output() << "  " << token::SetOption;
  Output() << "  " << token::Stop;
  Output() << "  " << token::Uci;
  Output() << "  " << token::UciNewGame;
  Output() << "Additional commands:";
  Output() << "  " << token::Exit;
  Output() << "  " << token::Fen;
  Output() << "  " << token::Help;
  Output() << "  " << token::New;
  Output() << "  " << token::Perft;
  Output() << "  " << token::Print;
  Output() << "  " << token::Test;
  Output() << "Also try '<command> help' for help on a specific command";
  Output() << "Or enter move(s) in coordinate notation, e.g. d2d4 g8f6";
}

//----------------------------------------------------------------------------
//! \brief Do the "fen" command (not a UCI command)
//! Print the FEN string for the current board position
//----------------------------------------------------------------------------
void UCIAdapter::FENCommand(const char* params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::Fen;
    Output() << "Output FEN string of the current position.";
    return;
  }

  if (!engine->IsInitialized()) {
    engine->Initialize();
  }
  Output() << engine->GetFEN();
}

//----------------------------------------------------------------------------
//! \brief Do the "print" command (not a UCI command)
//! Output an ascii representation of the current board position
//----------------------------------------------------------------------------
void UCIAdapter::PrintCommand(const char* params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::Print;
    Output() << "Output text representation of the current position.";
    return;
  }

  if (!engine->IsInitialized()) {
    engine->Initialize();
  }
  engine->PrintBoard();
}

//----------------------------------------------------------------------------
//! \brief Do the "new" command (not a UCI command)
//! Clear search data, set position, and apply moves (if any given).
//----------------------------------------------------------------------------
void UCIAdapter::NewCommand(const char* params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::New << " [" << token::StartPos << "|"
             << token::Fen << " <fen_string>] [" << token::Moves
             << " <movelist>]";
    Output() << "Clear search data, set position, and apply <movelist>.";
    Output() << "If no position is specified " << token::StartPos
             << " is assumed.";
    return;
  }

  lastPosition.clear();
  engine->ClearSearchData();

  if (!engine->IsInitialized()) {
    engine->Initialize();
  }
  if (!params || !*params ||
      ParamMatch(token::StartPos, params) ||
      ParamMatch(token::Moves, params))
  {
    if (!engine->SetPosition(ChessEngine::_STARTPOS)) {
      return;
    }
  }
  else {
    ParamMatch(token::Fen, params); // simply consume "fen" token if it present
    if (!(params = engine->SetPosition(params))) {
      return;
    }
  }

  // simply consume "moves" token if present
  ParamMatch(token::Moves, params);

  // apply moves (if any)
  while (params && *NextWord(params)) {
    const char* move = params;
    if (!IsMove(move)) {
      break;
    }
    if (!(params = engine->MakeMove(params))) {
      Output() << "Invalid move: " << move;
      break;
    }
  }

  if (engine->IsDebugOn()) {
    engine->PrintBoard();
  }
}

//----------------------------------------------------------------------------
//! \brief Do the "perft" command (not a UCI command)
//! Execute performance test on current position or a set of test positions
//----------------------------------------------------------------------------
void UCIAdapter::PerftCommand(const char* params)
{
  PerftCommandHandle* handle = new PerftCommandHandle(engine);
  if (!handle) {
    Output() << "Out of memory";
    return;
  }

  if (!params || !*params || ParamMatch(token::Help, params)) {
    Output() << "usage: " << handle->Usage();
    Output() << handle->Description();
  }
  else if (handle->ParseAndExecute(params, thread)) {
    return;
  }

  delete handle;
  handle = NULL;
}

//----------------------------------------------------------------------------
//! \brief Do the "opts" command (not a UCI command)
//! Output current engine option values
//----------------------------------------------------------------------------
void UCIAdapter::OptsCommand(const char* /*params*/)
{
  std::list<EngineOption> opts = engine->GetOptions();
  std::list<EngineOption>::const_iterator it;
  for (it = opts.begin(); it != opts.end(); ++it) {
    const EngineOption& opt = *it;
    switch (opt.GetType()) {
    case EngineOption::Checkbox:
    case EngineOption::Spin:
    case EngineOption::String:
      Output() << opt.GetTypeName() << ':' << opt.GetName() << ' '
               << opt.GetValue();
      break;
    case EngineOption::ComboBox: {
      const std::set<std::string>& values = opt.GetComboValues();
      std::set<std::string>::const_iterator it;
      Output out;
      out << opt.GetTypeName() << ':' << opt.GetName();
      for (it = values.begin(); it != values.end(); ++it) {
        out << ' ' << *it;
      }
    }
    default:
      break;
    }
  }
}

//----------------------------------------------------------------------------
//! \brief Do the "test" command (not a UCI command)
//! Execute problem solving test on current position or a set of test positions
//----------------------------------------------------------------------------
void UCIAdapter::TestCommand(const char* params)
{
  TestCommandHandle* handle = new TestCommandHandle(engine);
  if (!handle) {
    Output() << "Out of memory";
    return;
  }

  if (!params || !*params || ParamMatch(token::Help, params)) {
    Output() << "usage: " << handle->Usage();
    Output() << handle->Description();
  }
  else if (handle->ParseAndExecute(params, thread)) {
    return;
  }

  delete handle;
  handle = NULL;
}

//----------------------------------------------------------------------------
//! \brief Execute the given move(s) on the current position
//----------------------------------------------------------------------------
void UCIAdapter::MoveCommand(const char* params)
{
  lastPosition.clear();
  if (!engine->IsInitialized()) {
    engine->Initialize();
  }
  while (params && *NextWord(params)) {
    const char* move = params;
    if (!IsMove(move) || !(params = engine->MakeMove(params))) {
      Output() << "Invalid move: " << move;
      return;
    }
    if (engine->IsDebugOn()) {
      engine->PrintBoard();
    }
  }
}

//----------------------------------------------------------------------------
//! \brief Do the "exit" command (not a UCI command)
//! \return true if exit requested, otherwise false
//----------------------------------------------------------------------------
bool UCIAdapter::ExitCommand(const char *params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::Exit;
    Output() << "Stop engine and terminate program.";
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
//! \brief Do the UCI "quit" command
//! UCI specification:
//!   Quit the program as soon as possible.
//! \return true if quit requested, otherwise false
//----------------------------------------------------------------------------
bool UCIAdapter::QuitCommand(const char *params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::Quit;
    Output() << "Stop engine and terminate program.";
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
//! \brief Do the UCI "debug" command
//! UCI specification:
//!   Switch the debug mode of the engine on and off.  In debug mode the engine
//!   should send additional infos to the GUI, e.g. with the "info string"
//!   command, to help debugging, e.g. the commands that the engine has
//!   received etc.  This mode should be switched off by default and this
//!   command can be sent any time, also when the engine is thinking.
//----------------------------------------------------------------------------
void UCIAdapter::DebugCommand(const char* params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::Debug;
    Output() << "Toggle debug mode.";
    return;
  }

  engine->SetDebug(!engine->IsDebugOn());
  Output() << "debug " << (engine->IsDebugOn() ? "on" : "off");
}

//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void UCIAdapter::IsReadyCommand(const char* params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::IsReady;
    Output() << "Output readyok when engine is ready to receive input.";
    return;
  }

  if (!engine->IsInitialized()) {
    engine->Initialize();
  }
  Output(Output::NoPrefix) << "readyok";
}

//----------------------------------------------------------------------------
//! \brief Do the UCI "stop" command
//! UCI specification:
//!   Stop calculating as soon as possible, don't forget the "bestmove" and
//!   possibly the "ponder" token when finishing the search.
//----------------------------------------------------------------------------
void UCIAdapter::StopCommand(const char* params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::Stop;
    Output() << "Stop engine if it is calculating.";
    return;
  }

  engine->Stop(ChessEngine::FullStop);
  thread.Join();
}

//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void UCIAdapter::UCICommand(const char* params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::Uci;
    Output() << "Output engine info and options followed by uciok.";
    return;
  }

  std::list<EngineOption> opts = engine->GetOptions();
  Output(Output::NoPrefix) << "id " << engine->GetEngineName()
                           << ' ' << engine->GetEngineVersion()
                           << ' ' << engine->GetAuthorName();

  std::list<EngineOption>::const_iterator opt;
  for (opt = opts.begin(); opt != opts.end(); ++opt) {
    Output out(Output::NoPrefix);
    out << "option name " << opt->GetName() << " type " << opt->GetTypeName();
    if (opt->GetDefaultValue().size()) {
      out << " default " << opt->GetDefaultValue();
    }
    if (opt->GetMinValue() > INT64_MIN) {
      out << " min " << opt->GetMinValue();
    }
    if (opt->GetMaxValue() < INT64_MAX) {
      out << " max " << opt->GetMaxValue();
    }
  }

  Output(Output::NoPrefix) << "uciok";

  if (engine->IsCopyProtected()) {
    Output(Output::NoPrefix) << "copyprotection checking";
    if (engine->CopyIsOK()) {
      Output(Output::NoPrefix) << "copyprotection ok";
    }
    else {
      Output(Output::NoPrefix) << "copyprotection error";
    }
  }

  if (!engine->IsRegistered()) {
    Output(Output::NoPrefix) << "registration error";
  }
}

//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void UCIAdapter::UCINewGameCommand(const char* params)
{
  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::UciNewGame;
    Output() << "Clear all search data.";
    return;
  }

  if (!engine->IsInitialized()) {
    engine->Initialize();
  }
  lastPosition.clear();
  engine->ClearSearchData();
}

//----------------------------------------------------------------------------
//! \brief Do the UCI "position" command
//! UCI specification:
//!   Set up the position described in fenstring on the internal board and
//!   play the moves on the internal chess board.  If the game was played from
//!   the start position the string "startpos" will be sent.
//!   Note: no "new" command is needed.  However, if this position is from a
//!   different game than the last position sent to the engine, the GUI should
//!   have sent a "ucinewgame" inbetween.
//----------------------------------------------------------------------------
void UCIAdapter::PositionCommand(const char* params)
{
  if (!params || !*params || ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::Position << " {" << token::StartPos << "|"
             << token::Fen << " <fen_string>} [<movelist>]";
    Output() << "Set a new position and apply <movelist> (if given).";
    return;
  }

  const char* position = params;
  if (lastPosition.size() &&
      !strncmp(params, lastPosition.c_str(), lastPosition.size()))
  {
    NextWord(params += lastPosition.size());
  }
  else {
    if (ParamMatch(token::StartPos, params)) {
      if (!engine->SetPosition(ChessEngine::_STARTPOS)) {
        return;
      }
    }
    else {
      ParamMatch(token::Fen, params);
      params = engine->SetPosition(params);
    }
  }

  // simply consume "moves" token if present
  ParamMatch(token::Moves, params);

  // apply moves (if any)
  while (params && *NextWord(params)) {
    const char* move = params;
    if (!IsMove(move)) {
      break;
    }
    if (!(params = engine->MakeMove(params))) {
      Output() << "Invalid move: " << move;
      break;
    }
  }

  lastPosition = params ? position : "";
  if (engine->IsDebugOn()) {
    engine->PrintBoard();
  }
}

//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void UCIAdapter::SetOptionCommand(const char* params)
{
  if (!params || !*params || ParamMatch(token::Help, params)) {
    Output() << "usage: " << token::SetOption << ' ' << token::Name
             << " <option_name> [" << token::Value << " <option_value>]";
    Output() << "Set the value of the specified option name.";
    Output() << "If no value specified the option's default value is used,";
    Output() << "or the option will be triggered if it's a button option.";
    return;
  }

  std::string name;
  std::string value;

  if (!ParamMatch(token::Name, params)) {
    Output() << "Missing name token";
    return;
  }
  if (!ParamValue(name, params, token::Value)) {
    Output() << "Missing name value";
    return;
  }

  if (ParamMatch(token::Value, params)) {
    if (!ParamValue(value, params, "")) {
      Output() << "Missing value";
      return;
    }
  }

  if (*params) {
    Output() << "Unexpected token: " << params;
    return;
  }

  if (!engine->SetEngineOption(name, value)) {
    Output() << "Unknown option name '" << name
             << "' or invalid option value '" << value << "'";
  }
}

//----------------------------------------------------------------------------
//! \brief Do the UCI "go" command
//! UCI specification:
//! 	Start calculating on the current position set up with the "position"
//!   command.  There are a number of commands that can follow this command,
//!   all will be sent in the same string.  If one command is not sent its
//!   value should be interpreted as it would not influence the search.
//! 	* searchmoves <move1> .... <movei>
//!     Restrict search to this moves only.  Example: After "position startpos"
//!     and "go infinite searchmoves e2e4 d2d4" the engine should only search
//!     the two moves e2e4 and d2d4 in the initial position.
//! 	* ponder
//! 	  Start searching in pondering mode.  Do not exit the search in ponder
//!     mode, even if it's mate!  This means that the last move sent in in the
//!     position string is the ponder move.  The engine can do what it wants to
//!     do, but after a "ponderhit" command it should execute the suggested
//!     move to ponder on. This means that the ponder move sent by the GUI can
//!     be interpreted as a recommendation about which move to ponder. However,
//!     if the engine decides to ponder on a different move, it should not
//!     display any mainlines as they are likely to be misinterpreted by the
//!     GUI because the GUI expects the engine to ponder on the suggested move.
//! 	* wtime <x>
//! 	  White has x msec left on the clock.
//! 	* btime <x>
//! 	  Black has x msec left on the clock.
//! 	* winc <x>
//! 	  White increment per move in mseconds if x > 0.
//! 	* binc <x>
//! 	  Black increment per move in mseconds if x > 0.
//! 	* movestogo <x>
//! 	  There are x moves to the next time control, this will only be sent
//!     if x > 0, if you don't get this and get the wtime and btime it's
//!     sudden death
//! 	* depth <x>
//! 	  Search x plies only.
//! 	* nodes <x>
//! 	  Search x nodes only.
//! 	* mate <x>
//! 	  Search for a mate in x moves.
//! 	* movetime <x>
//! 	  Search exactly x mseconds.
//! 	* infinite
//! 	  Search until the "stop" command. Do not exit the search without being
//!     told so in this mode!
//----------------------------------------------------------------------------
void UCIAdapter::GoCommand(const char* params)
{
  GoCommandHandle* handle = new GoCommandHandle(engine);
  if (!handle) {
    Output() << "Out of memory";
    return;
  }

  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << handle->Usage();
    Output() << handle->Description();
  }
  else if (handle->ParseAndExecute(params, thread)) {
    return;
  }

  delete handle;
  handle = NULL;
}

//----------------------------------------------------------------------------
//! \brief Do the UCI "register" command
//! UCI specification:
//!   This is the command to try to register an engine or to tell the engine
//!   that registration will be done later. This command should always be sent
//!   if the engine has sent "registration error" at program startup.
//!   The following tokens are allowed:
//!   * later
//!      The user doesn't want to register the engine now.
//!   * name <x>
//!      The engine should be registered with the name <x>
//!   * code <y>
//!      The engine should be registered with the code <y>
//!   Example:
//!      "register later"
//!      "register name Stefan MK code 4359874324"
//----------------------------------------------------------------------------
void UCIAdapter::RegisterCommand(const char* params)
{
  RegisterCommandHandle* handle = new RegisterCommandHandle(engine);
  if (!handle) {
    Output() << "Out of memory";
    return;
  }

  if (ParamMatch(token::Help, params)) {
    Output() << "usage: " << handle->Usage();
    Output() << handle->Description();
  }
  else if (handle->ParseAndExecute(params, thread)) {
    return;
  }

  delete handle;
  handle = NULL;
}

//----------------------------------------------------------------------------
//! \brief Do the UCI "ponderhit" command
//! UCI specification:
//! 	The user has played the expected move. This will be sent if the engine
//!   was told to ponder on the same move the user has played. The engine
//!   should continue searching but switch from pondering to normal search.
//----------------------------------------------------------------------------
void UCIAdapter::PonderHitCommand(const char* /*params*/)
{
  engine->PonderHit();
}

} // namespace senjo
