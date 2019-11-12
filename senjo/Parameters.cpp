//-----------------------------------------------------------------------------
// Copyright (c) 2019 Shawn Chidester <zd3nik@gmail.com>
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

#include "Parameters.h"

namespace senjo {

//-----------------------------------------------------------------------------
Parameters::Parameters(const std::string& commandLine) {
  parse(commandLine);
}

//-----------------------------------------------------------------------------
void Parameters::parse(const std::string& str) {
  std::stringstream ss(str);
  std::string param;
  clear();
  while (ss >> param) {
    push_back(param);
  }
}

//-----------------------------------------------------------------------------
std::string Parameters::toString() const {
  std::stringstream ss;
  bool first = true;
  for (std::string param : *this) {
    if (first) {
      first = false;
    } else {
      ss << ' ';
    }
    ss << param;
  }
  return ss.str();
}

//-----------------------------------------------------------------------------
bool Parameters::firstParamIs(const std::string& paramName) const {
  return (size() && iEqual(paramName, front()));
}

//-----------------------------------------------------------------------------
bool Parameters::popParam(const std::string& paramName) {
  if (firstParamIs(paramName)) {
    pop_front();
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
bool Parameters::popParam(const std::string& paramName, bool& exists) {
  if (popParam(paramName)) {
    exists = true;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
std::string Parameters::popString() {
  if (empty()) {
    return "";
  }

  std::string str = front();
  pop_front();
  return str;
}

//-----------------------------------------------------------------------------
bool Parameters::popString(const std::string& paramName, std::string& value,
                           const std::string& next)
{
  if ((size() < 2) || !iEqual(paramName, front())) {
    return false;
  }

  pop_front();
  while (size() && (next.empty() || !iEqual(next, front()))) {
    if (value.size()) {
      value += " ";
    }
    value += front();
    pop_front();
  }

  return (value.size() > 0);
}

} // namespace senjo

