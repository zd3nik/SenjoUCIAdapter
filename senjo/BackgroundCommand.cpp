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

#include "BackgroundCommand.h"
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

  const TimePoint start;
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

  TimePoint now;
  double msecs = start.msecsUntil(now);
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

} // namespace senjo
