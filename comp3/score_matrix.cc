#include <algorithm>
#include <set>

#include "compatibility.h"
#include "score_matrix.h"

void compute_profile_points(const SchoolInput& school, const CrushRoulette& crush,
                            ScoreMatrices& out) {
    Logger& logger = Logger::instance();
    logger.log(INFO, "Computing profile points");

    const std::vector<User>& users = school.users;
    const size_t tucount = users.size();
    out.eligible = Matrix2D<uint8_t>(tucount, tucount);
    out.profile_points = Matrix2D<float>(tucount, tucount);
    Matrix2D<int8_t>& matchtypes = out.matchtypes;
    matchtypes = Matrix2D<int8_t>(tucount, tucount);

    for (size_t i = 0; i < tucount; ++i) {
        matchtypes(i, i) = -1;
        for (size_t j = i + 1; j < tucount; ++j) {
            std::optional<float> points =
                profile_points(&users[i], &users[j], crush.shipmat(i, j),
                               crush.ship_mu, crush.ship_sig);
            if (points) {
                out.eligible(i, j) = out.eligible(j, i) = 1;
                out.profile_points(i, j) = out.profile_points(j, i) = *points;
            }

            MatchType matchtype = getmatchtype(&users[i], &users[j]);
            if (matchtype == LOVE) {
                matchtypes(i, j) = 1;
                matchtypes(j, i) = 1;
            } else if (matchtype == FRIENDSHIP) {
                matchtypes(i, j) = 0;
                matchtypes(j, i) = 0;
            } else {
                matchtypes(i, j) = -1;
                matchtypes(j, i) = -1;
            }
        }
    }
}

void apply_blocklists(const json& full, SchoolInput& school, ScoreMatrices& out) {
    Logger& logger = Logger::instance();
    logger.log(INFO, "Processing block list");

    const json& private_info = full.at("privateProfile");
    const size_t tucount = school.users.size();
    Matrix2D<int8_t>& matchtypes = out.matchtypes;

    for (size_t i = 0; i < tucount; ++i) {
        User* user = &school.users[i];
        if (private_info[user->id].contains("blocklist")) {
            std::set<std::string> blocklist;
            for (size_t j = 0;
                 j < private_info[user->id]["blocklist"].size();
                 ++j) {
                std::string email = private_info[user->id]["blocklist"][j];
                blocklist.insert(email);
                size_t blocked_idx = get_index(school.emails, email);
                user->blocklist = blocklist;
                if (blocked_idx < tucount) {
                    out.eligible(i, blocked_idx) = 0;
                    out.eligible(blocked_idx, i) = 0;
                    matchtypes(i, blocked_idx) = -1;
                    matchtypes(blocked_idx, i) = -1;
                }
            }
        }
    }
}

void compute_scores(const SchoolInput& school, const PairSims& sims,
                    const std::unordered_map<std::string, College>& college_dists,
                    ScoreMatrices& out) {
    Logger& logger = Logger::instance();
    logger.log(INFO, "Computing scores");

    const std::vector<User>& users = school.users;
    const size_t tucount = users.size();
    out.scores = Matrix2D<float>(tucount, tucount);

    for (size_t i = 0; i < tucount; ++i) {
        for (size_t j = i + 1; j < tucount; ++j) {
            if (!out.eligible(i, j)) {
                continue;
            }
            std::optional<float> survey =
                survey_points(&users[i], &users[j], school.nquestions,
                              college_dists, sims.bios(i, j),
                              sims.short_answers(i, j),
                              sims.spotify_artists(i, j));
            if (!survey) {
                out.eligible(i, j) = out.eligible(j, i) = 0;
                continue;
            }
            float score = *survey + out.profile_points(i, j);
            out.scores(i, j) = out.scores(j, i) = score;
        }
    }
}
