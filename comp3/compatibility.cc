#include <algorithm>
#include <cstdlib>

#include "compatibility.h"

MatchType getmatchtype(const User* u1, const User* u2) {
    if (u1->loveprefs.find(u2->gender) != u1->loveprefs.end() &&
        u2->loveprefs.find(u1->gender) != u2->loveprefs.end()) {
        return LOVE;
    }
    if (u1->friendprefs.find(u2->gender) != u1->friendprefs.end() &&
        u2->friendprefs.find(u1->gender) != u2->friendprefs.end()) {
        return FRIENDSHIP;
    }
    return NEITHER;
}

Crush getcrushtype(const User* u1, const User* u2) {
    if (u1->crush == u2->id && u2->crush == u1->id) {
        return MUTUAL;
    }
    if (u1->crush == u2->id || u2->crush == u1->id) {
        return ONE_WAY;
    }
    return NO_CRUSH;
}


bool check_compatability(const User* u1, const User* u2) {
    // Love vs. Friendship
    if (getmatchtype(u1, u2) == NEITHER) {
        return false;
    }

    if (getcrushtype(u1, u2) != MUTUAL) {
        // No freshman and senior matches allowed
        if (abs(u1->year - u2->year) > 2) {
            return false;
        }

        // Dealbreakers: age restriction
        if (u1->ageRange > 0 || u2->ageRange > 0) {
            int age_diff = abs(u1->age - u2->age);
            if (u1->ageRange > 0 &&age_diff > u1->ageRange){
                return false;
            }
            if (u2->ageRange > 0 && age_diff > u2->ageRange) {
                return false;
            }
        }

        // Dealbreakers: religion restriction
        if (u1->restrict_religion || u2->restrict_religion) {
            if (u1->religion != u2->religion){
                return false;
            }
        }

        // TODO: If either user set noDormMatch (User::no_house_matches), they
        // can't be matched with someone in the same dorm (see the README).

        if (u1->college == u2->college) {
            if (std::find(u1->blocked_houses.begin(), u1->blocked_houses.end(), u2->house) != u1->blocked_houses.end()) {
                return false;
            }
            if (std::find(u2->blocked_houses.begin(), u2->blocked_houses.end(), u1->house) != u2->blocked_houses.end()) {
                return false;
            }
        }
    }
    return true;
}
