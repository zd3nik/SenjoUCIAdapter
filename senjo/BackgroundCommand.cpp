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

#include "BackgroundCommand.h"
#include "MoveFinder.h"
#include "Output.h"
#include <fstream>

namespace senjo {

//-----------------------------------------------------------------------------
bool BackgroundCommand::parseAndExecute(Parameters& params) {
  if (!parse(params)) {
    return false;
  }

  if (isRunning()) {
    Output() << "Another background command is still active, can't execute";
    return false;
  }

  if (!engine.isInitialized()) {
    engine.initialize();
  }

  return run();
}

//-----------------------------------------------------------------------------
bool RegisterCommandHandle::parse(Parameters& params) {
  later = false;
  code  = "";
  name  = "";

  params.popParam("later", later);
  params.popString("name", name, "code");
  params.popString("code", code);

  if (params.size()) {
    Output() << "Unexpected token: " << params.front();
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
void RegisterCommandHandle::doWork() {
  static const std::string registrationOK = "registration ok";

  Output(Output::NoPrefix) << "registration checking";
  if (engine.isRegistered()) {
    Output(Output::NoPrefix) << registrationOK;
  }
  else if (later) {
    engine.registerLater();
    Output(Output::NoPrefix) << registrationOK;
  }
  else if (engine.doRegistration(name, code)) {
    Output(Output::NoPrefix) << registrationOK;
  }
  else {
    Output(Output::NoPrefix) << "registration error";
  }
}

//-----------------------------------------------------------------------------
bool GoCommandHandle::parse(Parameters& params) {
  goParams = GoParams(); // reset all params to default values

  bool invalid = false;
  while (!invalid && params.size()) {
    if (params.firstParamIs("searchmoves")) {
      Output() << "searchmoves not implemented!"; // TODO
      break;
    }
    if (params.popParam("infinite", goParams.infinite) ||
        params.popParam("ponder", goParams.ponder) ||
        params.popNumber("depth", goParams.depth) ||
        params.popNumber("movestogo", goParams.movestogo) ||
        params.popNumber("binc", goParams.binc) ||
        params.popNumber("btime", goParams.btime) ||
        params.popNumber("movetime",  goParams.movetime) ||
        params.popNumber("nodes", goParams.nodes) ||
        params.popNumber("winc", goParams.winc) ||
        params.popNumber("wtime", goParams.wtime))
    {
      continue;
    }
    Output() << "Unexpected token: " << params.front();
    return false;
  }

  if (invalid) {
    Output() << "usage: " << usage();
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
void GoCommandHandle::doWork() {
  std::string ponderMove;
  std::string bestMove = engine.go(goParams, &ponderMove);

  if (bestMove.empty()) {
    bestMove = "none";
    ponderMove.clear();
  }

  if (ponderMove.size()) {
    Output(Output::NoPrefix) << "bestmove " << bestMove
                             << " ponder " << ponderMove;
  }
  else {
    Output(Output::NoPrefix) << "bestmove " << bestMove;
  }
}

//-----------------------------------------------------------------------------
const std::string PerftCommandHandle::_TEST_FILE = "epd/perftsuite.epd";

//-----------------------------------------------------------------------------
bool PerftCommandHandle::parse(Parameters& params) {
  count    = 0;
  skip     = 0;
  maxDepth = 0;
  maxLeafs = 0;
  fileName = "";

  bool epd = false;
  bool invalid = false;

  while (params.size() && !invalid) {
    if (params.popParam("epd", epd) ||
        params.popNumber("count", count, invalid) ||
        params.popNumber("skip",  skip, invalid) ||
        params.popNumber("depth", maxDepth, invalid) ||
        params.popNumber("leafs", maxLeafs, invalid) ||
        params.popString("file",  fileName))
    {
      continue;
    }
    Output() << "Unexpected token: " << params.front();
    return false;
  }

  if (invalid) {
    Output() << "usage: " << usage();
    return false;
  }

  if (epd && fileName.empty()) {
    fileName = _TEST_FILE;
  }

  return true;
}

//-----------------------------------------------------------------------------
void PerftCommandHandle::doWork() {
  if (fileName.empty()) {
    engine.perft(maxDepth);
    return;
  }

  std::ifstream fs(fileName);

  const TimePoint start = now();
  uint64_t pcount = 0;
  bool done = false;
  int positions = 0;
  int line = 0;

  std::string fen;
  fen.reserve(16384);

  while (!done && std::getline(fs, fen)) {
    line++;

    size_t i = fen.find_first_not_of(" \t\r\n");
    if ((i == std::string::npos) || (fen[i] == '#')) {
      continue;
    }

    positions++;
    if ((skip > 0) && (positions <= skip)) {
      continue;
    }

    Output() << fileName << " line " << line << ' ' << fen;
    std::string remain;
    if (!engine.setPosition(fen, &remain)) {
      break;
    }

    // process "D<depth> <leafs>" parameters (e.g. D5 4865609)
    Parameters params(remain);
    while (!done && params.size()) {
      std::string depthToken = trim(params.popString(), " ;");
      if (depthToken.empty() || (depthToken.at(0) != 'D')) {
        continue;
      }

      int depth = toNumber<int>(depthToken.substr(1));
      if (depth < 1) {
        Output() << "--- invalid depth: " << depthToken;
        break;
      }

      if (params.empty()) {
        Output() << "--- missing expected leaf count";
        break;
      }

      uint64_t leafs = params.popNumber<uint64_t>();
      if (leafs < 1) {
        Output() << "--- invalid expected leaf count";
        break;
      }

      done |= !process(depth, leafs, pcount);
    }

    done |= ((count > 0) && (positions >= count));
  }

  double msecs = getMsecs(start, now());
  double kLeafs = (double(pcount) / 1000);

  Output() << "Total Perft " << pcount << ' '
           << rate(kLeafs, msecs) << " KLeafs/sec";
}

//-----------------------------------------------------------------------------
//! \brief Perform [q]perft search, \p params format = 'D<depth> <leafs>'
//! \param[in] depth The depth to search to
//! \param[in] expected_leaf_count The expected leaf count at \p depth
//! \param[out] leaf_count The actual leaf count at \p depth
//! \return false if leaf_count count does not match expected leaf count
//-----------------------------------------------------------------------------
bool PerftCommandHandle::process(const int depth,
                                 const uint64_t expected_leaf_count,
                                 uint64_t& leaf_count)
{
  if ((maxDepth > 0) && (depth > maxDepth)) {
    return true;
  }

  if ((maxLeafs > 0) && (expected_leaf_count > maxLeafs)) {
    return true;
  }

  Output() << "--- " << depth << " => " << expected_leaf_count;
  uint64_t perft_count = engine.perft(depth);
  leaf_count += perft_count;

  if (perft_count != expected_leaf_count) {
    Output() << "--- " << perft_count << " != " << expected_leaf_count;
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
const std::string TestCommandHandle::_TEST_FILE = "epd/test.epd";

//-----------------------------------------------------------------------------
bool TestCommandHandle::parse(Parameters& params) {
  noClear    = false;
  printBoard = false;
  maxCount   = 0;
  maxDepth   = 0;
  maxFails   = 0;
  skipCount  = 0;
  maxTime    = 0;
  fileName   = "";

  bool invalid = false;
  while (params.size() && !invalid) {
    if (params.popParam("noclear", noClear) ||
        params.popParam("print", printBoard) ||
        params.popNumber("count", maxCount, invalid) ||
        params.popNumber("depth", maxDepth, invalid) ||
        params.popNumber("fail",  maxFails, invalid) ||
        params.popNumber("skip",  skipCount, invalid) ||
        params.popNumber("time",  maxTime, invalid) ||
        params.popString("file",  fileName))
    {
      continue;
    }
    Output() << "Unexpected token: " << params.front();
    return false;
  }

  if (invalid) {
    Output() << "usage: " << usage();
    return false;
  }

  if (fileName.empty()) {
    fileName = _TEST_FILE;
  }

  return true;
}

//-----------------------------------------------------------------------------
void TestCommandHandle::doWork() {
  if (fileName.empty()) {
    Output() << "FileName not set for 'test' command";
    return;
  }

  std::ifstream fs(fileName);

  struct FailedTest {
    std::string bestmove;
    std::string fen;
    int line;
  };

  int      line = 0;
  int      maxSearchDepth = 0;
  int      maxSeldepth = 0;
  int      minSearchDepth = -1;
  int      minSeldepth = -1;
  int      passed = 0;
  int      positions = 0;
  int      tested = 0;
  int      totalDepth = 0;
  int      totalSeldepth = 0;
  uint64_t totalNodes = 0;
  uint64_t totalQnodes = 0;
  uint64_t totalTime = 0;
  MoveFinder moveFinder;
  std::list<FailedTest> failed;

  std::string fen;
  fen.reserve(16384);

  engine.resetEngineStats();

  while (std::getline(fs, fen)) {
    line++;

    size_t i = fen.find_first_not_of(" \t\r\n");
    if ((i == std::string::npos) || (fen[i] == '#')) {
      continue;
    }

    positions++;
    if (skipCount && (positions <= skipCount)) {
      continue;
    }

    Output() << "--- Test " << (++tested) << " at line " << line << ' ' << fen;
    std::string remain;
    if (!moveFinder.loadFEN(fen) || !engine.setPosition(fen, &remain)) {
      break;
    }

    // consume 'am' and 'bm' parameters
    Parameters params(remain);
    std::set<std::string> avoid;
    std::set<std::string> best;
    while (params.size()) {
      if (params.popParam("am")) {
        while (params.size()) {
          std::string coord = moveFinder.toCoordinates(params.front());
          if (coord.size()) {
            params.pop_front();
            avoid.insert(coord);
          }
          else {
            break;
          }
        }
      }
      else if (params.popParam("bm")) {
        while (params.size()) {
          std::string coord = moveFinder.toCoordinates(params.front());
          if (coord.size()) {
            params.pop_front();
            best.insert(coord);
          }
          else {
            break;
          }
        }
      }
      else {
        params.pop_front();
      }
    }

    if (avoid.empty() && best.empty()) {
      Output() << "error at line " << line
               << ", no best or avoid moves specified";
      break;
    }

    if (!noClear) {
      engine.clearSearchData();
    }
    if (printBoard && !engine.isDebugOn()) {
      engine.printBoard();
    }

    GoParams goParams;
    goParams.depth = maxDepth;
    goParams.movetime = maxTime;

    const std::string bestmove = engine.go(goParams);
    SearchStats stats = engine.getSearchStats();
    Output(Output::NoPrefix) << "bestmove " << bestmove;

    if (bestmove.empty() ||
        (best.size() && !best.count(bestmove)) ||
        (avoid.size() && avoid.count(bestmove)))
    {
      Output() << "--- FAILED! line " << line << " ("
               << percent(passed, tested) << "%) " << fen;

      failed.push_back(FailedTest{bestmove, fen, line});
      if ((maxFails > 0) && (failed.size() >= size_t(maxFails))) {
        break;
      }
    }
    else {
      passed++;
      Output() << "--- Passed. line " << line << " ("
               << percent(passed, tested) << "%) " << fen;
    }

    maxSearchDepth = std::max<int>(maxSearchDepth, stats.depth);
    maxSeldepth = std::max<int>(maxSeldepth, stats.seldepth);
    if ((minSearchDepth < 0) || (stats.depth < minSearchDepth)) {
      minSearchDepth = stats.depth;
    }
    if ((minSeldepth < 0) || (stats.seldepth < minSeldepth)) {
      minSeldepth = stats.seldepth;
    }
    totalDepth += stats.depth;
    totalNodes += stats.nodes;
    totalQnodes += stats.qnodes;
    totalSeldepth += stats.seldepth;
    totalTime += stats.msecs;

    if (engine.stopRequested() || (maxCount && (tested >= maxCount))) {
      break;
    }
  }

  Output() << "--- Completed " << tested << " test positions";
  Output() << "--- Passed    " << passed << " passed ("
           << percent(passed, tested) << "%)";
  Output() << "--- Time      " << totalTime << " ("
           << average(totalTime, static_cast<uint64_t>(tested)) << " avg)";
  Output() << "--- Nodes     " << totalNodes << ", "
           << rate((totalNodes / 1000), totalTime) << " KNodes/sec";
  Output() << "--- QNodes    " << totalQnodes << " ("
           << percent(totalQnodes, totalNodes) << "%)";
  Output() << "--- Depth     " << minSearchDepth << " min, "
           << static_cast<int>(average(totalDepth, tested)) << " avg, "
           << maxSearchDepth << " max";
  Output() << "--- SelDepth  " << minSeldepth << " min, "
           << static_cast<int>(average(totalSeldepth, tested)) << " avg, "
           << maxSeldepth << " max";
  Output() << "--- Everaged Engine Statistics ---";
  engine.showEngineStats();

  for (const auto& fail : failed) {
    Output() << "--- Failed line " << fail.line << ' ' << fail.fen;
    Output() << "--- Engine move: " << fail.bestmove;
    if (printBoard || engine.isDebugOn()) {
      engine.setPosition(fail.fen);
      engine.printBoard();
    }
  }
}

} // namespace senjo
