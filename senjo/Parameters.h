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

#ifndef SENJO_PARAMETERS_H
#define SENJO_PARAMETERS_H

#include "Platform.h"

namespace senjo {

class Parameters : public std::list<std::string> {
public:
  Parameters() = default;
  Parameters(const std::string& commandLine);

  //---------------------------------------------------------------------------
  //! \brief Initialize this instance from the given string
  //---------------------------------------------------------------------------
  void parse(const std::string& str);

  //---------------------------------------------------------------------------
  //! \brief Join the parameters together into a single space delimited string
  //---------------------------------------------------------------------------
  std::string toString() const;

  //---------------------------------------------------------------------------
  //! \brief Test whether the specified paramName matches the first param
  //! \param[in] paramName The parameter name to check for
  //! \return true if the specified paramName matches the first param
  //---------------------------------------------------------------------------
  bool firstParamIs(const std::string& paramName) const;

  //---------------------------------------------------------------------------
  //! \brief Pop the specified paramName
  //! \param[in] paramName The parameter name to pop
  //! \return true if the specified paramName was popped
  //---------------------------------------------------------------------------
  bool popParam(const std::string& paramName);

  //---------------------------------------------------------------------------
  //! \brief Pop the specified paramName
  //! \param[in] paramName The parameter name to pop
  //! \param[out] exists Set to true if the specified paramName was popped
  //! \return true if the specified paramName was popped
  //---------------------------------------------------------------------------
  bool popParam(const std::string& paramName, bool& exists);

  //-----------------------------------------------------------------------------
  //! \brief Pop the first string
  //! \return an empty string if the parameters list is empty
  //-----------------------------------------------------------------------------
  std::string popString();

  //-----------------------------------------------------------------------------
  //! \brief Pop the specified paramName and the following string value
  //! \param[in] paramName The name of the string parameter to pop
  //! \param[out] value Populated with the string value of \p paramName
  //! \param[in] next The next expected parameter name
  //! \return true If a non-empty value was assigned to \p value
  //-----------------------------------------------------------------------------
  bool popString(const std::string& paramName, std::string& value,
                 const std::string& next = "");

  //-----------------------------------------------------------------------------
  //! \brief Pop the first param as a number
  //! \return \defaultValue if the parameters list is empty
  //!         or the first param could not be converted to the number type
  //-----------------------------------------------------------------------------
  template<typename T>
  T popNumber(const T& defaultValue = 0) {
    if (empty()) {
      return defaultValue;
    }

    std::stringstream ss(front());
    T value;
    if (ss >> value) {
      pop_front();
      return value;
    }

    return defaultValue;
  }

  //-----------------------------------------------------------------------------
  //! \brief Pop the specified paramName and the following number value
  //! \param[in] paramName The name of the numeric parameter to pop
  //! \param[out] value Populated with the numeric value of \p paramName
  //! \param[out] invalid Set to true if paramName has a non-numeric value
  //! \return true if a number value was assigned to \p number
  //-----------------------------------------------------------------------------
  template<typename T>
  bool popNumber(const std::string& paramName, T& value, bool& invalid) {
    if ((size() < 2) || !iEqual(paramName, front())) {
      return false;
    }

    pop_front();
    std::stringstream ss(front());
    if (ss >> value) {
      pop_front();
      return true;
    }

    invalid = true;
    return false;
  }

  //-----------------------------------------------------------------------------
  //! \brief Pop the specified paramName and the following number value
  //! \param[in] paramName The name of the numeric parameter to pop
  //! \param[out] value Populated with the numeric value of \p paramName
  //! \return true if a number value was assigned to \p number
  //-----------------------------------------------------------------------------
  template<typename T>
  bool popNumber(const std::string& paramName, T& value) {
    bool invalid;
    return popNumber(paramName, value, invalid);
  }
};

} // namespace senjo

#endif // SENJO_PARAMETERS_H
