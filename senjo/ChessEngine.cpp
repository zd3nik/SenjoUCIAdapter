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

#include "ChessEngine.h"

namespace senjo {

//-----------------------------------------------------------------------------
const std::string ChessEngine::STARTPOS =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

//-----------------------------------------------------------------------------
std::string ChessEngine::getEmailAddress() const {
  return "";
}

//-----------------------------------------------------------------------------
std::string ChessEngine::getCountryName() const {
  return "";
}

//-----------------------------------------------------------------------------
bool ChessEngine::isRegistered() const {
  return true;
}

//-----------------------------------------------------------------------------
void ChessEngine::registerLater() {
}

//-----------------------------------------------------------------------------
bool ChessEngine::doRegistration(const std::string& /*name*/,
                                 const std::string& /*code*/)
{
  return true;
}

//-----------------------------------------------------------------------------
bool ChessEngine::isCopyProtected() const {
  return false;
}

//-----------------------------------------------------------------------------
bool ChessEngine::copyIsOK() {
  return true;
}

//-----------------------------------------------------------------------------
void ChessEngine::resetEngineStats() {
}

//-----------------------------------------------------------------------------
void ChessEngine::showEngineStats() const {
}

} // namespace senjo
