#ifndef CRUSH_ROULETTE_H
#define CRUSH_ROULETTE_H

// Stage 2: Crush Roulette crushes and ships for one school.

#include <cstdint>

#include "inputs.h"
#include "types.h"
#include "utils.h"

struct CrushRoulette {
    // shipmat(i, j): how many times someone else submitted i and j as a pair
    Matrix2D<uint16_t> shipmat;
    // crushmat(i, j) == 1: i submitted themselves with j
    Matrix2D<uint8_t> crushmat;
    // Mean and standard deviation of ship counts, over pairs shipped at least once
    float ship_mu = 0;
    float ship_sig = 0;
};

// Reads the "crushes" section of the Firebase export. Sets users[i].crush and
// fills in shipmat and crushmat.
void load_crushes(const json& full, SchoolInput& school, CrushRoulette& crush);

// Computes crush.ship_mu and crush.ship_sig from crush.shipmat.
void compute_ship_stats(size_t tucount, CrushRoulette& crush);

#endif
