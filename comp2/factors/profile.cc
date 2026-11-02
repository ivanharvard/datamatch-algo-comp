#include "factors.hpp"

// Answer points when both users filled in a bio, and when both answered
// prompts. How similar the bio and prompt text is gets scored separately, as
// part of survey similarity (scoring.cc).
static const double BOTH_HAVE_BIO_POINTS     = 0.38;
static const double BOTH_HAVE_PROMPTS_POINTS = 0.38;

double bio_factor(const PairContext& pair) {
    if (!pair.u1.bio.empty() && !pair.u2.bio.empty()) {
        return BOTH_HAVE_BIO_POINTS;
    }
    return 0;
}

double prompts_factor(const PairContext& pair) {
    if (pair.u1.has_prompts && pair.u2.has_prompts) {
        return BOTH_HAVE_PROMPTS_POINTS;
    }
    return 0;
}
