#include "../scoring.hpp"

// check_compatibility respects love/friendship preferences

int main() {
    User u1;
    User u2;
    User u3;
    User u4;

    u1.gender = MALE;
    u2.gender = FEMALE;
    u3.gender = MALE;
    u4.gender = MALE;

    // success
    u1.loveprefs.insert(FEMALE);
    u2.loveprefs.insert(MALE);

    // failure
    u3.friendprefs.insert(MALE);
    u4.loveprefs.insert(MALE);

    return !(!check_compatibility(u1, u2) && check_compatibility(u3, u4));
}