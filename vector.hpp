#pragma once

#include <cstdlib>
#include <utility>
#include <limits>
#include <stdexcept>
#include <memory>
#include <algorithm>

template <typename Element, typename Alloc = std::allocator<Element>>
class Vector : private Alloc {
public:
  constexpr Vector() = default;

  constexpr Vector(size_t count) : capacity_ {count}, data_ {get_allocator().allocate(count)} {
    for (size_t i = 0; i < count; ++i){
      std::construct_at(data_ + i);
    }
    size_ = count;
  }

  constexpr Vector(const Vector& other) : capacity_ {other.capacity_}, size_ {other.size_} {
    data_ = get_allocator().allocate(capacity_);
    for (size_t i = 0; i < other.get_size(); ++i){
      std::construct_at(data_ + i, other.data_[i]);
    }
  }

  constexpr auto operator=(const Vector& other) -> Vector& {
    if (this == &other) return *this;

    clear();

    if (other.capacity_ > capacity_){
      if (data_){
        get_allocator().deallocate(data_ , capacity_);
      }
      capacity_ = other.capacity_;
      data_ = get_allocator().allocate(capacity_);
    }
    
    size_ = other.size_;
    for (size_t i = 0; i < size_; ++i){
      std::construct_at(data_ + i, other.data_[i]);
    }

    return *this;
  }

  constexpr Vector(Vector&& other) : capacity_ {other.capacity_} , size_ {other.size_}, data_ {other.data_} {
    other.data_ = nullptr;
    other.capacity_ = 0;
    other.size_ = 0;
  }

  constexpr auto operator=(Vector&& other) -> Vector& {
    if (this == &other) return *this;
    clear();
    if (data_){
      get_allocator().deallocate(data_, capacity_);
    }
      data_ = other.data_;
      capacity_ = other.capacity_;
      size_ = other.size_;

      other.data_ = nullptr;
      other.capacity_ = 0;
      other.size_ = 0;

      return *this;
  }

  constexpr ~Vector() {
    clear();
    if (data_){
      get_allocator().deallocate(data_, capacity_);
    }
  }

  constexpr auto get_allocator() -> Alloc& {
    return static_cast<Alloc&>(*this);
  }

  /* Element Access */
  constexpr auto at(size_t index) -> Element& {
    if (index >= size_){
      throw std::out_of_range{"index is out of bounds."};
    }
    return data_[index];
  }

  constexpr auto at(size_t index) const -> const Element& {
    if (index >= size_){
      throw std::out_of_range{"index is out of bounds."};
    }
    return data_[index];
  }

  constexpr auto operator[](size_t index) -> Element& { return data_[index]; }
  constexpr auto operator[](size_t index) const -> const Element& { return data_[index]; }

  constexpr auto front() -> Element& { return data_[0]; }
  constexpr auto front() const -> const Element& {return data_[0];}

  constexpr auto back() -> Element& { return data_[size_ - 1];}
  constexpr auto back() const -> const Element& { return data_[size_ - 1];}

  constexpr auto data() -> Element* { return data_; }
  constexpr auto data() const -> const Element* { return data_; }

  /* Capacity */
  constexpr auto empty() const -> bool { return size_ == 0;}

  constexpr auto get_size() const -> size_t { return size_; }

  constexpr auto max_size() const -> size_t {return std::numeric_limits<size_t>::max() / sizeof(Element); }

  constexpr auto reserve(size_t new_cap) -> void {
    if (new_cap <= capacity_ || new_cap == 0) 
      return;

    alter_capacity(new_cap);
  }

  constexpr auto get_capacity() const  -> size_t { return capacity_; }

  constexpr auto shrink_to_fit() -> void {
    if (capacity_ == size_ ) return;
    alter_capacity(size_);
  }

  /* Modifiers */
  constexpr auto clear() -> void {
    for (size_t i = 0; i < size_; ++i){
      std::destroy_at(data_ + i);
    }
    size_ = 0;
  }

  constexpr auto push_back(const Element& value) -> void {
    if (size_ == capacity_){
      alter_capacity(capacity_ == 0 ? 1 : growth_factor(capacity_));
    }
    std::construct_at(data_ + size_, value);
    ++size_;
  }

  constexpr auto push_back(Element&& value) -> void {
    if (size_ == capacity_){
      alter_capacity(capacity_ == 0 ? 1 : growth_factor(capacity_));
    }
    std::construct_at(data_ + size_, std::move(value));
    ++size_;
  }

  constexpr auto pop_back() -> void {
    if (size_ > 0){
      --size_;
      std::destroy_at(data_ + size_);
    }
  }

  constexpr auto resize(size_t count) -> void {
    if (count == size_) return;

    if (count < size_) {
      for (size_t i = count; i < size_ ; i++){
        std::destroy_at(data_ + i);
      }
    } else if (size_ < count) {
      if (capacity_ < count) {
        reserve(count);
      }
      for (size_t i = size_; i < count; ++i){
        std::construct_at(data_ + i);
      }
    }
    size_ = count;
  }

  constexpr auto resize(size_t count , const Element& value) -> void {
    if (count == size_) return;

    if (count < size_){
      for (size_t i = count; i < size_; ++i){
        std::destroy_at(data_ + i);
      }
    } else if (size_ < count){
      if (capacity_ < count) {
        reserve(count);
      }
      for (size_t i = size_; i < count; ++i){
        std::construct_at(data_ + i, value);
      }
    }
    size_ = count;
  }

  constexpr auto swap(Vector& other) noexcept -> void {
    std::swap(data_, other.data_);
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
  }

  /* Non - Member Functions */
 bool operator==(const Vector<Element, Alloc>& rhs) const {
    if (size_ != rhs.size_ ) return false;
    for (size_t i = 0; i < size_; ++i){
      if (data_[i] != rhs.data_[i]) 
        return false;
    }
    return true;
  }

  auto operator<=>(const Vector<Element, Alloc>& rhs) const {
    size_t n = std::min(size_, rhs.size_);
    for (size_t i = 0; i < n; ++i){
      auto cmp = data_[i] <=> rhs.data_[i];
      if (cmp != 0) return cmp;
    }
    return size_ <=> rhs.size_;
  }

private:
  constexpr auto alter_capacity(size_t new_capacity) -> void {
    if (new_capacity > max_size()) 
      throw std::length_error{"Capacity is greater than maximum capacity of container."};
      
    Element* new_data = get_allocator().allocate(new_capacity);
    for (size_t i = 0; i < size_; ++i){
      std::construct_at(new_data + i, std::move(data_[i]));
      std::destroy_at(data_ + i);
    }
    if (data_)
      get_allocator().deallocate(data_ ,capacity_);

    data_ = new_data;
    capacity_ = new_capacity;
  }

  constexpr auto growth_factor(size_t capacity) -> size_t { return capacity * 2; }

  size_t capacity_ {0};
  size_t size_ {0};
  Element* data_ {nullptr};
};
