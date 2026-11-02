#include <cstdio>

#include "../scoring.hpp"
#include "../factors/factors.hpp"
#include "expect.hpp"

// my_factor is listed in profile_factors() and gives points to some pairs

// Builds pair number `i` of a varied set of compatible-looking users, so that
// most kinds of factor (crushes, years, ages, houses, bios, prompts, rice
// purity, seriousness, answers) apply to at least some of them.
static void make_pair(int i, User& u1, User& u2) {
    u1.id = 2 * i;
    u2.id = 2 * i + 1;
    u1.gender = MALE;
    u2.gender = FEMALE;
    u1.loveprefs = {FEMALE};
    u2.loveprefs = {MALE};
    u1.friendprefs = u2.friendprefs = {MALE, FEMALE, NONBINARY, UNSPECIFIED};
    u1.college = u2.college = "Harvard";

    const char* houses[] = {"Adams", "Quincy", "Lowell", "Eliot"};
    u1.house = houses[i % 4];
    u2.house = houses[(i / 4) % 4];
    u1.year = i % 4;
    u2.year = (i / 2) % 4;
    u1.age = 18 + i % 5;
    u2.age = 18 + (i / 3) % 5;
    if (i % 7 == 0) u1.crush = u2.id;
    if (i % 11 == 0) u2.crush = u1.id;
    u1.bio = (i % 2) ? "Coffee and climbing." : "";
    u2.bio = (i % 3) ? "Chess, cooking, and long walks." : "";
    u1.has_prompts = i % 4 != 0;
    u2.has_prompts = i % 5 != 0;
    u1.ricepurity = (i % 6 == 0) ? -1.0f : float(40 + (i * 7) % 60);
    u2.ricepurity = (i % 9 == 0) ? -1.0f : float(30 + (i * 13) % 70);
    u1.seriousness = i % 4;
    u2.seriousness = (i / 4) % 4;
    for (int q = 0; q < 10; ++q) {
        u1.answers.push_back((i + q) % 5);
        u2.answers.push_back((i * q + 1) % 5);
    }
}

int main() {
    const int NUM_PAIRS = 200;

    bool registered = false;
    for (const ProfileFactor& factor : profile_factors()) {
        if (factor.points == my_factor) {
            registered = true;
        }
    }
    expect(registered,
           "my_factor isn't listed in profile_factors() in factors/registry.cc, so it never counts toward a score");

    int pairs_with_points = 0;
    bool all_finite = true;
    double lowest = 0, highest = 0;
    for (int i = 0; i < NUM_PAIRS; ++i) {
        User u1, u2;
        make_pair(i, u1, u2);
        const double points = my_factor(PairContext{u1, u2});
        if (!std::isfinite(points)) {
            all_finite = false;
            continue;
        }
        if (points != 0) {
            ++pairs_with_points;
        }
        lowest = std::min(lowest, points);
        highest = std::max(highest, points);
    }
    expect(all_finite, "my_factor returned NaN or infinity for some pairs");
    expect(pairs_with_points > 0,
           "my_factor gave 0 points to all 200 test pairs. Did you implement it?");

    std::printf("my_factor gave points to %d of %d test pairs, from %.2f to %.2f answer points\n",
                pairs_with_points, NUM_PAIRS, lowest, highest);
    return failures;
}
