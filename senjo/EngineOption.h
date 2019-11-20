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

#ifndef SENJO_ENGINE_OPTION_H
#define SENJO_ENGINE_OPTION_H

#include "Platform.h"

namespace senjo {

//-----------------------------------------------------------------------------
//! \brief Simple representaion of a single chess engine option
//! Provides details such as option type, name, default/min/max values.
//-----------------------------------------------------------------------------
class EngineOption {
public:
  virtual ~EngineOption() {}

  enum OptionType {
    Unknown,  ///< Unknown option type
    Button,   ///< Option does not have a value
    Checkbox, ///< Option value must be "true" or "false"
    ComboBox, ///< Option value must be one of a predefined set
    Spin,     ///< Option value must be an integer between min and max
    String    ///< Option value must be a string
  };

  //--------------------------------------------------------------------------
  //! \brief Convert option type name to OptionType
  //! \param[in] name The option type name
  //! \return Unknown if \p name is not known
  //--------------------------------------------------------------------------
  static OptionType toOptionType(const std::string& name);

  //--------------------------------------------------------------------------
  //! \brief Get a string representation of a given option type
  //! \param[in] type The option type
  //! \return A string representation of \p type
  //--------------------------------------------------------------------------
  static std::string getTypeName(const OptionType type);

  //--------------------------------------------------------------------------
  //! \brief Constructor
  //! \param[in] name The option name (default = empty)
  //! \param[in] defaultValue The option default value (default = empty)
  //! \param[in] type The option type (default = String)
  //! \param[in] minValue The min value for Spin options (default = INT64_MIN)
  //! \param[in] maxValue The max value for Spin options (default = INT64_MAX)
  //! \param[in] combo Set of legal ComboBox values (default = empty)
  //--------------------------------------------------------------------------
  EngineOption(const std::string& name = std::string(),
               const std::string& defaultValue = std::string(),
               const OptionType type = String,
               const int64_t minValue = INT64_MIN,
               const int64_t maxValue = INT64_MAX,
               const std::set<std::string>& combo = std::set<std::string>());

  //--------------------------------------------------------------------------
  //! \brief Get the option type
  //! \return The option type
  //--------------------------------------------------------------------------
  OptionType getType() const { return optType; }

  //--------------------------------------------------------------------------
  //! \brief Get a string representation of the option type
  //! \return A string representation of the option type
  //--------------------------------------------------------------------------
  virtual std::string getTypeName() const { return getTypeName(optType); }

  //--------------------------------------------------------------------------
  //! \brief Get the option name
  //! \return The option name
  //--------------------------------------------------------------------------
  virtual std::string getName() const { return optName; }

  //--------------------------------------------------------------------------
  //! \brief Get the option's current value in string form
  //! \return The option's current value in string form
  //--------------------------------------------------------------------------
  virtual std::string getValue() const { return optValue; }

  //--------------------------------------------------------------------------
  //! \brief Get the option's current value in integer form
  //! \return The option's current value in integer form, 0 if not an integer
  //--------------------------------------------------------------------------
  virtual int64_t getIntValue() const;

  //--------------------------------------------------------------------------
  //! \brief Get the option's default value in string form
  //! \return The option's default value in string form
  //--------------------------------------------------------------------------
  virtual std::string getDefaultValue() const { return defaultValue; }

  //--------------------------------------------------------------------------
  //! \brief Get the option's default value in integer form
  //! \return The option's default value in integer form, 0 if not an integer
  //--------------------------------------------------------------------------
  virtual int64_t getDefaultIntValue() const;

  //--------------------------------------------------------------------------
  //! \brief Get the option's minimum value (applicable to Spin type only)
  //! \return The option's minimum value
  //--------------------------------------------------------------------------
  virtual int64_t getMinValue() const { return minValue; }

  //--------------------------------------------------------------------------
  //! \brief Get the option's maximum value (applicable to Spin type only)
  //! \return The option's maximum value
  //--------------------------------------------------------------------------
  virtual int64_t getMaxValue() const { return maxValue; }

  //--------------------------------------------------------------------------
  //! \brief Get set of legal values for ComboBox options in string form
  //! \return set of legal values for ComboBox options in string form
  //--------------------------------------------------------------------------
  virtual const std::set<std::string>& getComboValues() const {
    return comboValues;
  }

  //--------------------------------------------------------------------------
  //! \brief Get set of legal values for ComboBox options in integer form
  //! \return set of legal values for ComboBox options in integer form
  //--------------------------------------------------------------------------
  virtual std::set<int64_t> getIntComboValues() const;

  //--------------------------------------------------------------------------
  //! \brief Set the option type
  //! \param[in] type The new option type
  //--------------------------------------------------------------------------
  virtual void setType(const OptionType type) { optType = type; }

  //--------------------------------------------------------------------------
  //! \brief Set the option name
  //! \param[in] name The new option name
  //--------------------------------------------------------------------------
  virtual void setName(const std::string& name) { optName = name; }

  //--------------------------------------------------------------------------
  //! \brief Set the option's current value
  //! \param[in] value The new option value
  //--------------------------------------------------------------------------
  virtual bool setValue(const std::string& value);

  //--------------------------------------------------------------------------
  //! \brief Set the option's current value
  //! \param[in] value The new option value
  //! \return true if \p value is valid, otherwise false
  //--------------------------------------------------------------------------
  virtual bool setValue(const int64_t value);

  //--------------------------------------------------------------------------
  //! \brief Set the option's default value
  //! \param[in] value The new default value
  //! \return true if \p value is valid, otherwise false
  //--------------------------------------------------------------------------
  virtual void setDefaultValue(const std::string& value) {
    defaultValue = value;
  }

  //--------------------------------------------------------------------------
  //! \brief Set the option's default value
  //! \param[in] value The new default value
  //--------------------------------------------------------------------------
  virtual void setDefaultValue(const int64_t value);

  //--------------------------------------------------------------------------
  //! \brief Set the option's minimum value (applicable to Spin type only)
  //! \param[in] min The new minimum value
  //--------------------------------------------------------------------------
  virtual void setMinValue(const int64_t min) { minValue = min; }

  //--------------------------------------------------------------------------
  //! \brief Set the option's maximum value (applicable to Spin type only)
  //! \param[in] max The new maximum value
  //--------------------------------------------------------------------------
  virtual void setMaxValue(const int64_t max) { maxValue = max; }

  //--------------------------------------------------------------------------
  //! \brief Set legal values for ComboBox options
  //! \param[in] values The set of legal values
  //--------------------------------------------------------------------------
  virtual void setComboValues(const std::set<std::string>& values) {
    comboValues = values;
  }

  //--------------------------------------------------------------------------
  //! \brief Set legal values for ComboBox options
  //! \param[in] values The set of legal values
  //--------------------------------------------------------------------------
  virtual void setComboValues(const std::set<int64_t>& values);

private:
  OptionType  optType;
  std::string optName;
  std::string optValue;
  std::string defaultValue;
  int64_t     minValue;
  int64_t     maxValue;
  std::set<std::string> comboValues;
};

} // namespace

#endif // SENJO_ENGINE_OPTION_H
