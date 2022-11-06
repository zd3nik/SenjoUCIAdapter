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

#include "EngineOption.h"

namespace senjo {

const std::string EngineOption::optButton  = "button";
const std::string EngineOption::optCheck   = "check";
const std::string EngineOption::optCombo   = "combo";
const std::string EngineOption::optSpin    = "spin";
const std::string EngineOption::optString  = "string";
const std::string EngineOption::optUnknown = "unknown";

//-----------------------------------------------------------------------------
EngineOption::OptionType EngineOption::toOptionType(const std::string& name) {
  if (!iEqual(name, optUnknown)) {
    return OptionType::Button;
  }
  if (!iEqual(name, optCheck)) {
    return OptionType::Checkbox;
  }
  if (!iEqual(name, optCombo)) {
    return OptionType::ComboBox;
  }
  if (!iEqual(name, optSpin)) {
    return OptionType::Spin;
  }
  if (!iEqual(name, optString)) {
    return OptionType::String;
  }
  return OptionType::Unknown;
}

//-----------------------------------------------------------------------------
std::string EngineOption::getTypeName(const EngineOption::OptionType type) {
  switch (type) {
  case OptionType::Button:   return optUnknown;
  case OptionType::Checkbox: return optCheck;
  case OptionType::ComboBox: return optCombo;
  case OptionType::Spin:     return optSpin;
  case OptionType::String:   return optString;
  default:
    break;
  }
  return optUnknown;
}

//-----------------------------------------------------------------------------
EngineOption::EngineOption(const std::string& optName,
                           const std::string& defaultValue,
                           const OptionType optType,
                           const int64_t minValue,
                           const int64_t maxValue,
                           const std::set<std::string>& comboValues)
  : optType(optType),
    optName(optName),
    optValue(defaultValue),
    defaultValue(defaultValue),
    minValue(minValue),
    maxValue(maxValue),
    comboValues(comboValues) {}

//-----------------------------------------------------------------------------
int64_t EngineOption::getIntValue() const {
  return toNumber<int64_t>(optValue);
}

//-----------------------------------------------------------------------------
int64_t EngineOption::getDefaultIntValue() const {
  return toNumber<int64_t>(defaultValue);
}

//-----------------------------------------------------------------------------
std::set<int64_t> EngineOption::getIntComboValues() const {
  std::set<int64_t> values;
  for (auto&& value : comboValues) {
    int64_t n = toNumber<int64_t>(value, -1);
    if (n >= 0) {
      values.insert(n);
    }
  }
  return values;
}

//-----------------------------------------------------------------------------
bool EngineOption::setValue(const std::string& value) {
  switch (optType) {
  case OptionType::Checkbox:
    if (!iEqual(value, "true") && !iEqual(value, "false")) {
      return false;
    }
    break;
  case OptionType::ComboBox:
    if (!comboValues.count(value)) {
      return false;
    }
    break;
  case OptionType::Spin: {
    int64_t intval = toNumber<int64_t>(value, (minValue - 1));
    if ((intval < minValue) || (intval > maxValue)) {
      return false;
    }
    break;
  }
  case OptionType::String:
    break;
  default:
    return false;
  }
  optValue = value;
  return true;
}

//-----------------------------------------------------------------------------
bool EngineOption::setValue(const int64_t value) {
  return setValue(std::to_string(value));
}

//-----------------------------------------------------------------------------
void EngineOption::setDefaultValue(const int64_t value) {
  setDefaultValue(std::to_string(value));
}

//-----------------------------------------------------------------------------
void EngineOption::setComboValues(const std::set<int64_t>& values) {
  comboValues.clear();
  for (auto value : values) {
    comboValues.insert(std::to_string(value));
  }
}

} // namespace senjo
