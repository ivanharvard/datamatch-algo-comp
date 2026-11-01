#include "../scoring.hpp"
#include "expect.hpp"

// check_compatibility returns false for the same user

int main() {
    User u1;
    User u2;

    u1.id = 1;
    u2.id = 2;

    u1.gender = MALE;
    u2.gender = FEMALE;
    u1.loveprefs.insert(FEMALE);
    u2.loveprefs.insert(MALE);

    expect(!check_compatibility(u1, u1),
           "a user should not be compatible with themselves");
    expect(check_compatibility(u1, u2),
           "two different users with matching preferences should be compatible");

    return failures;
}
