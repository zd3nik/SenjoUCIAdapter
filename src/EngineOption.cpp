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

#include "EngineOption.h"

namespace senjo {

//----------------------------------------------------------------------------
// static variables
//----------------------------------------------------------------------------
static const char* OPT_BUTTON_NAME  = "button";
static const char* OPT_CHECK_NAME   = "check";
static const char* OPT_COMBO_NAME   = "combo";
static const char* OPT_SPIN_NAME    = "spin";
static const char* OPT_STRING_NAME  = "string";
static const char* OPT_UNKNOWN_NAME = "unknown";

//----------------------------------------------------------------------------
EngineOption::OptionType EngineOption::ToOptionType(const std::string& name)
{
  if (!stricmp(name.c_str(), OPT_BUTTON_NAME)) {
    return OptionType::Button;
  }
  if (!stricmp(name.c_str(), OPT_CHECK_NAME)) {
    return OptionType::Checkbox;
  }
  if (!stricmp(name.c_str(), OPT_COMBO_NAME)) {
    return OptionType::ComboBox;
  }
  if (!stricmp(name.c_str(), OPT_SPIN_NAME)) {
    return OptionType::Spin;
  }
  if (!stricmp(name.c_str(), OPT_STRING_NAME)) {
    return OptionType::String;
  }
  return OptionType::Unknown;
}

//----------------------------------------------------------------------------
std::string EngineOption::GetTypeName(const EngineOption::OptionType type)
{
  switch (type) {
  case OptionType::Button:   return OPT_BUTTON_NAME;
  case OptionType::Checkbox: return OPT_CHECK_NAME;
  case OptionType::ComboBox: return OPT_COMBO_NAME;
  case OptionType::Spin:     return OPT_SPIN_NAME;
  case OptionType::String:   return OPT_STRING_NAME;
  default:
    break;
  }
  return OPT_UNKNOWN_NAME;
}

//----------------------------------------------------------------------------
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
    comboValues(comboValues)
{
}

//----------------------------------------------------------------------------
int64_t EngineOption::GetIntValue() const
{
  int64_t value = 0;
  sscanf(optValue.c_str(), "%" PRId64, &value);
  return value;
}

//----------------------------------------------------------------------------
int64_t EngineOption::GetDefaultIntValue() const
{
  int64_t value = 0;
  sscanf(defaultValue.c_str(), "%" PRId64, &value);
  return value;
}

//----------------------------------------------------------------------------
std::set<int64_t> EngineOption::GetIntComboValues() const
{
  std::set<int64_t> values;
  std::set<std::string>::const_iterator it;
  for (it = comboValues.begin(); it != comboValues.end(); ++it) {
    int64_t value = 0;
    sscanf(it->c_str(), "%" PRId64, &value);
    values.insert(value);
  }
  return values;
}

//----------------------------------------------------------------------------
bool EngineOption::SetValue(const std::string& value) {
  int64_t intval = 0;
  switch (optType) {
  case OptionType::Checkbox:
    if ((value != "true") && (value != "false")) {
      return false;
    }
    break;
  case OptionType::ComboBox:
    if (!comboValues.count(value)) {
      return false;
    }
    break;
  case OptionType::Spin:
    if ((sscanf(value.c_str(), "%" PRId64, &intval) != 1) ||
        (intval < minValue) ||
        (intval > maxValue))
    {
      return false;
    }
    break;
  case OptionType::String:
    break;
  default:
    return false;
  }
  optValue = value;
  return true;
}

//----------------------------------------------------------------------------
bool EngineOption::SetValue(const int64_t value)
{
  char sbuf[32];
  snprintf(sbuf, sizeof(sbuf), "%" PRId64, value);
  return SetValue(std::string(sbuf));
}

//----------------------------------------------------------------------------
void EngineOption::SetDefaultValue(const int64_t value)
{
  char sbuf[32];
  snprintf(sbuf, sizeof(sbuf), "%" PRId64, value);
  SetDefaultValue(std::string(sbuf));
}

//----------------------------------------------------------------------------
void EngineOption::SetComboValues(const std::set<int64_t>& values)
{
  char sbuf[32];
  comboValues.clear();
  std::set<int64_t>::const_iterator it;
  for (it = values.begin(); it != values.end(); ++it) {
    snprintf(sbuf, sizeof(sbuf), "%" PRId64, *it);
    comboValues.insert(std::string(sbuf));
  }
}

} // namespace senjo
