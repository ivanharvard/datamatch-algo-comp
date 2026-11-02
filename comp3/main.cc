// Entry point. Runs the algorithm for each school, in five stages:
//
//   1. inputs.cc          read input files, build the list of users
//   2. crush_roulette.cc  crushes and ships
//   3. score_matrix.cc    score every pair (math in scoring.cc and factors/)
//   4. match_rounds.cc    turn scores into matches (algorithm in matching.cc)
//   5. output.cc          write data/results/matches_<school>.json etc.
//
// Usage:
// ./main datamatch2023-prod-default-rtdb-export.json answer_choice_sims user_bio_out short_answer_sims_out spotify_artist_sims_out

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "checking.h"
#include "crush_roulette.h"
#include "inputs.h"
#include "match_rounds.h"
#include "output.h"
#include "score_matrix.h"
#include "scoring.h"
#include "utils.h"

using json = nlohmann::json;

// Folders holding the precomputed similarity files (command line arguments)
struct SimPaths {
    std::string answer_choice;
    std::string user_bio;
    std::string short_answer;
    std::string spotify_artists;
};

// Sets the log level from the ALGO_LOG_LEVEL environment variable.
// Accepts either full names (e.g., "DEBUG") or first-letter shorthands
// (e.g., "D"), case-insensitive. Defaults to INFO.
static void set_log_level_from_env(Logger& logger) {
    const char* lvl = std::getenv("ALGO_LOG_LEVEL");
    char code = (lvl && *lvl) ? std::toupper(static_cast<unsigned char>(*lvl)) : 'I';
    switch (code) {
        case 'D':
            logger.set_level(DEBUG);
            break;
        case 'W':
            logger.set_level(WARNING);
            break;
        case 'E':
            logger.set_level(ERROR);
            break;
        case 'T':
            logger.set_level(TESTING);
            break;
        case 'I':
        default:
            logger.set_level(INFO);
            break;
    }
}

// Runs every stage for one school. Returns false on a fatal error (the error
// has already been logged).
static bool run_school(const std::string& name, const json& full,
                       const SimPaths& paths,
                       std::unordered_map<std::string, College>& college_dists) {
    Logger& logger = Logger::instance();
    logger.log(INFO, "\nBeginning algorithm for %s", name.c_str());

    // Stage 1: inputs
    SchoolInput school;
    school.name = name;
    College college;
    if (!load_answer_choice_sims(paths.answer_choice, school, college)) {
        return false;
    }
    college_dists.insert({name, std::move(college)});

    load_users(full, school);
    const size_t tucount = school.users.size();
    if (!tucount) {
        logger.log(INFO, "%s has no users\n", name.c_str());
        return true;
    }

    compute_answer_distribution(school, college_dists.at(name));

    // Stage 2: Crush Roulette
    CrushRoulette crush;
    load_crushes(full, school, crush);

    logger.log(INFO, "Computing crush roulette summary statistics");
    printf("%zu\n", tucount);
    compute_ship_stats(tucount, crush);

    // Stage 3: scoring
    logger.log(INFO, "Begin scoring data");

    PairSims sims;
    logger.log(INFO, "Loading in user bio similarities");
    std::string bio_filename =
        paths.user_bio + "/user_bio_sims_" + name + ".json";
    logger.log(INFO, "user bio filename: %s", bio_filename.c_str());
    if (!load_pair_matrix(bio_filename, tucount, sims.bios)) {
        return false;
    }

    logger.log(INFO, "Loading in short answer sims");
    if (!load_pair_matrix(paths.short_answer + "/short_answer_sims_" + name + ".json",
                          tucount, sims.short_answers)) {
        return false;
    }

    logger.log(INFO, "Loading in spotify artist sims");
    if (!load_pair_matrix(paths.spotify_artists + "/spotify_artists_sims_" + name + ".json",
                          tucount, sims.spotify_artists)) {
        return false;
    }

    ScoreMatrices scored;
    compute_profile_points(school, crush, scored);
    apply_blocklists(full, school, scored);
    compute_scores(school, sims, college_dists, scored);

    // Stage 4: matching
    logger.log(INFO, "Begin writing out results and matching");
    MatchResults results;
    if (!run_match_rounds(scored, tucount, results)) {
        return false;
    }

    logger.log(INFO, "Running checking logic now");
    check_blocklist(school.users, results.matches);
    check_gender_prefs(school.users, results.matches, scored.matchtypes);

    balance_sponsored_matches(school, results);

    // Stage 5: output
    write_results(school, scored, results);
    return true;
}

int main(int argc, char** argv) {
    Logger& logger = Logger::init(Logger::create_file());
    set_log_level_from_env(logger);

    if (argc < 6) {
        logger.log(ERROR, "Not enough arguments!");
        logger.log(ERROR, "Arguments format: FIREBASE_EXPORT.json "
            "ANSWER_SIMS_PATH USER_BIO_PATH SHORT_ANSWER_PATH SPOTIFY_ARTISTS_PATH");
        return EXIT_FAILURE;
    }
    logger.log(INFO, "%d", argc);
    logger.log(INFO, "Beginning file input");

    std::ifstream userin(argv[1]);
    SimPaths paths = {argv[2], argv[3], argv[4], argv[5]};
    logger.log(INFO, "ANSWER CHOICE PATH: %s", paths.answer_choice.c_str());
    logger.log(INFO, "USER BIO PATH: %s", paths.user_bio.c_str());
    logger.log(INFO, "SHORT ANSWER PATH: %s", paths.short_answer.c_str());

    logger.log(INFO, "SPOTIFY ARTISTS PATH: %s", paths.spotify_artists.c_str());
    if (!userin) {
        logger.log(ERROR, "File %s not found", argv[1]);
        return EXIT_FAILURE;
    }

    json full;
    userin >> full;

    std::vector<std::string> required_fields = {
        "privateProfile", "publicProfile", "responses", "crushes"
    };

    if (!validate_json(full, required_fields)) {
        logger.log(ERROR, "Invalid JSON!");
        return EXIT_FAILURE;
    }

    std::vector<std::string> colleges;
    if (!load_college_names(paths.answer_choice, colleges)) {
        return EXIT_FAILURE;
    }

    make_results_dir();

    // TODO: college_dists keeps every school's College for the whole run, but
    // each school only reads its own entry. It could live in SchoolInput.
    std::unordered_map<std::string, College> college_dists;

    for (const std::string& name : colleges) {
        if (!run_school(name, full, paths, college_dists)) {
            return EXIT_FAILURE;
        }
    }

    logger.log(INFO, "Done! Saved to data/results/ folder.\n");

    Logger::shutdown();
    return 0;
}
