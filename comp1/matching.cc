#include <set>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <cstdio>

// ================================================
// Helper functions
// ================================================

/// addmatch(a, b, matched, matched_sets):
///     Add a match between individuals `a` and `b`, which updates the `matched`
///     matrix and the `match_sets` accordingly.

static void addmatch(int a, int b, bool** matched, std::set<int>* match_sets) {
    assert(!matched[a][b] && !matched[b][a]);
    matched[a][b] = matched[b][a] = true;
    match_sets[a].insert(b);
    match_sets[b].insert(a);
}

/// removematch(a, b, matched, has, match_sets):
///     Remove the match between individuals `a` and `b`, updating the `matched`
///     structure and updating `has` and `match_sets` accordingly.

static void removematch(int a, int b, bool** matched, std::set<int>* match_sets) {
    assert(matched[a][b] && matched[b][a]);
    matched[a][b] = matched[b][a] = false;
    match_sets[a].erase(b);
    match_sets[b].erase(a);
}

/// make_sortedindices(sorted_indices, scores, ucount):
///     For each individual, create a list of indices sorted by their scores.

static void make_sortedindices(int** sorted_indices, int** scores,
                               const int ucount) {
    for (int i = 0; i < ucount; ++i) {
        std::iota(sorted_indices[i], sorted_indices[i] + ucount, 0);
        // Sort candidate indices for i, descending by score[i][j].
        // If scores are equal, keep lower index first.
        std::sort(sorted_indices[i], sorted_indices[i] + ucount,
                  [&](int a, int b){
                      int sa = scores[i][a];
                      int sb = scores[i][b];
                      if (sa != sb) return sa > sb; // descending
                      return a < b;
                  });
    }
}

/// totalmatches(ucount, match_sets):
///     Calculate the total number of matches made.

static int totalmatches(const int ucount, const std::set<int>* match_sets) {
    int total = 0;
    for (int i = 0; i < ucount; ++i) {
        total += match_sets[i].size();
    }
    return total / 2; // each edge is counted twice
}

/// totalscore(scores, ucount, match_sets):
///     Calculate the total score of all matches made.

static long long totalscore(int** scores, int ucount, const std::set<int>* match_sets) {
    long long total = 0;
    for (int i = 0; i < ucount; ++i) {
        for (int j : match_sets[i]) {
            total += scores[i][j];
        }
    }
    return total / 2; // each edge is counted twice
}

/// static void output(scores, ucount, std::set<int>* match_sets):
///    Output the current matching statistics.

static void output(int** scores, const int ucount, std::set<int>* match_sets) {
    std::printf("Total matches: %d\n", totalmatches(ucount, match_sets));
    std::printf("Total match score: %lld\n", totalscore(scores, ucount, match_sets));
    std::printf("================\n");
}

// ================================================
// TODO
// ================================================

/// try_propose(proposer, iter, sorted_indices, scores, matched, cur_poss,
///             change, ucount, has, match_sets):
///     A proposer advances through their preference list and attempts to match
///     with the first feasible candidate.
///
///     Rules:
///      - If a proposer has a score of -1 with a candidate 
///        (i.e. score[proposer][candidate] == -1), they cannot propose
///        to that candidate. Skip over them.
///      - If a proposer and candidate are not currently matched, and candidate
///        has the capacity to accept a new match
///        (i.e. match_sets[candidate].size() < iter), then a match is made.
///      - Else, if the candidate is at capacity, compare scores at the candidate.
///        If the proposer is strictly better than some current partner m in
///        candidate's match set, replace them using `removematch` and a 
///        subsequent `addmatch`.
///      - Returns true if a proposal was made, false otherwise.

static bool try_propose(const int proposer, 
                        const size_t iter,
                        int** sorted_indices, 
                        int** scores, 
                        bool** matched, 
                        int* cur_poss,
                        const int ucount, 
                        std::set<int>* match_sets) {
    
    // TODO
    return false;
}

/// make_matches(target, sorted_indices, scores, matched, cur_poss,
///              ucount, match_sets):
///     Iterate proposal rounds from `iter = 1` to `iter = target`. In each
///     round, keep making proposal until a full pass produces no changes.

static void make_matches(const int target, 
                         int** sorted_indices, 
                         int** scores,
                         bool** matched, 
                         int* cur_poss,
                         const int ucount,
                         std::set<int>* match_sets) {

    // TODO
    return;
}

// ================================================
// END OF TODO 
// ================================================

static void run(int target, int limit,
                int** sorted_indices,
                int** scores,
                bool** matched,
                int* cur_poss,
                int ucount,
                std::set<int>* match_sets) {
    std::printf("Making matches: target %d (limit %d)\n", target, limit);


    // Clear state
    for (int i = 0; i < ucount; ++i) {
    match_sets[i].clear();
    for (int j = 0; j < ucount; ++j) matched[i][j] = false;
    }


    make_sortedindices(sorted_indices, scores, ucount);


    make_matches(target, sorted_indices, scores, matched, 
                 cur_poss, ucount, match_sets);


    // Sanity: no forbidden edges used when someone is totally isolated
    for (int i = 0; i < ucount; ++i) {
        for (int j = 0; j < ucount; ++j) {
            if (match_sets[i].empty() && match_sets[j].empty()) {
                assert(scores[i][j] < 0);
            }
        }
    }

    output(scores, ucount, match_sets);
}

void perform_matching(const int target, const int limit, int** scores,
                      bool** matched, const int ucount) {
    std::printf("Beginning matching...\n");


    // Allocate working memory
    auto match_sets = new std::set<int>[ucount];
    auto cur_poss = new int[ucount];
    auto sorted_indices = new int*[ucount];
    for (int i = 0; i < ucount; ++i) sorted_indices[i] = new int[ucount];


    run(target, limit, sorted_indices, scores, matched, 
        cur_poss, ucount, match_sets);


    // Cleanup
    for (int i = 0; i < ucount; ++i) delete[] sorted_indices[i];
    delete[] sorted_indices;
    delete[] cur_poss;
    delete[] match_sets;
}

int main() {
    // You can call perform_matching here if you'd like. Just make sure to
    // comment it out during testing. Otherwise, just run the tests.
    // perform_matching(-1, -1, nullptr, nullptr, 0);
}


