#pragma once 

#include <vector>
#include <stdexcept>
#include <concepts>

template <typename T>
struct Compare 
{
  bool operator()(const T& a, const T& b) const
  {
    return a < b;
  }
};

template <std::copyable T, typename Compare = std::less<T>>
class Priority_Queue 
{
public:
  Priority_Queue() = default;

  explicit Priority_Queue(size_t capacity) { heap.reserve(capacity); }

  explicit Priority_Queue(const Compare& cmp) : myCompare {cmp} {}

  Priority_Queue(const Priority_Queue& other) = default;
  Priority_Queue(Priority_Queue&& other) noexcept = default;

  Priority_Queue& operator=(const Priority_Queue& other) = default;
  Priority_Queue& operator=(Priority_Queue&& other) = default;

  
  auto top() const -> const T {
    if (empty()) throw std::out_of_range("Priority queue is empty.");
    return heap.front(); }

  [[nodiscard]] constexpr auto empty() const noexcept -> bool { return heap.empty(); }
  constexpr auto size() const noexcept -> size_t { return heap.size(); }

  auto push(const T& value) -> void 
  {
    heap.push_back(value);
    int i = static_cast<int>(heap.size()) - 1;
    while (i > 0 && myCompare(heap[i], heap[(i - 1)/ 2]) )
    {
      std::swap(heap[i], heap[(i - 1) / 2] );
      i = (i - 1)/ 2;
    }
  }


  auto push(T&& value) -> void
  {
    heap.push_back(std::move(value));
    int i = static_cast<int>(heap.size()) - 1;
    while (i > 0 && myCompare(heap[i], heap[(i - 1)/ 2]))
    {
      std::swap(heap[i], heap[(i - 1) / 2] ); // percolate up
      i = (i - 1)/ 2;
    }
  }
  
  auto pop() -> void
  {
    int n = static_cast<int>(heap.size());
    if (n > 1) {
      std::swap(heap[0], heap[n - 1]);
      heap.pop_back();  
      n--;
      int i = 0;

      while (2 * i + 1 < n){
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        int smallest = left;
        if (right < n && myCompare(heap[right], heap[left])) {
          smallest = right;
        }
        if (!myCompare(heap[smallest], heap[i])) break;
        
        std::swap(heap[i], heap[smallest]); // percolate down
        i = smallest;
      }
    }
    else if (n == 1) {
      heap.pop_back();
    }
  }

private:
  std::vector<T> heap;
  Compare myCompare;
};

