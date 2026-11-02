#include <algorithm>
#include <array>
#include <limits>
#include <set>

#include "config.h"
#include "match_rounds.h"
#include "matching.h"

static void sponsor(const size_t a, const size_t b, Matrix2D<long long>& boostedscores) {
    if (boostedscores(a, b) < SPONSORED_CUTOFF) {
        boostedscores(a, b) += SPONSORED_CUTOFF;
    }

    if (boostedscores(b, a) < SPONSORED_CUTOFF) {
        boostedscores(b, a) += SPONSORED_CUTOFF;
    }
}

static bool comparator(std::pair<size_t, long long>& a,
                       std::pair<size_t, long long>& b) {
  return a.second < b.second;
}

// perform_matching() works on ints, with -1 meaning "can't be matched", so
// scores (answer points, possibly negative) are shifted to start at 1 and
// kept to a millionth of a point.
static const double INT_SCORE_UNITS_PER_POINT = 1000000;

bool run_match_rounds(const ScoreMatrices& scored, size_t tucount,
                      MatchResults& out) {
    Logger& logger = Logger::instance();
    const Matrix2D<float>& scores = scored.scores;

    Matrix2D<long long>& boostedscores = out.boostedscores;
    boostedscores = Matrix2D<long long>(tucount, tucount);
    out.matchesperperson.assign(tucount, {});

    Matrix2D<int> mscores(tucount, tucount);
    Matrix2D<uint8_t> matched(tucount, tucount);

    float lowest = std::numeric_limits<float>::max();
    float highest = std::numeric_limits<float>::lowest();
    for (size_t i = 0; i < tucount; ++i) {
        for (size_t j = 0; j < tucount; ++j) {
            if (scored.eligible(i, j)) {
                lowest = std::min(lowest, scores(i, j));
                highest = std::max(highest, scores(i, j));
            }
        }
    }
    if (lowest <= highest &&
        (double(highest) - lowest) * INT_SCORE_UNITS_PER_POINT >=
            std::numeric_limits<int>::max() - 1) {
        logger.log(ERROR, "Scores range from %f to %f answer points, too wide to "
                   "convert to ints for matching", lowest, highest);
        return false;
    }

    for (size_t i = 0; i < tucount; ++i) {
        for (size_t j = 0; j < tucount; ++j) {
            if (scored.eligible(i, j)) {
                mscores(i, j) = 1 + int((double(scores(i, j)) - lowest) *
                                        INT_SCORE_UNITS_PER_POINT);
            } else {
                mscores(i, j) = -1;
            }
            boostedscores(i, j) = mscores(i, j);
        }
    }

    logger.log(INFO, "Running matching now");

    perform_matching(SPONSORED, MAX_SPONSORED, mscores, matched,
                        int(tucount));
    for (size_t i = 0; i < tucount; ++i) {
        for (size_t j = i + 1; j < tucount; ++j) {
            if (matched(i, j)) {
                sponsor(i, j, boostedscores);
                mscores(i, j) = -1;
                mscores(j, i) = -1;
            }
        }
    }

    perform_matching(TOTAL - SPONSORED, MAX_TOTAL - MAX_SPONSORED,
                    mscores, matched, tucount);

    for (size_t i = 0; i < tucount; ++i) {
        for (size_t j = i + 1; j < tucount; ++j) {
            if (matched(i, j)) {
                out.matches.push_back({i, j});
                out.matchesperperson[i].push_back({j, boostedscores(i, j)});
                out.matchesperperson[j].push_back({i, boostedscores(j, i)});
                mscores(i, j) = -1;
                mscores(j, i) = -1;
            }
        }
    }
    return true;
}

void balance_sponsored_matches(const SchoolInput& school, MatchResults& out) {
    Logger& logger = Logger::instance();
    const size_t tucount = school.users.size();
    const std::vector<User>& users = school.users;
    auto& matchesperperson = out.matchesperperson;
    Matrix2D<long long>& boostedscores = out.boostedscores;
    std::vector<long long>& matchscores = out.matchscores;

    logger.log(INFO, "Handling sponsored matches");
    // Find sponsored matches
    for (size_t i = 0; i < tucount; ++i) {
        std::sort(matchesperperson[i].begin(), matchesperperson[i].end(),
                  comparator);
    }

    // Make sure everyone has 2 sponsored matches
    for (auto p : out.matches) {
        size_t u1 = p.first;
        size_t u2 = p.second;

        ssize_t idx1 = -1;
        for (size_t i = 0; i < matchesperperson[u1].size(); ++i) {
            if (matchesperperson[u1][i].first == u2) {
                idx1 = i;
                break;
            }
        }

        ssize_t idx2 = -1;
        for (size_t i = 0; i < matchesperperson[u2].size(); ++i) {
            if (matchesperperson[u2][i].first == u1) {
                idx2 = i;
                break;
            }
        }

        assert(idx1 > -1 && idx2 > -1);

        if (size_t(idx1 + SPONSORED) >= matchesperperson[u1].size() &&
            size_t(idx2 + SPONSORED) >= matchesperperson[u2].size()) {
            sponsor(u1, u2, boostedscores);
        }

        matchscores.push_back(boostedscores(u1, u2));
    }

    // TODO: Turn this into a generic helper (replace this occurence +
    // sponsmatchcounts)
    for (size_t i = 0; i < tucount; ++i) {
        if (matchesperperson[i].size() == 0) {
            logger.log(INFO, "Has 0 matches at %s: %s with index %zu",
                    school.name.c_str(), users[i].email.c_str(), i);
        }
        // assert(matchesperperson[i].size() > 0);
    }

    // TODO: Does this have to be of type unsigned long?
    std::vector<size_t> sponsmatchcounts(tucount);

    logger.log(INFO, "Balancing out sponsored matches");

    // Balance out everything
    // TODO: This hit a strange infinite loop last year and the code I
    // wrote was a little rushed, so potentially worth double checking
    size_t expandedthresh = MAX_SPONSORED + 1;
    std::array<std::set<size_t>, SPONSORED> has;
    for (size_t i = 0; i < tucount; ++i) {
        sponsmatchcounts[i] = 0;
        for (size_t j = 0; j < matchesperperson[i].size(); ++j) {
            sponsmatchcounts[i] +=
                matchesperperson[i][j].second > SPONSORED_CUTOFF;
        }

        // printf("sponsmatchcounts for %zu: %zu\n", i, sponsmatchcounts[i]);

        if (sponsmatchcounts[i] < SPONSORED) {
            has[sponsmatchcounts[i]].insert(i);
        }
    }

    size_t current = 0;
    while (current < SPONSORED) {
        while (has[current].size() > 0) {
            size_t u = *has[current].begin();
            bool expand = true;
            if (matchesperperson[u].size() == 0) {
                has[current].erase(u);
                printf("Breaking out for %zu\n", u);
                break;
            }
            for (ssize_t i = matchesperperson[u].size() - 1;
                    i >= 0;
                    --i) {
                size_t p = matchesperperson[u][i].first;
                if (sponsmatchcounts[p] < expandedthresh &&
                    boostedscores(u, p) < SPONSORED_CUTOFF) {
                    sponsor(u, p, boostedscores);
                        // maybe add this update in sponsor()
                    ++sponsmatchcounts[u];
                    ++sponsmatchcounts[p];
                    expand = false;
                    matchscores.push_back(boostedscores(u, p));
                    matchesperperson[u][i].second = boostedscores(u, p);
                    break;
                }
            }
            if (expand) {
                ++expandedthresh;
                if (expandedthresh > 5) {
                    logger.log(WARNING, "expandedthresh is now 5, so "
                        " it's likely that no balancing solution is "
                        "possible. Breaking!");
                    break;
                }
            } else {
                has[current].erase(u);
                if (current + 1 < SPONSORED) {
                    printf("inserting %zu into new level %zu\n", u,
                            current + 1);
                    has[current + 1].insert(u);
                }
            }
        }
        ++current;
    }

    logger.log(INFO, "Finished balancing");

    // TODO: crush roulette here?

    logger.log(INFO, "number of final matches %d\n", out.matches.size());

    sort(matchscores.begin(), matchscores.end());
}
