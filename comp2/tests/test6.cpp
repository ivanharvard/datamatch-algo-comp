#include "../scoring.hpp"
#include "expect.hpp"

// check_compatibility respects age preferences

int main() {
    User u1;
    User u2;
    User u3;
    User u4;

    u1.gender = MALE;
    u2.gender = FEMALE;
    u3.gender = MALE;
    u4.gender = MALE;

    u1.loveprefs.insert(FEMALE);
    u2.loveprefs.insert(MALE);
    u3.loveprefs.insert(MALE);
    u4.loveprefs.insert(MALE);

    u1.id = 1;
    u2.id = 2;
    u3.id = 3;
    u4.id = 4;
    u1.crush = u2.crush = u3.crush = u4.crush = -1; // no crushes

    u1.year = 0;
    u2.year = 0;
    u3.year = 0;
    u4.year = 0;


    // success
    u1.min_compatible_age = 20;
    u1.max_compatible_age = 25;
    // u2.min_compatible_age = 21;  // checks that optional age prefs work
    u2.max_compatible_age = 24;
    u1.age = 22;
    u2.age = 23;

    // failure
    u3.min_compatible_age = 21;
    u3.max_compatible_age = 21;
    u4.min_compatible_age = 20;
    u4.max_compatible_age = 23;
    u3.age = 21;
    u4.age = 20;

    expect(check_compatibility(u1, u2),
           "u1 (22, wants 20-25) and u2 (23, wants at most 24) fit each other's age preferences, so they should be compatible");
    expect(!check_compatibility(u3, u4),
           "u3 only wants age 21 but u4 is 20, so they should not be compatible");

    return failures;
}