#pragma once
#ifndef PRETTY_STL_H
#define PRETTY_STL_H

#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace pretty_stl {
namespace detail {

struct sequence_container {};
struct associative_container {};
struct tuple_container {};

// Contains 'type' to determine the container type for pretty print
template <class> struct container_traits;

template <class T, std::size_t N> struct container_traits<T[N]> {
  using type = sequence_container;
};

template <class T, std::size_t N> struct container_traits<std::array<T, N>> {
  using type = sequence_container;
};

template <class T, class A> struct container_traits<std::vector<T, A>> {
  using type = sequence_container;
};

template <class T, class A> struct container_traits<std::deque<T, A>> {
  using type = sequence_container;
};

template <class T, class A> struct container_traits<std::forward_list<T, A>> {
  using type = sequence_container;
};

template <class T, class A> struct container_traits<std::list<T, A>> {
  using type = sequence_container;
};

template <class K, class C, class A>
struct container_traits<std::set<K, C, A>> {
  using type = associative_container;
};

template <class K, class T, class C, class A>
struct container_traits<std::map<K, T, C, A>> {
  using type = associative_container;
};

template <class K, class C, class A>
struct container_traits<std::multiset<K, C, A>> {
  using type = associative_container;
};

template <class K, class T, class C, class A>
struct container_traits<std::multimap<K, T, C, A>> {
  using type = associative_container;
};

template <class K, class H, class KE, class A>
struct container_traits<std::unordered_set<K, H, KE, A>> {
  using type = associative_container;
};

template <class K, class T, class H, class KE, class A>
struct container_traits<std::unordered_map<K, T, H, KE, A>> {
  using type = associative_container;
};

template <class K, class H, class KE, class A>
struct container_traits<std::unordered_multiset<K, H, KE, A>> {
  using type = associative_container;
};

template <class K, class T, class H, class KE, class A>
struct container_traits<std::unordered_multimap<K, T, H, KE, A>> {
  using type = associative_container;
};

template <class T, class U> struct container_traits<std::pair<T, U>> {
  using type = tuple_container;
};

template <class... Args> struct container_traits<std::tuple<Args...>> {
  using type = tuple_container;
};

// Contains 'value' set to true if the container is defined as a pretty_stl
// container
template <class T, class = void>
struct is_pretty_container : public std::false_type {};

template <class T>
struct is_pretty_container<T, std::void_t<typename container_traits<T>::type>>
    : public std::true_type {};

template <>
struct is_pretty_container<std::string> : public std::false_type {
}; // std::string is explicitly removed

template <>
struct is_pretty_container<const char[]> : public std::false_type {
}; // const char[] is explicitly removed

} // namespace detail

// open_container copies the opening character to a basic ostream based on the
// container type
template <class CharT, class Traits>
void open_container(std::basic_ostream<CharT, Traits> &os,
                    detail::sequence_container) {
  os << '[';
}

template <class CharT, class Traits>
void open_container(std::basic_ostream<CharT, Traits> &os,
                    detail::associative_container) {
  os << '{';
}

template <class CharT, class Traits>
void open_container(std::basic_ostream<CharT, Traits> &os,
                    detail::tuple_container) {
  os << '(';
}

// close_container copies the closing character to a basic ostream based on the
// container type.
template <class CharT, class Traits>
void close_container(std::basic_ostream<CharT, Traits> &os,
                     detail::sequence_container) {
  os << ']';
}

template <class CharT, class Traits>
void close_container(std::basic_ostream<CharT, Traits> &os,
                     detail::associative_container) {
  os << '}';
}

template <class CharT, class Traits>
void close_container(std::basic_ostream<CharT, Traits> &os,
                     detail::tuple_container) {
  os << ')';
}

// Helper to call open_container using container traits
template <class T> void open_container_helper(std::ostream &os, const T &) {
  open_container(os, detail::container_traits<T>::type{});
}

// Helper to call close_container using container traits
template <class T> void close_container_helper(std::ostream &os, const T &t) {
  close_container(os, detail::container_traits<T>::type{});
}

// Copy std::tuple to a basic ostream using compile time recursion
template <class T, std::size_t Idx>
void pretty_tuple(std::ostream &os, const T &t,
                  std::integral_constant<std::size_t, Idx> index) {
  if constexpr (index.value < std::tuple_size<T>::value) {
    if constexpr (index.value != 0) {
      os << ", ";
    }
    os << std::get<index.value>(t);
    pretty_tuple(os, t, std::integral_constant<std::size_t, index.value + 1>{});
  }
}

// Copy all sequence_containers and associative_containers containers to a
// basic ostream.
template <class T, class CharT, class Traits, class ContainerType>
void pretty(std::basic_ostream<CharT, Traits> &os, const T &t, ContainerType) {
  auto it = std::cbegin(t);
  if (it != std::cend(t)) {
    os << *it;
  }
  for (++it; it != std::cend(t); ++it) {
    os << ", " << *it;
  }
}

// Overload for tuple_container types is necessary since they are not
// iteratable. Calls a helper method to print tuple_containers.
template <class T, class CharT, class Traits>
void pretty(std::basic_ostream<CharT, Traits> &os, const T &t,
            detail::tuple_container) {
  pretty_tuple(os, t, std::integral_constant<std::size_t, 0>{});
}

} // namespace pretty_stl

// operator<< overload for basic_ostream. Will only be enabled for stl
// containers handled by pretty_stl.
template <class T, class CharT, class Traits,
          std::enable_if_t<pretty_stl::detail::is_pretty_container<T>::value,
                           int> = 0>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const T &t) {
  using container_type = typename pretty_stl::detail::container_traits<T>::type;
  pretty_stl::open_container_helper(os, t);
  pretty_stl::pretty(os, t, container_type{});
  pretty_stl::close_container_helper(os, t);
  return os;
}

#endif
