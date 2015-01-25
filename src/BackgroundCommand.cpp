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

#include "BackgroundCommand.h"
#include "MoveFinder.h"
#include "Output.h"

namespace senjo {

//----------------------------------------------------------------------------
bool BackgroundCommand::ParseAndExecute(const char* params, Thread& thread)
{
  if (!engine) {
    Output() << "Engine not set for background command";
    return false;
  }

  if (!Parse(params)) {
    return false;
  }

  if (thread.Active()) {
    Output() << "Another background command is still active, can't execute";
    return false;
  }

  if (!engine->IsInitialized()) {
    engine->Initialize();
  }

  return thread.Start(BackgroundCommand::Run, this);
}

//----------------------------------------------------------------------------
void BackgroundCommand::Run(void* data)
{
  BackgroundCommand* cmd = NULL;
  try {
    if (!data) {
      Output() << "BackgroundCommand::Run() NULL data parameter";
      return;
    }

#ifdef NDEBUG
    cmd = static_cast<BackgroundCommand*>(data);
#else
    cmd = reinterpret_cast<BackgroundCommand*>(data);
    if (!cmd) {
      // NOTE: potential memory leak if this happens
      Output() << "BackgroundCommand::Run() failed to cast data parameter";
      return;
    }
#endif

    cmd->Execute();
  }
  catch (const std::exception& e) {
    std::cerr << "BackgrounThread::Run() ERROR: "
              << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "BackgrounThread::Run() ERROR: unknown exception"
              << std::endl;
  }

  if (cmd) {
    delete cmd;
    cmd = NULL;
  }
}

//----------------------------------------------------------------------------
bool RegisterCommandHandle::Parse(const char* params)
{
  static const std::string argCode  = "code";
  static const std::string argLater = "later";
  static const std::string argName  = "name";

  later = false;
  code  = "";
  name  = "";

  if (ParamMatch(argLater, params)) {
    later = true;
  }

  if (ParamMatch(argName, params)) {
    if (!ParamValue(name, params, argCode)) {
      Output() << "Missing name value";
      return false;
    }
  }

  if (ParamMatch(argCode, params)) {
    if (!ParamValue(code, params, "")) {
      Output() << "Missing code value";
      return false;
    }
  }

  if (params && *params) {
    Output() << "Unexpected token: " << params;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
void RegisterCommandHandle::Execute()
{
  static const std::string registrationOK = "registration ok";

  Output(Output::NoPrefix) << "registration checking";
  if (engine->IsRegistered()) {
    Output(Output::NoPrefix) << registrationOK;
  }
  else if (later) {
    engine->RegisterLater();
    Output(Output::NoPrefix) << registrationOK;
  }
  else if (engine->Register(name, code)) {
    Output(Output::NoPrefix) << registrationOK;
  }
  else {
    Output(Output::NoPrefix) << "registration error";
  }
}

//----------------------------------------------------------------------------
bool GoCommandHandle::Parse(const char* params)
{
  static const std::string argBinc        = "binc";
  static const std::string argBtime       = "btime";
  static const std::string argDepth       = "depth";
  static const std::string argInfinite    = "infinite";
  static const std::string argMovestogo   = "movestogo";
  static const std::string argMovetime    = "movetime";
  static const std::string argNodes       = "nodes";
  static const std::string argPonder      = "ponder";
  static const std::string argSearchmoves = "searchmoves";
  static const std::string argWinc        = "winc";
  static const std::string argWtime       = "wtime";

  infinite  = false;
  ponder    = false;
  depth     = 0;
  movestogo = 0;
  binc      = 0;
  btime     = 0;
  movetime  = 0;
  nodes     = 0;
  winc      = 0;
  wtime     = 0;

  bool invalid = false;
  while (!invalid && params && *NextWord(params)) {
    if (ParamMatch(argSearchmoves, params)) {
      Output() << "searchmoves not implemented!"; // TODO
      break;
    }
    if (HasParam(argInfinite,     infinite,  params) ||
        HasParam(argPonder,       ponder,    params) ||
        NumberParam(argDepth,     depth,     params, invalid) ||
        NumberParam(argMovestogo, movestogo, params, invalid) ||
        NumberParam(argBinc,      binc,      params, invalid) ||
        NumberParam(argBtime,     btime,     params, invalid) ||
        NumberParam(argMovetime,  movetime,  params, invalid) ||
        NumberParam(argNodes,     nodes,     params, invalid) ||
        NumberParam(argWinc,      winc,      params, invalid) ||
        NumberParam(argWtime,     wtime,     params, invalid))
    {
      continue;
    }
    Output() << "Unexpected token: " << params;
    return false;
  }
  if (invalid) {
    Output() << "usage: " << Usage();
    return false;
  }

  if (infinite || ponder) {
    depth     = 0;
    movestogo = 0;
    binc      = 0;
    btime     = 0;
    movetime  = 0;
    nodes     = 0;
    winc      = 0;
    wtime     = 0;
  }
  else {
    // convert movetime to milliseconds
    movetime *= 1000;
  }

  return true;
}

//----------------------------------------------------------------------------
void GoCommandHandle::Execute()
{
  if (!engine) {
    Output() << "Engine not set for 'go' command";
    return;
  }

  std::string ponder; // NOTE: shadows this->ponder
  std::string bestmove =
      engine->Go(depth, movestogo, movetime, wtime, winc, btime, binc, &ponder);

  if (bestmove.empty()) {
    bestmove = "none";
    ponder.clear();
  }

  if (ponder.size()) {
    Output(Output::NoPrefix) << "bestmove " << bestmove << " ponder " << ponder;
  }
  else {
    Output(Output::NoPrefix) << "bestmove " << bestmove;
  }
}

//----------------------------------------------------------------------------
const std::string PerftCommandHandle::_TEST_FILE = "epd/perftsuite.epd";

//----------------------------------------------------------------------------
bool PerftCommandHandle::Parse(const char* params)
{
  static const std::string argCount = "count";
  static const std::string argDepth = "depth";
  static const std::string argEpd   = "epd";
  static const std::string argFile  = "file";
  static const std::string argLeafs = "leafs";
  static const std::string argSkip  = "skip";

  count    = 0;
  skip     = 0;
  maxDepth = 0;
  maxLeafs = 0;
  fileName = "";

  bool epd = false;
  bool invalid = false;
  while (!invalid && params && *NextWord(params)) {
    if (HasParam(argEpd,      epd,      params) ||
        NumberParam(argCount, count,    params, invalid) ||
        NumberParam(argSkip,  skip,     params, invalid) ||
        NumberParam(argDepth, maxDepth, params, invalid) ||
        NumberParam(argLeafs, maxLeafs, params, invalid) ||
        StringParam(argFile,  fileName, params, invalid))
    {
      continue;
    }
    Output() << "Unexpected token: " << params;
    return false;
  }
  if (invalid) {
    Output() << "usage: " << Usage();
    return false;
  }

  if (epd && fileName.empty()) {
    fileName = _TEST_FILE;
  }

  return true;
}

//----------------------------------------------------------------------------
void PerftCommandHandle::Execute()
{
  if (!engine) {
    Output() << "Engine not set for 'perft' command";
    return;
  }

  if (fileName.empty()) {
    engine->Perft(maxDepth);
    return;
  }

  FILE* fp = NULL;
  try {
    if (!(fp = fopen(fileName.c_str(), "r"))) {
      Output() << "Cannot open '" << fileName << "': " << strerror(errno);
      return;
    }

    const uint64_t start = Now();
    uint64_t leafs = 0;
    uint64_t nodes = 0;
    bool done = false;
    char fen[16384];
    int positions = 0;

    for (int line = 1; !done && fgets(fen, sizeof(fen), fp); ++line) {
      char* f = fen;
      if (!*NextWord(f) || (*f == '#')) {
        continue;
      }

      positions++;
      if ((skip > 0) && (positions <= skip)) {
        continue;
      }

      Output() << "Perft line " << line << ' ' << f;
      NormalizeString(f);
      if (!(f = const_cast<char*>(engine->SetPosition(f)))) {
        break;
      }

      while (f && *f) {
        // null terminate this parameter (parameters end with ; or end of line)
        char* end = strchr(f, ';');
        if (end) {
          *end = 0;
        }

        // process "D<depth> <leafs>" parameters (e.g. D5 4865609)
        if ((*NextWord(f) == 'D') && isdigit(f[1])) {
          if (!Process(f, leafs, nodes)) {
            done = true;
            break;
          }
        }

        // move 'f' to beginning of next parameter
        if (end) {
          f = (end + 1);
          continue;
        }
        break;
      }

      if ((count > 0) && (positions >= count)) {
        break;
      }
    }

    const uint64_t time = (Now() - start);
    Output() << "Perft "
             << Rate((leafs / 1000), time) << " KLeafs/sec "
             << Rate((nodes / 1000), time) << " KNodes/sec";
  }
  catch (const std::exception& e) {
    Output() << "ERROR: " << e.what();
  }
  catch (...) {
    Output() << "Unknown error!";
  }

  if (fp) {
    fclose(fp);
    fp = NULL;
  }
}

//----------------------------------------------------------------------------
//! \brief Perform perft search, \p params format = 'D<depth> <leafs>'
//! \param[in] params The parameter string
//! \param[out] leafs Incremented by the number of leaf nodes visited
//! \param[out] nodes Incremented by the number of nodes visited
//! \return false if leaf nodes does not match expected count
//----------------------------------------------------------------------------
bool PerftCommandHandle::Process(const char* params,
                                 uint64_t& leafs, uint64_t& nodes)
{
  const char* p = (params + 1);
  int depth = 0;
  while (*p && isdigit(*p)) {
    depth = ((10 * depth) + (*p++ - '0'));
    if (depth < 0) {
      Output() << "invalid depth: " << params;
      return true;
    }
  }
  if (!depth || (maxDepth && (depth > maxDepth))) {
    return true;
  }

  NextWord(p);

  uint64_t expected = 0;
  while (*p && isdigit(*p)) {
    expected = ((10 * expected) + (*p++ - '0'));
  }
  if (!expected || (maxLeafs && (expected > maxLeafs))) {
    return true;
  }

  Output() << "--- " << depth << " => " << expected;
  uint64_t leaf_count = engine->Perft(depth);
  uint64_t node_count = 0;

  engine->GetStats(NULL, NULL, &node_count);
  leafs += leaf_count;
  nodes += node_count;

  if (leaf_count != expected) {
    Output() << "--- " << leaf_count << " != " << expected;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
const std::string TestCommandHandle::_TEST_FILE = "epd/test.epd";

//----------------------------------------------------------------------------
bool TestCommandHandle::Parse(const char* params)
{
  static const std::string argCount   = "count";
  static const std::string argDepth   = "depth";
  static const std::string argFile    = "file";
  static const std::string argGain    = "gain";
  static const std::string argNoClear = "noclear";
  static const std::string argPrint   = "print";
  static const std::string argSkip    = "skip";
  static const std::string argTime    = "time";

  noClear    = false;
  printBoard = false;
  maxCount   = 0;
  maxDepth   = 0;
  minGain    = 0;
  skipCount  = 0;
  maxTime    = 0;
  fileName   = "";

  bool invalid = false;
  while (!invalid && params && *NextWord(params)) {
    if (HasParam(argNoClear,  noClear,    params) ||
        HasParam(argPrint,    printBoard, params) ||
        NumberParam(argCount, maxCount,   params, invalid) ||
        NumberParam(argDepth, maxDepth,   params, invalid) ||
        NumberParam(argGain,  minGain,    params, invalid) ||
        NumberParam(argSkip,  skipCount,  params, invalid) ||
        NumberParam(argTime,  maxTime,    params, invalid) ||
        StringParam(argFile,  fileName,   params, invalid))
    {
      continue;
    }
    Output() << "Unexpected token: " << params;
    return false;
  }
  if (invalid) {
    Output() << "usage: " << Usage();
    return false;
  }

  if (fileName.empty()) {
    fileName = _TEST_FILE;
  }

  return true;
}

//----------------------------------------------------------------------------
void TestCommandHandle::Execute()
{
  if (!engine) {
    Output() << "Engine not set for 'test' command";
    return;
  }

  if (fileName.empty()) {
    Output() << "FileName not set for 'test' command";
    return;
  }

  char     fen[16384];
  int      depth = 0;
  int      line = 0;
  int      maxSearchDepth = 0;
  int      maxSeldepth = 0;
  int      minSearchDepth = -1;
  int      minSeldepth = -1;
  int      passed = 0;
  int      positions = 0;
  int      seldepth = 0;
  int      tested = 0;
  int      totalDepth = 0;
  int      totalSeldepth = 0;
  uint64_t nodes = 0;
  uint64_t qnodes = 0;
  uint64_t time = 0;
  uint64_t totalNodes = 0;
  uint64_t totalQnodes = 0;
  uint64_t totalTime = 0;
  FILE*    fp = NULL;

  try {
    MoveFinder moveFinder;

    if (!(fp = fopen(fileName.c_str(), "r"))) {
      Output() << "Cannot open '" << fileName << "': " << strerror(errno);
      return;
    }

    while (fgets(fen, sizeof(fen), fp)) {
      line++;

      char* f = fen;
      if (!*NextWord(f) || (*f == '#')) {
        continue;
      }

      positions++;
      if (skipCount && (positions <= skipCount)) {
        continue;
      }

      Output() << "--- Test " << (++tested) << " at line " << line << ' ' << f;
      NormalizeString(f);
      const char* next = engine->SetPosition(f);
      if (!next || !moveFinder.LoadFEN(f)) {
        break;
      }
      f += (next - f);

      // consume 'am' and 'bm' parameters
      std::set<std::string> avoid;
      std::set<std::string> best;
      while (f && *NextWord(f)) {
        // null terminate this parameter (parameters end with ; or end of line)
        char* end = strchr(f, ';');
        if (end) {
          *end = 0;
        }

        if (!strncmp(f, "am ", 3)) {
          f += 3;
          while (*NextWord(f)) {
            std::string coord = moveFinder.ToCoordinates(f);
            if (coord.size()) {
              avoid.insert(coord);
            }
            else {
              break;
            }
          }
        }
        else if (!strncmp(f, "bm ", 3)) {
          f += 3;
          while (*NextWord(f)) {
            std::string coord = moveFinder.ToCoordinates(f);
            if (coord.size()) {
              best.insert(coord);
            }
            else {
              break;
            }
          }
        }

        // move 'f' to beginning of next parameter
        if (end) {
          f = (end + 1);
          continue;
        }
        break;
      }

      if (avoid.empty() && best.empty()) {
        Output() << "error at line " << line
                 << ", no best or avoid moves specified";
        break;
      }

      if (!noClear) {
        engine->ClearSearchData();
      }
      if (printBoard) {
        engine->PrintBoard();
      }

      const std::string bestmove = engine->Go(maxDepth, 0, maxTime);
      Output(Output::NoPrefix) << "bestmove " << bestmove;

      engine->GetStats(&depth, &seldepth, &nodes, &qnodes, &time);
      if (bestmove.empty() ||
          (best.size() && !best.count(bestmove)) ||
          (avoid.size() && avoid.count(bestmove)))
      {
        Output() << "--- FAILED! line " << line << " ("
                 << Percent(passed, tested) << "%) " << f;
      }
      else {
        passed++;
        Output() << "--- Passed. line " << line << " ("
                 << Percent(passed, tested) << "%) " << f;
      }

      if (depth > maxSearchDepth) {
        maxSearchDepth = depth;
      }
      if ((minSearchDepth < 0) || (depth < minSearchDepth)) {
        minSearchDepth = depth;
      }
      if (seldepth > maxSeldepth) {
        maxSeldepth = seldepth;
      }
      if ((minSeldepth < 0) || (seldepth < minSeldepth)) {
        minSeldepth = seldepth;
      }
      totalDepth += depth;
      totalNodes += nodes;
      totalQnodes += qnodes;
      totalSeldepth += seldepth;
      totalTime += time;

      if (engine->StopRequested() || (maxCount && (tested >= maxCount))) {
        break;
      }
    }

    Output() << "--- Complete " << Rate((totalNodes / 1000), totalTime)
             << " KNodes/sec";
    Output() << "--- Nodes    " << totalNodes;
    Output() << "--- QNodes   " << totalQnodes << " ("
             << Percent(totalQnodes, totalNodes) << "%)";
    Output() << "--- Time     " << totalTime << " ("
             << Average(totalTime, static_cast<uint64_t>(tested)) << "/pos)";
    Output() << "--- Depth    " << minSearchDepth << ", "
             << static_cast<int>(Average(totalDepth, tested)) << ", "
             << maxSearchDepth;
    Output() << "--- SelDepth " << minSeldepth << ", "
             << static_cast<int>(Average(totalSeldepth, tested)) << ", "
             << maxSeldepth;
    Output() << "--- Passed   " << passed << '/' << tested << " ("
             << Percent(passed, tested) << "%)";
  }
  catch (const std::exception& e) {
    Output() << "ERROR: " << e.what();
  }
  catch (...) {
    Output() << "Unknown error!";
  }

  if (fp) {
    fclose(fp);
    fp = NULL;
  }
}

} // namespace senjo
