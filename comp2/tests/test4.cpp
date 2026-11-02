#include "../scoring.hpp"
#include "expect.hpp"

// check_compatibility returns true for mutual crushes, unless their love or
// friendship preferences don't fit each other

int main() {
    User u1;
    User u2;

    u1.gender = MALE;
    u2.gender = FEMALE;

    u1.loveprefs.insert(FEMALE);
    u2.loveprefs.insert(MALE);

    u1.id = 1;
    u2.id = 2;
    u1.crush = u2.id;
    u2.crush = u1.id;

    expect(check_compatibility(u1, u2),
           "u1 and u2 have mutual crushes, so they should be compatible");

    // A mutual crush skips the class year rule and the dealbreakers...
    u1.year = 0;
    u2.year = 3;
    expect(check_compatibility(u1, u2),
           "u1 and u2 have mutual crushes, so they should be compatible even with a class year gap of 3");

    u1.house = "Adams";
    u2.house = "Adams";
    u1.no_house_matches = true;
    expect(check_compatibility(u1, u2),
           "u1 and u2 have mutual crushes, so they should be compatible even though u1 doesn't want matches from their own house");

    // ...but not love/friendship preferences that don't fit each other.
    u2.loveprefs.clear();
    expect(!check_compatibility(u1, u2),
           "u1 and u2 have mutual crushes, but u2 isn't looking for love with men, so they shouldn't be compatible");

    return failures;
}