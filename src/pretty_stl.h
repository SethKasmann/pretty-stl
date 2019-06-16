#pragma once

#ifndef pretty_stl
#define pretty_stl

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace pretty_stl {
namespace detail {
/*
 * Meta function to detect if any type is an iteratable stl container.
 * This is defined by any type where the following are well formed:
 *
 * 1. begin()  iterator
 * 2. end()    iterator
 * 3. cbegin() iterator
 * 4. cend()   iterator
 * 5. max_size
 * 6. empty
 *
 */
template <class C, class = void>
struct is_iteratable_stl_container : std::false_type {};
template <class C>
struct is_iteratable_stl_container<
    C, std::void_t<typename C::value_type, decltype(std::declval<C>().begin()),
                   decltype(std::declval<C>().end()),
                   decltype(std::declval<C>().cbegin()),
                   decltype(std::declval<C>().cend()),
                   decltype(std::declval<C>().max_size()),
                   decltype(std::declval<C>().empty())>> : std::true_type {};

/*
 * Specilization is made for any container that already has an
 * ostream<< implementation.
 */
template <>
struct is_iteratable_stl_container<std::string> : std::false_type {};

/*
 * Metafunction to determine if the stl container is a map
 * who's Value_type is a key value pair (std::pair)
 */
template <class C, class = void> struct has_key_value_pair : std::false_type {};
template <class C>
struct has_key_value_pair<
    C, std::void_t<decltype(std::declval<typename C::value_type>().first),
                   decltype(std::declval<typename C::value_type>().second),
                   typename C::key_type, typename C::mapped_type>>
    : std::true_type {};

} // namespace detail

/*
 * Copy a basic element to the output stream (non key value pair)
 */
template <class Ele>
void element_to_ostream(std::ostream &os, const Ele &element) {
  os << element;
}

/*
 * Specilization to copy a key value pair to the output stream.
 */
template <class Key, class Value>
void element_to_ostream(std::ostream &os, const std::pair<Key, Value> &kvp) {
  os << kvp.first << ": " << kvp.second;
}

template <class C>
void copy_elements_to_ostream(std::ostream &os, const C &container) {
  auto it = container.cbegin();
  if (it != container.cend()) {
    element_to_ostream(os, *it);
    for (++it; it != container.cend(); ++it) {
      os << ", ";
      element_to_ostream(os, *it);
    }
  }
}

template <class C> void container_front(std::ostream &os, const C &container) {
  char front = detail::has_key_value_pair<C>::value ? '{' : '[';
  os << front;
}

template <class C> void container_back(std::ostream &os, const C &container) {
  char back = detail::has_key_value_pair<C>::value ? '}' : ']';
  os << back;
}

template <class C>
std::ostream &pretty_stl_container(std::ostream &os, const C &container) {
  container_front(os, container);
  copy_elements_to_ostream(os, container);
  container_back(os, container);
  return os;
}

/*
 * Overload the output stream operator<< to capture any containers that
 * meet the requirements of the is_iteratable_stl_container meta funciton.
 */
template <class C,
          std::enable_if_t<detail::is_iteratable_stl_container<C>::value, int> = 0>
inline std::ostream &operator<<(std::ostream &os, const C &container) {
  return pretty_stl_container(os, container);
}

} // namespace pretty_stl

#endif
