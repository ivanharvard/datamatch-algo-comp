#include "../scoring.hpp"
#include "expect.hpp"

// check_compatibility returns true for mutual crushes

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

    // "No matter what": a mutual crush beats the class year rule.
    u1.year = 0;
    u2.year = 3;
    expect(check_compatibility(u1, u2),
           "u1 and u2 have mutual crushes, so they should be compatible even with a class year gap of 3");

    return failures;
}