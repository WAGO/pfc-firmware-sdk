// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <utility>
#include <type_traits>
#include <sstream>

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


namespace details {
  template<class> struct is_ref_wrapper : std::false_type {};
  template<class T> struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type {};

  template<class B>
  struct negation : std::integral_constant<bool, !bool(B::value)> { };

  template<class T>
  using not_ref_wrapper = negation<is_ref_wrapper<std::decay_t<T>>>;


  template<class...> struct conjunction : std::true_type { };
  template<class B1> struct conjunction<B1> : B1 { };
  template<class B1, class... Bn>
  struct conjunction<B1, Bn...>
      : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};

  template <class D, class...> struct return_type_helper { using type = D; };
  template <class... Types>
  struct return_type_helper<void, Types...> : std::common_type<Types...> {
      static_assert(conjunction<not_ref_wrapper<Types>...>::value,
                    "Types cannot contain reference_wrappers when D is void");
  };

  template <class D, class... Types>
  using return_type = std::array<typename return_type_helper<D, Types...>::type,
                                 sizeof...(Types)>;
}

template < class D = void, class... Types>
constexpr details::return_type<D, Types...> make_array(Types&&... t) {
  return {{std::forward<Types>(t)... }};
}

