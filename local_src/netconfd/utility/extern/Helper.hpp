// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     Helper.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef UTILITY_EXTERN_HELPER_HPP_
#define UTILITY_EXTERN_HELPER_HPP_

#include <string>
#include <vector>
#include <algorithm>

template<typename T1, typename T2>
bool IsIncluded(const T1& element, const T2& vector) {
  auto it = ::std::find(vector.begin(), vector.end(), element);
  return (it != vector.end());
}

template<typename T>
bool IsNotIncluded(const ::std::string& element, const T& vector) {
  return !IsIncluded(element, vector);
}

template<typename T>
bool KeyIsNotIncluded(const ::std::string& key, const T& map){
  return (map.find(key) == map.end());
}

template<typename T1, typename T2>
void RemoveByValue(const T1& element, T2& vector){
  vector.erase(remove(vector.begin(), vector.end(), element), vector.end());
}

#endif /* UTILITY_EXTERN_HELPER_HPP_ */
//---- End of source file ------------------------------------------------------

