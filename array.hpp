#pragma once

#include <cstddef>
#include <stdexcept>
#include <iterator>
#include <concepts>
#include <type_traits>
#include <algorithm>
 
  template <typename T, size_t N>
    requires std::move_constructible<T> && std::is_move_assignable_v<T>
  struct Array {
  T array[N];

  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /* Iterators */
  constexpr auto begin() noexcept -> iterator { return array; }
  constexpr auto begin() const noexcept -> const_iterator { return array; }
  constexpr auto cbegin() const noexcept -> const_iterator { return begin(); }

  constexpr auto end() noexcept -> iterator { return array + N; }
  constexpr auto end() const noexcept -> const_iterator { return array + N; }
  constexpr auto cend() const noexcept -> const_iterator{ return end(); }

  constexpr auto rbegin() noexcept -> reverse_iterator { return reverse_iterator(end()); }
  constexpr auto rbegin() const noexcept -> const_reverse_iterator {return const_reverse_iterator(end());}
  constexpr auto crbegin() const noexcept -> const_reverse_iterator {return rbegin();}

  constexpr auto rend() noexcept -> reverse_iterator { return reverse_iterator(begin());}
  constexpr auto rend() const noexcept -> const_reverse_iterator {return const_reverse_iterator(begin());}
  constexpr auto crend() const noexcept -> const_reverse_iterator {return rend();}

  /* Element Access */
  constexpr auto at(size_t i) const -> const T& {
    if (i >= N ) throw std::out_of_range{"Size requested not a valid index."};
    return array[i];
  }

  constexpr auto at(size_t i) -> T& {
    if (i >= N || i < 0) throw std::out_of_range{"Size requested not a valid index."};
    return array[i];
  }
  
  constexpr auto operator[](size_t i) const -> const T&{ return array[i]; }
  constexpr auto operator[](size_t i) -> T& { return array[i]; }

  constexpr auto front() const -> const T&{ return array[0];}
  constexpr auto front() -> T& { return array[0];}

  
  constexpr auto back() const -> const T& { return array[N - 1]; }
  constexpr auto back() -> T&{ return array[N - 1];}

  
  constexpr auto data() noexcept -> T* { return array; }
  constexpr auto data() const noexcept -> const T* { return array; }

  /* Capacity */
  constexpr auto size() const noexcept -> size_t { return N; }

  [[nodiscard]] constexpr auto empty() const noexcept -> bool { return N == 0;}

  constexpr auto max_size() const noexcept -> size_t { return N; }

  /* Operations */

  constexpr auto fill(const T& value) -> void {
    for (size_t i = 0; i < N; ++i){
      array[i] = value;
    }
  }

  constexpr auto swap(Array& other) noexcept -> void {
    std::swap(array, other.array);
  }

  /* Non-member functions */
  auto operator<=>(const Array&) const = default;

  bool operator==(const Array&) const = default;


};
