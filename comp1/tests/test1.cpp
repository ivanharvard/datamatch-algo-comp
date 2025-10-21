#include <set>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <cstdio>

bool try_propose(const int proposer, 
                 const size_t iter,
                 int** sorted_indices, 
                 int** scores, 
                 bool** matched, 
                 int* cur_poss,
                 const int ucount, 
                 std::set<int>* match_sets);
void make_matches(const int target, 
                  int** sorted_indices, 
                  int** scores,
                  bool** matched, 
                  int* cur_poss,
                  const int ucount,
                  std::set<int>* match_sets);
void output(int** scores, const int ucount, std::set<int>* match_sets);

/// handout code compiles and links with no errors

int main(void) {
    try_propose(0, 0, nullptr, nullptr, nullptr, nullptr, 0, nullptr);
    make_matches(0, nullptr, nullptr, nullptr, nullptr, 0, nullptr);
    output(nullptr, 0, nullptr);
    return 0;
}