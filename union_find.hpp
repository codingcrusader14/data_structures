#pragma once

#include <vector>

class Union_Find {
public:
  Union_Find(int n) : numSet {n} {
    parent.resize(n);
    rank.assign(n, 0);
    for (int i = 0; i < n; ++i){
      parent[i] = i;
    }
  }

  int findSet(int i){
    return (parent[i] == i) ? i : findSet(parent[i]);
  }

  bool isSameSet(int i, int j) { return findSet(i) == findSet(j); }

  void unionSet(int i, int j){
    if (isSameSet(i, j)) return;
    int x = findSet(i), y = findSet(j);
    if (rank[x] < rank[y]){
      parent[x] = y;
    } else if (rank[x] > rank[y]){
      parent[y] = x;
    } else {
      parent[y] = x;
      rank[x]++;
    }
    numSet--;
  }


  
private:
  std::vector<int> parent, rank;
  int numSet {};
};
