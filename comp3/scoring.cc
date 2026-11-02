#include "lib/json.hpp"

#include "factors/factors.h"
#include "scoring.h"
#include "user.h"
#include "utils.h"


using json = nlohmann::json;

// Survey similarity grows with the number of questions (about 0.64 points per
// question for a person's best candidates). Scaling it to a 12-question survey
// keeps one answer point worth the same at every school, so profile factor
// points don't count for less at schools with longer surveys.
static const double REFERENCE_SURVEY_QUESTIONS = 12;

// TODO: Add in custom weights for specified school pairings and school-support
// (based on Supremes)

std::optional<float> profile_points(const User* u1, const User* u2,
                                    const float shipval, const float ship_mu,
                                    const float ship_sig) {

    // Check automatic no-match cases
    if (!check_compatability(u1, u2)) {
        return std::nullopt;
    }

    // Add up every factor (see factors/registry.cc for the list)
    const PairContext pair = {*u1, *u2, shipval, ship_mu, ship_sig};
    double points = 0;
    double points_before_crush = 0;  // for the TESTING log below
    for (const ProfileFactor& factor : profile_factors()) {
        if (factor.points == crush_factor) {
            points_before_crush = points;
        }
        points += factor.points(pair);
    }

    // Used for TESTING logs
    Crush ctype = getcrushtype(u1, u2);
    Logger logger = Logger::instance();
    if (ctype != NO_CRUSH || shipval > 0) {
        logger.log(
            TESTING,
            "CRUSH u1=%s u2=%s type=%d shipval=%.3f base_points=%.6f boosted_points=%.6f",
            u1->id.c_str(),
            u2->id.c_str(),
            static_cast<int>(ctype),
            shipval,
            points_before_crush,
            points
        );
    }

    return points;
}

static bool is_cheating(const User* u1, const User* u2, const size_t nquestions,
                        const std::unordered_map<std::string, College>& college_dists) {
    if (u1->college != u2->college) {
        return false;
    }

    College college = college_dists.at(u1->college);

    size_t matches = 0;
    for (size_t i = 0; i < nquestions; ++i) {
        matches += u1->answers[i] == u2->answers[i];
    }

    if (getcrushtype(u1, u2) == MUTUAL) {
        return matches - college.exp_sim > 6.5;
    }
    return matches - college.exp_sim > 8.5;
}


std::optional<float> survey_points(
    const User* u1,
    const User* u2,
    const size_t num_questions,
    const std::unordered_map<std::string, College>& college_dists,
    const float bio_score,
    const float short_answer_score,
    const float spotify_score
) {
    Logger logger = Logger::instance();
    // logger.log(INFO, "calculating score...");
    if (is_cheating(u1, u2, num_questions, college_dists)) {
        return std::nullopt;
    }
    // logger.log(INFO, "finished cheating");

    std::vector<int> answers1 = u1->answers;
    std::vector<int> answers2 = u2->answers;

    float sim_score = -1;
    if (u1->college == u2->college) {
        College college = college_dists.at(u1->college);

        // logger.log(INFO, "grabbing sims");
        Matrix3D<float> cosine_sims = college.cosine_sims;
        Matrix2D<float> answer_dist = college.answer_dist;
        float dist_min = college.dist_min;

        float ml_score = 0.0;
        float prop_score = 0.0;
        float imp_wgt = 0.0;
        for (size_t i = 0; i < num_questions; ++i) {
            // logger.log(INFO, "answer dist");
            float p1 = answer_dist(i, answers1[i]);
            // logger.log(INFO, "grabbed p1");
            if (answers1[i] == answers2[i]) {
                imp_wgt = 1 / (1 + p1);
                prop_score += (imp_wgt / sqrt(p1)) / (1 / sqrt(dist_min));
                ml_score += imp_wgt;
            } else {
                // logger.log(INFO, "different");
                float p2 = answer_dist(i, answers2[i]);
                // logger.log(INFO, "grabbed p2");
                // logger.log(INFO, "answers1 i: %d, answers2 i: %d", answers1[i], answers2[i]);
                // logger.log(INFO, "user1 email: %s", u1->email.c_str());
                imp_wgt = 1 / (1 + (sqrt(p1 * p2)));
                ml_score += imp_wgt * cosine_sims(i, answers1[i], answers2[i]);
            }
        }

        // Should this be ml_score * prop_score?
        // Bio score weight could probably be a little more scientific
        sim_score = ml_score + prop_score + bio_score * 0.1 +
            short_answer_score * 0.1 + spotify_score * 0.3;
        if (sim_score <= 0) {
            sim_score = 0.001;
        }
    } else {
        return std::nullopt;
    }

    if (num_questions == 0) {
        return sim_score;
    }
    return sim_score * (REFERENCE_SURVEY_QUESTIONS / num_questions);
}
