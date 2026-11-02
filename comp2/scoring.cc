#include "scoring.hpp"
#include "factors/factors.hpp"

// Survey similarity grows with the number of questions, so it is scaled to a
// 12-question survey. That keeps one answer point worth the same at every
// school, so profile factor points don't count for less at schools with
// longer surveys.
static constexpr float REFERENCE_SURVEY_QUESTIONS = 12.0f;

namespace {
    /// clamp_min(val, minval):
    ///     Set `val` to be at least `minval`.

    static inline float clamp_min(float val, float minval) noexcept {
        return (val < minval) ? minval : val;
    }
}

/// is_cheating(u1, u2, nquestions, college_dists):
///    Determine if users `u1` and `u2` are likely "cheating", i.e., have
///    suspiciously similar answers.

bool is_cheating(const User& u1, const User& u2, std::size_t nquestions,
                 const std::unordered_map<std::string, College>& college_dists) {
    if (u1.college != u2.college) {
        return false;
    }

    const auto it = college_dists.find(u1.college);
    if (it == college_dists.end()) {
        return false;
    }

    const College& college = it->second;

    std::size_t matches = 0;
    for (std::size_t i = 0; i < nquestions && i < u1.answers.size() && i < u2.answers.size(); ++i) {
        matches += (u1.answers[i] == u2.answers[i]); // bool evaluates to 1 if true
    }

    const bool mutual = (get_crush_type(u1, u2) == Crush::MUTUAL);
    const float threshold = mutual ? 6.5f : 8.5f;  // matching answers above chance
    return (static_cast<float>(matches) - college.exp_sim) > threshold;
}

/// survey_points(u1, u2, num_questions, college_dists, bio_score,
///               short_answer_score, spotify_score):
///     How similar two users' survey answers, bios, prompts and Spotify
///     artists are, in answer points.

std::optional<float> survey_points(
    const User& u1,
    const User& u2,
    std::size_t num_questions,
    const std::unordered_map<std::string, College>& college_dists,
    float bio_score,
    float short_answer_score,
    float spotify_score
) {
    if (is_cheating(u1, u2, num_questions, college_dists)) {
        return std::nullopt;
    }

    if (u1.college != u2.college) {
        return std::nullopt;
    }

    const auto it = college_dists.find(u1.college);
    if (it == college_dists.end()) {
        return std::nullopt; // unknown college
    }

    const College& college = it->second;
    const std::size_t nquestions = std::min({
        num_questions, college.num_questions,
        u1.answers.size(), u2.answers.size()
    });
    const std::size_t num_options = college.num_options;

    float ml_score   = 0.0f;
    float prop_score = 0.0f;

    const float denom_guard = clamp_min(std::sqrt(college.dist_min), 1e-6f);
    const float norm_ref    = 1.0f / denom_guard;

    for (std::size_t i = 0; i < nquestions; ++i) {
        const int a1 = u1.answers[i];
        const int a2 = u2.answers[i];

        if (a1 < 0 || a1 >= static_cast<int>(num_options) ||
            a2 < 0 || a2 >= static_cast<int>(num_options)) {
            continue;
        }

        const float p1 = college.dist(i, static_cast<std::size_t>(a1));
        if (a1 == a2) {
            // Matching answers earn more when fewer people chose that answer
            const float imp_wgt = 1.0f / (1.0f + p1);
            prop_score += (imp_wgt / clamp_min(std::sqrt(p1), 1e-6f)) / norm_ref;
            ml_score   += imp_wgt;
        } else {
            // Different answers earn partial credit for how similar they are
            const float p2 = college.dist(i, static_cast<std::size_t>(a2));
            const float imp_wgt = 1.0f / (1.0f + std::sqrt(clamp_min(p1 * p2, 0.0f)));
            ml_score += imp_wgt * college.sim(i, static_cast<std::size_t>(a1), static_cast<std::size_t>(a2));
        }
    }

    float sim_score = ml_score + prop_score + bio_score * 0.10f +
                      short_answer_score * 0.10f + spotify_score * 0.30f;
    sim_score = clamp_min(sim_score, 0.001f);

    if (num_questions == 0) {
        return sim_score;
    }
    return sim_score * (REFERENCE_SURVEY_QUESTIONS / static_cast<float>(num_questions));
}

/// profile_points(u1, u2):
///     Add up the answer points from every profile factor.

std::optional<float> profile_points(const User& u1, const User& u2) {
    if (!check_compatibility(u1, u2)) {
        return std::nullopt;
    }

    const PairContext pair = {u1, u2};
    double points = 0;
    for (const ProfileFactor& factor : profile_factors()) {
        points += factor.points(pair);
    }
    return static_cast<float>(points);
}

/// calculate_score(u1, u2, num_questions, college_dists, bio_score,
///                 short_answer_score, spotify_score):
///     A pair's final score: survey points plus profile points.

std::optional<float> calculate_score(
    const User& u1,
    const User& u2,
    std::size_t num_questions,
    const std::unordered_map<std::string, College>& college_dists,
    float bio_score,
    float short_answer_score,
    float spotify_score
) {
    const std::optional<float> profile = profile_points(u1, u2);
    if (!profile) {
        return std::nullopt;
    }
    const std::optional<float> survey =
        survey_points(u1, u2, num_questions, college_dists,
                      bio_score, short_answer_score, spotify_score);
    if (!survey) {
        return std::nullopt;
    }
    return *survey + *profile;
}
