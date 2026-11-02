#include "scoring.hpp"


static constexpr int YEAR_DIFF_MAX = 2; // max allowed difference in academic year

/// get_match_type(u1, u2):
///     Determine the MatchType between users `u1` and `u2`.

MatchType get_match_type(const User& u1, const User& u2) {
    const bool love = u1.loveprefs.count(u2.gender) &&
                      u2.loveprefs.count(u1.gender);

    const bool friendship = u1.friendprefs.count(u2.gender) &&
                            u2.friendprefs.count(u1.gender);

    return love       ? MatchType::LOVE
         : friendship ? MatchType::FRIENDSHIP
         : MatchType::NEITHER;
}

/// get_crush_type(u1, u2):
///     Determine the Crush status between users `u1` and `u2`.

Crush get_crush_type(const User& u1, const User& u2) {
    return (u1.crush == u2.id && u2.crush == u1.id) ? Crush::MUTUAL
         : (u1.crush == u2.id || u2.crush == u1.id) ? Crush::ONE_WAY
         :                                            Crush::NO_CRUSH;
}

/// ================================
/// TODO
/// ================================

/// check_compatibility(u1, u2):
///     Check if users `u1` and `u2` are compatible matches based on various
///     criteria specified in the README.

bool check_compatibility(const User& u1, const User& u2) {
    // TODO
    return false;
}

/// ================================
/// END OF TODO
/// ================================
