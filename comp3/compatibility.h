#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

// Who can be matched with whom: match type, crushes, and dealbreakers.

#include "user.h"

enum Crush {
    MUTUAL,
    ONE_WAY,
    NO_CRUSH,
};

enum MatchType {
    LOVE,
    FRIENDSHIP,
    NEITHER,
};

// LOVE if both want love with each other's gender, else FRIENDSHIP if both
// want friendship with each other's gender, else NEITHER.
MatchType getmatchtype(const User* u1, const User* u2);

// Whether u1 and u2 put each other (or one put the other) as their crush.
Crush getcrushtype(const User* u1, const User* u2);

// False if u1 and u2 must never be matched (match type, class year gap, or
// dealbreakers). Mutual crushes skip the year gap and dealbreaker checks.
bool check_compatability(const User* u1, const User* u2);

#endif
